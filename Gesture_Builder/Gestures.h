#define GESTURE_COUNT 9
#define AXIS_COUNT 3
#define GESTURE_SIGNATURE_MAXIMUM_SIZE 35
#define GESTURE_CANDIDATE_SIZE 35 // The number of most recent live data points to store

int classifiedGestureIndex = 0;
int previousClassifiedGestureIndex = -1;
unsigned long lastGestureClassificationTime = 0L; // Time of last gesture classification

char* gestureName[GESTURE_COUNT] = {
	"at rest, on table",
	"at rest, in hand",
	"pick up",
	"place down",
	"tilt left",
	"tilt right",
	"shake",
	"tap to another, as left",
	"tap to another, as right"
};

int gestureSignatureSize[] = { 35, 35, 35, 35, 35, 35, 35, 35, 35 };

int gestureSustainDuration[GESTURE_COUNT] = {
	0, // "at rest, on table"
	0, // "at rest, in hand"
	0, // "pick up"
	0, // "place down"
	0, // "tilt left"
	0, // "tilt right"
	0, // "shake"
	200, // "tap to another, as left"
	200  // "tap to another, as right"
};

int gestureTransitions[GESTURE_COUNT][GESTURE_COUNT] = {
	{ 1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 6, 4, 5, 7, 8, -1, -1, -1 },
	{ 1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 4, 1, -1, -1, -1, -1, -1, -1, -1 },
	{ 5, 1, -1, -1, -1, -1, -1, -1, -1 },
	{ 6, 1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, -1, -1, -1, -1, -1, -1, -1, -1 }
};
int gestureCandidateSize = 0; // Current size of the live gesture data
int gestureCandidate[AXIS_COUNT][GESTURE_CANDIDATE_SIZE] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};


