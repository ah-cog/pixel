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
	{ 0, 2, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 3, 6, 4, 5, 7, 8, -1, -1 },
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
		{ 5, 5, 5, 5, 5, 4, 5, 4, 4, 4, 4, 4, 4, 5, 4, 5, 5, 5, 5, 5, 5, 4, 5, 4, 5, 4, 4, 5, 4, 3, 4, 3, 4, 5, 5 },
		{ 10, 10, 10, 11, 11, 11, 11, 11, 10, 11, 11, 11, 10, 10, 10, 11, 11, 11, 11, 10, 11, 11, 11, 12, 11, 11, 11, 10, 11, 11, 12, 13, 12, 12, 12 },
		{ 229, 229, 230, 229, 229, 229, 230, 230, 229, 230, 230, 229, 230, 229, 229, 230, 230, 230, 229, 228, 229, 229, 229, 230, 229, 229, 229, 229, 228, 229, 230, 230, 228, 230, 230 }
	},
	{
		{ 20, 20, 19, 19, 20, 20, 20, 20, 21, 21, 19, 19, 21, 21, 20, 20, 20, 21, 20, 20, 20, 22, 21, 21, 21, 21, 19, 19, 19, 20, 21, 21, 20, 20, 21 },
		{ 32, 31, 30, 31, 31, 31, 32, 32, 32, 32, 31, 31, 33, 32, 32, 34, 33, 34, 35, 35, 34, 34, 33, 33, 33, 33, 31, 31, 32, 33, 33, 34, 32, 32, 32 },
		{ 228, 228, 229, 229, 227, 226, 227, 228, 227, 228, 227, 226, 226, 225, 227, 227, 227, 227, 228, 226, 226, 227, 227, 228, 228, 227, 229, 230, 229, 230, 228, 228, 228, 228, 227 }
	},
	{
		{ 6, 6, 7, -1, 1, -1, -4, 0, 7, 5, 10, 14, 17, 19, 17, 14, 12, 10, 9, 9, 5, 2, 0, 3, 5, 2, 0, -2, -2, -3, -3, -3, -4, -4, -4 },
		{ 12, 12, 18, 15, 15, 13, 13, 14, 16, 20, 25, 27, 27, 29, 29, 25, 21, 17, 17, 17, 18, 20, 20, 16, 19, 18, 18, 17, 18, 21, 22, 24, 19, 23, 26 },
		{ 230, 229, 234, 252, 285, 288, 298, 279, 266, 265, 270, 260, 248, 234, 227, 219, 214, 208, 206, 206, 206, 204, 202, 197, 192, 192, 192, 196, 197, 195, 193, 194, 197, 196, 196 }
	},
	{
		{ -1, 0, 1, 1, 2, 3, 3, 2, 1, -1, 0, 2, 4, 9, 4, 20, 22, 16, 14, 12, 28, 18, 15, 22, 12, 2, 3, 3, 12, 13, 14, 15, 12, 9, 3 },
		{ 23, 25, 27, 27, 27, 27, 25, 24, 22, 16, 16, 15, 17, 24, 18, 25, 17, 17, 14, 14, -9, -9, 3, 7, 10, 15, 12, 11, 18, 18, 10, 10, 10, 10, 9 },
		{ 209, 212, 210, 208, 205, 201, 198, 197, 195, 207, 200, 201, 197, 180, 178, 256, 251, 227, 244, 241, 310, 300, 266, 293, 265, 229, 232, 232, 259, 258, 283, 268, 251, 236, 235 }
	},
	{
		{ 16, 16, 16, 17, 16, 16, 16, 16, 15, 13, 12, 11, 12, 9, 10, 11, 15, 16, 15, 19, 21, 22, 25, 26, 26, 26, 26, 28, 27, 26, 26, 25, 23, 23, 23 },
		{ 22, 23, 23, 24, 23, 23, 23, 24, 23, 22, 25, 27, 31, 36, 48, 56, 73, 91, 106, 119, 134, 149, 165, 179, 187, 195, 200, 202, 204, 205, 206, 205, 204, 205, 206 },
		{ 227, 228, 231, 230, 228, 228, 228, 226, 229, 229, 228, 230, 228, 230, 224, 219, 207, 202, 191, 186, 176, 167, 156, 150, 144, 139, 135, 134, 132, 130, 129, 128, 129, 130, 127 }
	},
	{
		{ 28, 27, 28, 27, 27, 28, 27, 27, 27, 28, 27, 26, 24, 23, 25, 26, 26, 28, 28, 22, 18, 15, 14, 13, 14, 11, 11, 11, 10, 10, 9, 11, 11, 12, 14 },
		{ 51, 50, 49, 49, 49, 46, 47, 50, 48, 49, 47, 47, 44, 40, 32, 16, 9, -6, -20, -46, -68, -88, -101, -112, -124, -142, -155, -163, -165, -169, -173, -178, -179, -177, -178 },
		{ 223, 223, 223, 224, 224, 227, 226, 225, 223, 222, 224, 221, 217, 217, 221, 222, 224, 230, 226, 225, 214, 206, 199, 192, 185, 176, 166, 160, 158, 157, 153, 153, 152, 154, 154 }
	},
	{
		{ 21, 25, 33, 41, 45, 43, 31, 9, -12, -42, -41, -48, -9, 29, 80, 106, 108, 98, 64, 27, -24, -65, -75, -63, -5, 46, 99, 126, 126, 118, 80, 14, -46, -88, -99 },
		{ 36, 35, 33, 30, 32, 30, 38, 40, 53, 59, 58, 44, 33, 25, 3, 11, 15, 37, 55, 59, 58, 64, 69, 61, 42, 32, -7, -4, 12, 29, 39, 41, 34, 45, 64 },
		{ 255, 280, 308, 338, 348, 353, 287, 206, 98, -1, -80, -70, 18, 107, 311, 449, 494, 442, 336, 256, 118, -2, -80, -79, 40, 165, 392, 549, 537, 476, 383, 236, 84, -53, -140 }
	},
	{
		{ 12, 14, 13, 12, 11, 10, 8, 8, 10, 11, 10, 10, 10, 10, 12, 12, 10, 12, 15, 15, 24, 30, 39, -75, 9, 81, 120, 129, -71, -30, -9, 25, 30, 38, 49 },
		{ 31, 29, 26, 26, 29, 28, 27, 27, 30, 31, 30, 31, 31, 30, 33, 36, 35, 39, 49, 51, 65, 72, 81, -24, 71, 240, 161, 178, -75, -47, -46, -5, 10, 46, 67 },
		{ 232, 235, 239, 237, 236, 237, 236, 237, 233, 233, 232, 230, 228, 227, 223, 221, 222, 220, 215, 212, 210, 206, 205, 212, 203, 103, 153, 165, 194, 208, 232, 205, 214, 220, 227 }
	},
	{
		{ 54, 83, 50, 33, 33, 30, 31, 30, 29, 25, 24, 21, 19, 16, 15, 14, 14, 18, 17, 34, 12, 9, 2, 8, 29, 11, 15, 21, 16, 17, 13, 14, 12, 10, 10 },
		{ 53, 38, 39, 43, 44, 43, 40, 37, 31, 26, 22, 17, 13, 11, 3, -12, -42, -62, -97, -83, -72, -43, 15, -10, -27, 49, 35, 50, 31, 36, 36, 29, 26, 22, 18 },
		{ 349, 333, 297, 280, 268, 248, 232, 224, 220, 221, 221, 221, 219, 218, 217, 213, 206, 206, 203, 189, 204, 197, 221, 230, 233, 256, 242, 244, 236, 234, 230, 220, 216, 211, 209 }
	}
};