int gestureSampleCount = 0;
int gestureSensorSampleCount = 0;
int gestureSignature[GESTURE_COUNT][AXIS_COUNT][GESTURE_SIGNATURE_MAXIMUM_SIZE] = {
	{
		{ 3, 3, 3, 2, 2, 3, 3, 5, 5, 5, 3, 4, 3, 3, 3, 5, 4, 4, 2, 2, 3, 4, 4, 4, 4, 4, 3, 3, 3, 2, 3, 2, 2, 4, 3 },
		{ 14, 14, 14, 13, 13, 14, 13, 13, 14, 13, 13, 14, 13, 14, 13, 13, 14, 15, 15, 14, 14, 14, 13, 13, 14, 14, 14, 13, 14, 14, 15, 13, 15, 14, 14 },
		{ 230, 230, 230, 229, 229, 229, 228, 230, 231, 230, 229, 229, 228, 228, 229, 230, 231, 231, 230, 229, 228, 227, 227, 230, 228, 230, 228, 230, 230, 229, 229, 230, 229, 230, 230 }
	},
	{
		{ 17, 17, 17, 19, 18, 19, 19, 20, 19, 19, 19, 20, 18, 17, 17, 14, 15, 16, 17, 17, 17, 18, 17, 17, 16, 15, 16, 17, 17, 17, 17, 17, 17, 17, 17 },
		{ 19, 17, 18, 16, 17, 17, 18, 18, 19, 18, 17, 18, 17, 18, 17, 16, 17, 18, 18, 17, 19, 20, 19, 19, 20, 20, 19, 20, 21, 21, 21, 19, 18, 17, 17 },
		{ 226, 227, 227, 226, 227, 228, 228, 228, 228, 230, 231, 230, 229, 230, 230, 230, 230, 230, 229, 229, 230, 230, 229, 229, 229, 229, 229, 226, 227, 226, 227, 225, 227, 227, 229 }
	},
	{
		{ 0, -1, 1, 2, 8, 2, 2, -3, -4, -1, -5, -7, -8, -9, -11, -11, -11, -10, -12, -14, -13, -12, -13, -13, -13, -15, -14, -14, -15, -13, -14, -17, -18, -17, -10 },
		{ 7, 5, 5, 15, 26, 15, 14, 15, 14, 20, 22, 22, 23, 22, 20, 19, 20, 20, 22, 22, 24, 21, 20, 19, 19, 20, 21, 25, 23, 23, 23, 23, 27, 22, 19 },
		{ 247, 254, 273, 282, 279, 287, 282, 280, 275, 263, 252, 245, 237, 229, 222, 216, 211, 209, 210, 203, 206, 202, 202, 199, 193, 185, 182, 185, 186, 183, 188, 185, 197, 199, 202 }
	},
	{
		{ 0, -1, -1, -2, -3, -2, -2, -2, -3, -4, -2, -1, 1, 4, 17, -15, -2, 1, 8, 0, 0, 4, 3, 1, 1, 3, 4, 4, 5, 7, 5, 2, 3, 4, 5 },
		{ 19, 21, 23, 25, 25, 27, 26, 24, 23, 22, 24, 24, 21, 21, -1, 34, 11, 12, 13, 14, 14, 20, 17, 14, 13, 14, 15, 15, 14, 14, 13, 15, 15, 16, 14 },
		{ 204, 204, 203, 204, 202, 198, 195, 196, 196, 194, 196, 206, 212, 220, 288, 179, 104, 196, 217, 226, 217, 222, 226, 230, 229, 229, 229, 229, 231, 230, 229, 230, 230, 228, 226 }
	},
	{
		{ -7, -6, -6, -6, -5, -6, -4, -5, -4, -6, -6, -6, -8, -9, -9, -10, -10, -13, -11, -9, -9, -9, -10, -10, -9, -10, -10, -11, -11, -10, -11, -11, -11, -11, -11 },
		{ 18, 16, 16, 16, 17, 19, 21, 23, 27, 34, 45, 65, 80, 97, 113, 125, 143, 156, 171, 185, 190, 195, 201, 204, 205, 206, 207, 208, 205, 206, 206, 206, 204, 203, 203 },
		{ 230, 229, 231, 231, 228, 229, 227, 223, 218, 216, 214, 207, 200, 197, 193, 187, 177, 173, 169, 165, 158, 152, 148, 148, 147, 148, 146, 143, 142, 139, 139, 136, 136, 137, 137 }
	},
	{
		{ -1, -1, 0, -1, -1, -1, 0, -1, -1, -3, -3, -4, -6, -5, -3, -2, -5, -11, -14, -16, -14, -11, -8, -7, -9, -9, -9, -8, -4, -4, -7, -5, -5, -6, -5 },
		{ 51, 52, 49, 49, 48, 47, 49, 52, 52, 53, 51, 46, 35, 21, 7, -7, -21, -34, -54, -79, -92, -109, -127, -140, -144, -149, -155, -166, -174, -177, -175, -175, -172, -173, -175 },
		{ 222, 222, 221, 224, 227, 229, 229, 228, 230, 237, 243, 243, 247, 248, 241, 235, 231, 220, 201, 182, 175, 170, 162, 162, 162, 157, 152, 147, 143, 143, 150, 150, 149, 149, 150 }
	},
	{
		{ 18, 44, 77, 81, 84, 68, 56, 15, -28, -44, -43, -49, -33, 37, 94, 136, 143, 147, 117, 49, -13, -46, -47, -52, -55, -4, 66, 121, 129, 157, 129, 71, 20, -17, -29 },
		{ 31, 41, 59, 68, 71, 75, 66, 46, 30, 19, 9, -6, -2, 27, 42, 58, 61, 72, 91, 77, 42, 13, 7, -3, 6, 52, 83, 86, 72, 68, 69, 70, 43, 8, 4 },
		{ 180, 241, 321, 341, 439, 412, 402, 266, 122, 1, -16, -77, -71, 67, 239, 392, 478, 556, 479, 305, 136, 1, -22, -34, -70, 30, 209, 376, 410, 526, 507, 370, 225, 79, 53 }
	},
	{
		{ 1, 0, 2, 2, 4, 4, 3, 3, 2, 2, 9, 15, 35, 61, 81, 65, 41, 20, -21, -11, -16, -16, -14, -6, -6, -3, -1, 3, 7, 12, 21, 25, 18, 18, 16 },
		{ 8, 6, 8, 9, 14, 16, 22, 26, 31, 43, 64, 89, 146, 192, 234, 352, 53, 43, -7, -15, -25, -37, -55, -22, 5, 17, 33, 45, 56, 68, 73, 70, 63, 61, 48 },
		{ 230, 231, 232, 230, 229, 231, 231, 229, 232, 232, 229, 226, 220, 210, 219, 186, 166, 224, 246, 235, 231, 221, 218, 210, 200, 200, 198, 201, 205, 209, 219, 224, 226, 227, 232 }
	},
	{
		{ 31, 30, 30, 34, 32, 32, 29, 30, 28, 28, 31, 37, 41, 51, 64, 40, 97, 127, -45, -4, -2, 16, 28, 20, 26, 28, 22, 23, 20, 20, 21, 27, 28, 29, 26 },
		{ -3, 4, 4, 1, 2, 5, 5, 6, 6, 3, -5, -24, -49, -88, -149, -338, -356, -325, -160, -87, -57, -15, 38, 69, 46, 32, 28, 15, 2, -6, -20, -29, -29, -29, -24 },
		{ 226, 229, 230, 228, 232, 232, 234, 234, 232, 230, 228, 225, 222, 212, 206, 154, 127, 148, 169, 229, 231, 237, 224, 218, 215, 221, 217, 216, 210, 207, 209, 211, 213, 216, 215 }
	}
};
