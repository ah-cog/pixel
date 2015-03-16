/*
  "Pixel" Firmware (for Gestural Language), Rendition 2
  
  Creator: Michael Gubbels

  "The reasonable man adapts himself to the world;
   the unreasonable one persists in trying to adapt the world to himself.
   Therefore all progress depends on the unreasonable man."
   
   - George Bernard Shaw
     Man and Superman (1903) "Maxims for Revolutionists"
     
   - Don't focus on difference. Focus on diversity.

*/

#include <EEPROM.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <SPI.h>

String ipAddress = "unassigned";

#include <Adafruit_NeoPixel.h>

#include "Utilities.h"
#include "Platform.h"
#include "Color.h"
#include "Sound.h"
#include "Motion.h"
#include "Memory.h"
#include "Communication.h"

// The module has focus (or is "active") and will receive commands (e.g., from Looper). If there are multiple active modules, interactively choose one (or say "both").
// TODO: Send message to Looper to let it know which module is active so Looper can show it.
// TODO: Implement the "focus" behavior in Looper itself 
//boolean hasFocus = false;
//unsigned long focusStartTime = 0L;
//#define MAX_FOCUS_DURATION 10000

//void Start_Focus () {
//  Serial.println ("Starting Focus");
//  focusStartTime = millis ();
//  hasFocus = true;
//  
//  // Set focus color
//  Update_Brightness (1.0);
//  
//  Queue_Message (platformUuid, BROADCAST_ADDRESS, "notice start focus"); // TODO: remember focus 234
//}
//
//void Continue_Focus () {
//}

//boolean Update_Focus () {
//  if (hasFocus) {
//    unsigned long currentTime = millis (); // TODO: Replace with global timescale
//    if (currentTime > focusStartTime + MAX_FOCUS_DURATION) {
//      Stop_Focus ();
//    } else {
//      Continue_Focus ();
//    }
//  }
//}

//void Stop_Focus () {
//  Serial.println ("Stopping Focus");
//  hasFocus = false;
//  
//  // Set default color
//  Update_Brightness (0.125);
//  
//  Queue_Message (platformUuid, BROADCAST_ADDRESS, "notice stop focus"); // TODO: remember focus 234
//}

#include "Gesture.h"
#include "Movement.h"
#include "Looper.h"
#include "Serial.h"
#include "Ports.h"

char* adverbs[331] = { "abnormally", "absentmindedly", "accidentally", "acidly", "actually", "adventurously", "afterwards", "almost", "always", "angrily", "annually", "anxiously", "arrogantly", "awkwardly", "badly", "bashfully", "beautifully", "bitterly", "bleakly", "blindly", "blissfully", "boastfully", "boldly", "bravely", "briefly", "brightly", "briskly", "broadly", "busily", "calmly", "carefully", "carelessly", "cautiously", "certainly", "cheerfully", "clearly", "cleverly", "closely", "coaxingly", "colorfully", "commonly", "continually", "coolly", "correctly", "courageously", "crossly", "cruelly", "curiously", "daily", "daintily", "dearly", "deceivingly", "delightfully", "deeply", "defiantly", "deliberately", "delightfully", "diligently", "dimly", "doubtfully", "dreamily", "easily", "elegantly", "energetically", "enormously", "enthusiastically", "equally", "especially", "even", "evenly", "eventually", "exactly", "excitedly", "extremely", "fairly", "faithfully", "famously", "far", "fast", "fatally", "ferociously", "fervently", "fiercely", "fondly", "foolishly", "fortunately", "frankly", "frantically", "freely", "frenetically", "frightfully", "fully", "furiously", "generally", "generously", "gently", "gladly", "gleefully", "gracefully", "gratefully", "greatly", "greedily", "happily", "hastily", "healthily", "heavily", "helpfully", "helplessly", "highly", "honestly", "hopelessly", "hourly", "hungrily", "immediately", "innocently", "inquisitively", "instantly", "intensely", "intently", "interestingly", "inwardly", "irritably", "jaggedly", "jealously", "joshingly", "joyfully", "joyously", "jovially", "jubilantly", "judgementally", "justly", "keenly", "kiddingly", "kindheartedly", "kindly", "kissingly", "knavishly", "knottily", "knowingly", "knowledgeably", "kookily", "lazily", "less", "lightly", "likely", "limply", "lively", "loftily", "longingly", "loosely", "lovingly", "loudly", "loyally", "madly", "majestically", "meaningfully", "mechanically", "merrily", "miserably", "mockingly", "monthly", "more", "mortally", "mostly", "mysteriously", "naturally", "nearly", "neatly", "needily", "nervously", "never", "nicely", "noisily", "not", "obediently", "obnoxiously", "oddly", "offensively", "officially", "often", "only", "openly", "optimistically", "overconfidently", "owlishly", "painfully", "partially", "patiently", "perfectly", "physically", "playfully", "politely", "poorly", "positively", "potentially", "powerfully", "promptly", "properly", "punctually", "quaintly", "quarrelsomely", "queasily", "queerly", "questionably", "questioningly", "quicker", "quickly", "quietly", "quirkily", "quizzically", "rapidly", "rarely", "readily", "really", "reassuringly", "recklessly", "regularly", "reluctantly", "repeatedly", "reproachfully", "restfully", "righteously", "rightfully", "rigidly", "roughly", "rudely", "sadly", "safely", "scarcely", "scarily", "searchingly", "sedately", "seemingly", "seldom", "selfishly", "separately", "seriously", "shakily", "sharply", "sheepishly", "shrilly", "shyly", "silently", "sleepily", "slowly", "smoothly", "softly", "solemnly", "solidly", "sometimes", "soon", "speedily", "stealthily", "sternly", "strictly", "successfully", "suddenly", "surprisingly", "suspiciously", "sweetly", "swiftly", "sympathetically", "tenderly", "tensely", "terribly", "thankfully", "thoroughly", "thoughtfully", "tightly", "tomorrow", "too", "tremendously", "triumphantly", "truly", "truthfully", "ultimately", "unabashedly", "unaccountably", "unbearably", "unethically", "unexpectedly", "unfortunately", "unimpressively", "unnaturally", "unnecessarily", "utterly", "upbeat", "upliftingly", "upright", "upside-down", "upward", "upwardly", "urgently", "usefully", "uselessly", "usually", "utterly", "vacantly", "vaguely", "vainly", "valiantly", "vastly", "verbally", "very", "viciously", "victoriously", "violently", "vivaciously", "voluntarily", "warmly", "weakly", "wearily", "well", "wetly", "wholly", "wildly", "willfully", "wisely", "woefully", "wonderfully", "worriedly", "wrongly", "yawningly", "yearly", "yearningly", "yesterday", "yieldingly", "youthfully", "zealously", "zestfully", "zestily" };
char* nouns[975] = { "account", "achiever", "acoustics", "act", "action", "activity", "actor", "addition", "adjustment", "advertisement", "advice", "aftermath", "afternoon", "afterthought", "agreement", "air", "airplane", "airport", "alarm", "amount", "amusement", "anger", "angle", "animal", "answer", "ant", "ants", "apparatus", "apparel", "apple", "apples", "appliance", "approval", "arch", "argument", "arithmetic", "arm", "army", "art", "attack", "attempt", "attention", "attraction", "aunt", "authority", "babies", "baby", "back", "badge", "bag", "bait", "balance", "ball", "balloon", "balls", "banana", "band", "base", "baseball", "basin", "basket", "basketball", "bat", "bath", "battle", "bead", "beam", "bean", "bear", "bears", "beast", "bed", "bedroom", "beds", "bee", "beef", "beetle", "beggar", "beginner", "behavior", "belief", "believe", "bell", "bells", "berry", "bike", "bikes", "bird", "birds", "birth", "birthday", "bit", "bite", "blade", "blood", "blow", "board", "boat", "boats", "body", "bomb", "bone", "book", "books", "boot", "border", "bottle", "boundary", "box", "boy", "boys", "brain", "brake", "branch", "brass", "bread", "breakfast", "breath", "brick", "bridge", "brother", "brothers", "brush", "bubble", "bucket", "building", "bulb", "bun", "burn", "burst", "bushes", "business", "butter", "button", "cabbage", "cable", "cactus", "cake", "cakes", "calculator", "calendar", "camera", "camp", "can", "cannon", "canvas", "cap", "caption", "car", "card", "care", "carpenter", "carriage", "cars", "cart", "cast", "cat", "cats", "cattle", "cause", "cave", "celery", "cellar", "cemetery", "cent", "chain", "chair", "chairs", "chalk", "chance", "change", "channel", "cheese", "cherries", "cherry", "chess", "chicken", "chickens", "children", "chin", "church", "circle", "clam", "class", "clock", "clocks", "cloth", "cloud", "clouds", "clover", "club", "coach", "coal", "coast", "coat", "cobweb", "coil", "collar", "color", "comb", "comfort", "committee", "company", "comparison", "competition", "condition", "connection", "control", "cook", "copper", "copy", "cord", "cork", "corn", "cough", "country", "cover", "cow", "cows", "crack", "cracker", "crate", "crayon", "cream", "creator", "creature", "credit", "crib", "crime", "crook", "crow", "crowd", "crown", "crush", "cry", "cub", "cup", "current", "curtain", "curve", "cushion", "dad", "daughter", "day", "death", "debt", "decision", "deer", "degree", "design", "desire", "desk", "destruction", "detail", "development", "digestion", "dime", "dinner", "dinosaurs", "direction", "dirt", "discovery", "discussion", "disease", "disgust", "distance", "distribution", "division", "dock", "doctor", "dog", "dogs", "doll", "dolls", "donkey", "door", "downtown", "drain", "drawer", "dress", "drink", "driving", "drop", "drug", "drum", "duck", "ducks", "dust", "ear", "earth", "earthquake", "edge", "education", "effect", "egg", "eggnog", "eggs", "elbow", "end", "engine", "error", "event", "example", "exchange", "existence", "expansion", "experience", "expert", "eye", "eyes", "face", "fact", "fairies", "fall", "family", "fan", "fang", "farm", "farmer", "father", "father", "faucet", "fear", "feast", "feather", "feeling", "feet", "fiction", "field", "fifth", "fight", "finger", "finger", "fire", "fireman", "fish", "flag", "flame", "flavor", "flesh", "flight", "flock", "floor", "flower", "flowers", "fly", "fog", "fold", "food", "foot", "force", "fork", "form", "fowl", "frame", "friction", "friend", "friends", "frog", "frogs", "front", "fruit", "fuel", "furniture", "alley", "game", "garden", "gate", "geese", "ghost", "giants", "giraffe", "girl", "girls", "glass", "glove", "glue", "goat", "gold", "goldfish", "good-bye", "goose", "government", "governor", "grade", "grain", "grandfather", "grandmother", "grape", "grass", "grip", "ground", "group", "growth", "guide", "guitar", "gun ", "hair", "haircut", "hall", "hammer", "hand", "hands", "harbor", "harmony", "hat", "hate", "head", "health", "hearing", "heart", "heat", "help", "hen", "hill", "history", "hobbies", "hole", "holiday", "home", "honey", "hook", "hope", "horn", "horse", "horses", "hose", "hospital", "hot", "hour", "house", "houses", "humor", "hydrant", "ice", "icicle", "idea", "impulse", "income", "increase", "industry", "ink", "insect", "instrument", "insurance", "interest", "invention", "iron", "island", "jail", "jam", "jar", "jeans", "jelly", "jellyfish", "jewel", "join", "joke", "journey", "judge", "juice", "jump", "kettle", "key", "kick", "kiss", "kite", "kitten", "kittens", "kitty", "knee", "knife", "knot", "knowledge", "laborer", "lace", "ladybug", "lake", "lamp", "land", "language", "laugh", "lawyer", "lead", "leaf", "learning", "leather", "leg", "legs", "letter", "letters", "lettuce", "level", "library", "lift", "light", "limit", "line", "linen", "lip", "liquid", "list", "lizards", "loaf", "lock", "locket", "look", "loss", "love", "low", "lumber", "lunch", "lunchroom", "machine", "magic", "maid", "mailbox", "man", "manager", "map", "marble", "mark", "market", "mask", "mass", "match", "meal", "measure", "meat", "meeting", "memory", "men", "metal", "mice", "middle", "milk", "mind", "mine", "minister", "mint", "minute", "mist", "mitten", "mom", "money", "monkey", "month", "moon", "morning", "mother", "motion", "mountain", "mouth", "move", "muscle", "music", "nail", "name", "nation", "neck", "need", "needle", "nerve", "nest", "net", "news", "night", "noise", "north", "nose", "note", "notebook", "number", "nut", "oatmeal", "observation", "ocean", "offer", "office", "oil", "operation", "opinion", "orange", "oranges", "order", "organization", "ornament", "oven", "owl", "owner", "page", "pail", "pain", "paint", "pan", "pancake", "paper", "parcel", "parent", "park", "part", "partner", "party", "passenger", "paste", "patch", "payment", "peace", "pear", "pen", "pencil", "person", "pest", "pet", "pets", "pickle", "picture", "pie", "pies", "pig", "pigs", "pin", "pipe", "pizzas", "place", "plane", "planes", "plant", "plantation", "plants", "plastic", "plate", "play", "playground", "pleasure", "plot", "plough", "pocket", "point", "poison", "police", "polish", "pollution", "popcorn", "porter", "position", "pot", "potato", "powder", "power", "price", "print", "prison", "process", "produce", "profit", "property", "prose", "protest", "pull", "pump", "punishment", "purpose", "push", "quarter", "quartz", "queen", "question", "quicksand", "quiet", "quill", "quilt", "quince", "quiver ", "rabbit", "rabbits", "rail", "railway", "rain", "rainstorm", "rake", "range", "rat", "rate", "ray", "reaction", "reading", "reason", "receipt", "recess", "record", "regret", "relation", "religion", "representative", "request", "respect", "rest", "reward", "rhythm", "rice", "riddle", "rifle", "ring", "rings", "river", "road", "robin", "rock", "rod", "roll", "roof", "room", "root", "rose", "route", "rub", "rule", "run", "sack", "sail", "salt", "sand", "scale", "scarecrow", "scarf", "scene", "scent", "school", "science", "scissors", "screw", "sea", "seashore", "seat", "secretary", "seed", "selection", "self", "sense", "servant", "shade", "shake", "shame", "shape", "sheep", "sheet", "shelf", "ship", "shirt", "shock", "shoe", "shoes", "shop", "show", "side", "sidewalk", "sign", "silk", "silver", "sink", "sister", "sisters", "size", "skate", "skin", "skirt", "sky", "slave", "sleep", "sleet", "slip", "slope", "smash", "smell", "smile", "smoke", "snail", "snails", "snake", "snakes", "sneeze", "snow", "soap", "society", "sock", "soda", "sofa", "son", "song", "songs", "sort", "sound", "soup", "space", "spade", "spark", "spiders", "sponge", "spoon", "spot", "spring", "spy", "square", "squirrel", "stage", "stamp", "star", "start", "statement", "station", "steam", "steel", "stem", "step", "stew", "stick", "sticks", "stitch", "stocking", "stomach", "stone", "stop", "store", "story", "stove", "stranger", "straw", "stream", "street", "stretch", "string", "structure", "substance", "sugar", "suggestion", "suit", "summer", "sun", "support", "surprise", "sweater", "swim", "swing", "system", "table", "tail", "talk", "tank", "taste", "tax", "teaching", "team", "teeth", "temper", "tendency", "tent", "territory", "test", "texture", "theory", "thing", "things", "thought", "thread", "thrill", "throat", "throne", "thumb", "thunder", "ticket", "tiger", "time", "tin", "title", "toad", "toe", "toes", "tomatoes", "tongue", "tooth", "toothbrush", "toothpaste", "top", "touch", "town", "toy", "toys", "trade", "trail", "train", "trains", "tramp", "transport", "tray", "treatment", "tree", "trees", "trick", "trip", "trouble", "trousers", "truck", "trucks", "tub", "turkey", "turn", "twig", "twist", "umbrella", "uncle", "underwear", "unit", "use", "vacation", "value", "van", "vase", "vegetable", "veil", "vein", "verse", "vessel", "vest", "view", "visitor", "voice", "volcano", "volleyball", "voyage", "walk", "wall", "war", "wash", "waste", "watch", "water", "wave", "waves", "wax", "way", "wealth", "weather", "week", "weight", "wheel", "whip", "whistle", "wilderness", "wind", "window", "wine", "wing", "winter", "wire", "wish", "woman", "women", "wood", "wool", "word", "work", "worm", "wound", "wren", "wrench", "wrist", "writer", "writing", "yak", "yam", "yard", "yarn", "year", "yoke" };

String name = "module"; // TODO: Move this to Foundation/Platform (next to the UUID)

// TODO: Create Perspective or Focus class
int messageTargetModule = -1; // i.e., the module to which commands will be sent by default (initially, this is set to the module connected via USB)
int messageSourceModule = -1; // i.e., the module from which commands are being sent, to which responses should be sent

#include "Foundation.h" // i.e., the kernel
#include "Language.h" // i.e., the shell

boolean hasGestureProcessed = false;

String Generate_Name () {
  String name = String (adverbs[random(331)]) + "-" + String (nouns[random(975)]);
  return name;
}

// Messaging
// - based on contemporary text messaging
// - @ mention to specific module, to specific groups, broadcast to all modules
// - # hashtag comments
// - "enter" another module, group, or all modules (i.e., broadcast/universal set)
// - "exit" another module, group, all modules (if in one)
//
// e.g., As sent from a module:
// intent: play sound 1050 400
// message: @52 play sound 1050 400 #wtf
// message: @52 @52 @43 play sound 1050 400
// broadcast: @all play sound 1050 400
//
// e.g., As received on a module:
// intent: 34: play sound 1050 400
// message: fuzzy-bandit: @52 play sound 1050 400 #wtf
// message: 55: @52 @52 @43 play sound 1050 400
// broadcast: 34: 33@ @all play sound 1050 400

/**
 * Module configuration
 */

void setup () {
  
  // set up the pseudorandom number generator
  randomSeed (analogRead (0));
  
  name = Generate_Name (); // TODO: If handle already exists, then use that one. Only generate if need a unique one WRT the scope of the network.
  
  setupCommunication ();
  setupBridge ();
  
  if (hasFoundationUuid == false) {
    setupFoundation ();
    hasFoundationUuid = true;
  } // The layer on which the "platform" depends
  
  Setup_Looper (); // set the Looper engine up
  
  setupPlatformUuid ();
//  setupPlatform(); // Setup Pixel's reflection (i.e., it's virtual machine)

  // Set up defualt perspective
  messageTargetModule = platformUuid;

  Setup_Ports (); // Setup pin mode for I/O
  setupColor (); // Setup the Pixel's color
  
  // TODO: Read device GUID from EEPROM if it exists, otherwise generate one, store it in EEPROM, and read it.

  // update the module's color
  //setModuleColor(random(256), random(256), random(256)); // Set the module's default color
  Update_Module_Color (32, 32, 32);
  
  // assign the module a color uniquely
//  Update_Color (defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
  
  // let the machine hardware stabilize
  delay (2000);
  
  // hello
  Serial.begin (115200); 
  Serial.println (F ("This is Pixel"));
  
  Serial.print ("Module name is ");
  Serial.print (name);
  Serial.print ("\n");
  
  // set the platform up
  setupPlatform (); // Setup Pixel's reflection (i.e., it's virtual machine)
  
  // set up the physical orientation sensing peripherals (i.e., the inertial measurement unit)
  setupOrientationSensing ();
  setupGestureSensing ();

  // set the sound up
  setupSound ();
  
  // blink the light
  Blink_Light (3);
  
  
  Move_Motion (0.5 * 1000);
  
//  Serial.print ("Foundation UUID: "); for (int i = 0; i < UUID_SIZE; i++) { Serial.print ((char) foundationUuid[i]); } Serial.print ("\n");
}

void loop () {
  
//  Update_Focus (); // Update focus (this is part of the gestural language)
  
  // Get "resource" sample
  Recieve_Serial_Message (); // Get any text received over the serial port (e.g., from the Arduino IDE)
  
  // Broadcast Presence
  if (hasPlatformUuid) { 

    // Broadcast device's address (UUID)
    unsigned long currentTime = millis ();
    if (currentTime - lastBroadcastTime > broadcastTimeout) {
      
//      if (isReading == false) {
//        isWriting = true;

        // MESH_SERIAL.write ('!');
//        String data = String ("{ uuid: ") + String (platformUuid) + String (" , type: 'keep-alive' }");
//        const int serialBufferSize = 64;
//        char charData[serialBufferSize];
//        data.toCharArray (charData, serialBufferSize);
        
        // TODO: Put this in Send_Message ("fyi active");
        int bytesSent = 0;
        String data = "";
        if (classifiedGestureIndex != previousClassifiedGestureIndex) {
//          data = String ("(") + String (BROADCAST_ADDRESS) + String (", ") + String (platformUuid) + String (", ") + String (ANNOUNCE_POOF) + String (")");
//          const int serialBufferSize = 64;
//          char charData[serialBufferSize];
//          data.toCharArray (charData, serialBufferSize);
//          bytesSent = MESH_SERIAL.write (charData);
          Queue_Message (platformUuid, BROADCAST_ADDRESS, "notice appearance");
        }
        
//        Serial.println (charData);
        
//        int bytesSent = MESH_SERIAL.write (charData);
//        Serial.print ("sent "); Serial.print (bytesSent); Serial.print (" bytes\n\n");
        
        lastBroadcastTime = millis ();
        
//        if (bytesSent >= data.length ()) {
//          isWriting = false;
//        }
//      }
    }
  }
  
  // TODO: Broadcast the foundation's default device address (upon boot)
  
  // TODO: Negotiate a unique dynamic mesh address
 
  // TODO: Broadcast the platform's dynamic mesh address (once per 30 seconds or so)
  
  Perform_Light_Behavior ();
  
  lastInputValue = touchInputMean;
  
  // Get module's input
  Get_Input_Port_Continuous ();
  // Serial.println(touchInputMean); // Output value for debugging (or manual calibration)
  
  // TODO: Update the following input method to be one of multiple that can be swapped in an out depending on the type of input connected
  
  int inputType = 1; // e.g., 1 = INPUT_TYPE_CAPACITATIVE_SWITCH, 2 = INPUT_TYPE_RESISTIVE_SWITCH, 3 = INPUT_TYPE_DIGITAL_SWITCH
  
  if (inputType == 1) {
    
    if (touchInputMean > 3000 && lastInputValue <= 3000) { // Check if state changed to "pressed" from "not pressed"
    
      // Update input pin value to low (off)
      Channel* moduleInputChannel = Get_Channel (platform, MODULE_INPUT_PIN);
      Update_Channel_Value (moduleInputChannel, PIN_VALUE_HIGH);
      Propagate_Channel_Value (moduleInputChannel);
    
      // Update output pin value to low (off)
      if (outputPinRemote == false) {
        // Output port is on this module!
        //Update_Channel_Value (MODULE_OUTPUT_PIN, PIN_VALUE_HIGH);
        Channel* moduleOutputChannel = Get_Channel (platform, MODULE_OUTPUT_PIN);
        Update_Channel_Value (moduleOutputChannel, PIN_VALUE_HIGH);
        Propagate_Channel_Value (moduleOutputChannel);
        Blink_Light (2);      
      } else {
        // Output port is on a different module than this one!
        Blink_Light (3); // BUG! wrong number
//        Queue_Message (BROADCAST_ADDRESS, ACTIVATE_MODULE_OUTPUT);
        Queue_Message (platformUuid, BROADCAST_ADDRESS, String ("turn output on"));
      }
//      delay(500);
    } else if (touchInputMean <= 3000 && lastInputValue > 3000) { // Check if state changed to "not pressed" from "pressed"
    
      // Update input pin value to high (on)
      Channel* moduleInputChannel = Get_Channel (platform, MODULE_INPUT_PIN);
      Update_Channel_Value (moduleInputChannel, PIN_VALUE_LOW);
      Propagate_Channel_Value (moduleInputChannel);
    
      // Update output pin value to high (on)
      if (outputPinRemote == false) {
//        Update_Channel_Value (MODULE_OUTPUT_PIN, PIN_VALUE_LOW);
//        syncPinValue(MODULE_OUTPUT_PIN);
        Channel* moduleOutputChannel = Get_Channel (platform, MODULE_OUTPUT_PIN);
        Update_Channel_Value (moduleOutputChannel, PIN_VALUE_LOW);
        Propagate_Channel_Value (moduleOutputChannel);
      } else {
//        Queue_Message (BROADCAST_ADDRESS, DEACTIVATE_MODULE_OUTPUT);
        Queue_Message (platformUuid, BROADCAST_ADDRESS, String ("turn output off"));
      }
//      delay(500);
    }
  }

  // TODO: Send updated state of THIS board (master) to the OTHER board (slave) for caching.
    
  // Get behavior updates from slave
  Get_Behavior_Transformations ();
  
  // Perform behavior step in the interpreter (Evaluate)
  // TODO: Transform_Behavior (i.e., the Behavior Transformer does this, akin to interpreting an instruction/command)
  boolean performanceResult = Perform_Behavior (performer);
  
  
  
  ////// GESTURE/MODULE STUFF (MASTER)
  
  // TODO: Add "getPins" function to read the state of pins, store the state of the pins, and handle interfacing with the pins (reading, writing), based on the program running (in both Looper and Mover).
  // TODO: Write code to allow Processing sketch (or other software) to automatically connect to this serial port. Send a "waiting for connection" signal one per second (or thereabout).
  
//  if (getPreviousModuleCount()) {
//    Serial.print("Previous modules: ");
//    Serial.print(getPreviousModuleCount());
//    Serial.println();
//  }
//  
//  if (getNextModuleCount ()) {
//    Serial.print ("Next modules: ");
//    Serial.print (getNextModuleCount());
//    Serial.println ();
//  }

  // Change color/light if needed
  if (crossfadeStep < 256) {
    Light_Apply_Color ();
  }
  
  ///
  /// Get data from mesh network
  ///
  
  unsigned long currentTime = millis ();
  
  // Update gesture/mesh communication timers
  // TODO: Add this to Looper's timers!
  if (lastSwingAddress != -1) {
    if (currentTime - lastReceivedSwingTime > lastReceivedSwingTimeout) {
      lastSwingAddress = -1;
    }
  }
  
  // Update gesture/mesh communication timers
  // TODO: Add this to Looper's timer!
  if (hasSwung == true) {
    currentTime = millis ();
    if (currentTime - lastSwingTime > lastSwingTimeout) {
      
      // Cancel swing gesture
      Stop_Blink_Light ();
      hasSwung = false;
      
      // Reset the timer
      lastSwingTime = 0L;
    }
  }
  
  // Broadcast message notifying other modules that this module is still active (i.e., broadcast this module's "heartbeat")
  currentTime = millis ();
  if (currentTime - lastSentActive >= lastSentActiveTimeout) {
    //Queue_Broadcast (ANNOUNCE_ACTIVE);
    Queue_Message (platformUuid, BROADCAST_ADDRESS, "notice presence");
    lastSentActive = millis ();
  }
  
  // Remove_Neighbor
  // Removes any neighbors that have left the network.
  // TODO: Update this to use a linked list?
  currentTime = millis ();
  for (int i = 0; i < neighborCount; i++) {

    if (currentTime - neighborAge[i] > 5000) {
      
      Serial.println ("Removing neighbor.");
      
      // Remove the module from the set
      for (int j = i; j < neighborCount - 1; j++) {
        neighbors[j] = neighbors[j + 1];
        neighborAge[j] = neighborAge[j + 1];
      }
      neighborCount--;
        
    }
  }
  
  
  // Check for mesh data and receive it if present
  boolean hasReceivedMeshData = false;
  hasReceivedMeshData = Receive_Mesh_Messages ();
  
  //
  // Sense gesture (and phsyical orientation, generally)
  //
  
  boolean hasGestureChanged = false;
  if (senseOrientation ()) {
    storeData ();
    
    // Classify live gesture sample
    unsigned long currentTime = millis();
    if (currentTime - lastGestureClassificationTime >= gestureSustainDuration[classifiedGestureIndex]) { // Check if gesture duration has expired
      classifiedGestureIndex = classifyGestureFromTransitions(); // (gestureCandidate);
      
      if (classifyMostFrequentGesture) {
        // Add the most recent gesture classification to the end of the classification history list
        if (previousClassifiedGestureCount < PREVIOUS_CLASSIFIED_GESTURE_COUNT) {
          previousClassifiedGestures[previousClassifiedGestureCount] = classifiedGestureIndex;
          previousClassifiedGestureCount++;
        } else {
          for (int i = 0; i < (PREVIOUS_CLASSIFIED_GESTURE_COUNT - 1); i++) {
            previousClassifiedGestures[i] = previousClassifiedGestures[i + 1];
          }
          previousClassifiedGestures[PREVIOUS_CLASSIFIED_GESTURE_COUNT - 1] = classifiedGestureIndex;
        }
        
        // Get the most frequently classified gesture in the history
        for (int i = 0; i < GESTURE_COUNT; i++) { // Reset the previous classified gesture frequency (the number of times each occured in the short history)
          previousClassifiedGestureFrequency[i] = 0;
        }
        
        // Count the number of times each previous gesture occurred i.e., compute frequency of gesture classifications in short history)
        for (int i = 0; i < previousClassifiedGestureCount; i++) {
          int previouslyClassifiedGestureIndex = previousClassifiedGestures[i]; // Get a previous gesture's index
          previousClassifiedGestureFrequency[previouslyClassifiedGestureIndex]++; // Increment the gesture's frequency by one
        }
        
        // Determine the gesture most frequently classified
        int mostFrequentGestureIndex = 0;
        for (int i = 0; i < GESTURE_COUNT; i++) {
  //        Serial.print(previousClassifiedGestureFrequency[i]); Serial.print(" ");
          if (previousClassifiedGestureFrequency[i] > previousClassifiedGestureFrequency[mostFrequentGestureIndex]) {
            mostFrequentGestureIndex = i;
          }
        }
  //      Serial.println();
        
        // Update the classified gesture to the most frequent one
        classifiedGestureIndex = mostFrequentGestureIndex;
      }
      
      // HACK: Doesn't allow tilt left and tilt right (reclassifies them as "at rest, in hand"
//      if (classifiedGestureIndex == 4 || classifiedGestureIndex == 5) {
//        classifiedGestureIndex = 1;
//      }

      if (classifiedGestureIndex == 4) { // Ignore "shake" gesture if not yet swung
        // TODO: Check if has a "remote output", and if so, then DO NOT ignore the swing gesture.
        if (hasSwung == false) {
          if (outputPinRemote == true) {
//            classifiedGestureIndex = 4; // Set "shake" to "at rest"
          } else {
            classifiedGestureIndex = 1; // Change "shake" to "swing" (since they're similar enough for this to make sense)
          }
        }
      } else if (classifiedGestureIndex == 2 || classifiedGestureIndex == 3) { // If hasn't yet swung, then ignore tap gestures
        if (lastSwingAddress == -1) {
          classifiedGestureIndex = 0; // Set to "at rest"
        }
      } else if (classifiedGestureIndex == 1) { // If has swung, don't allow another swing if already swung (no effect)
        if (hasSwung) {
//          classifiedGestureIndex = previousClassifiedGestureIndex;
        }
      }
      
      lastGestureClassificationTime = millis (); // Update time of most recent gesture classification
    }
    
    // Update current gesture (if it has changed)
    if (classifiedGestureIndex != previousClassifiedGestureIndex) {
      Serial.print ("Detected gesture: ");
      Serial.print (gestureName[classifiedGestureIndex]);
      Serial.println ();
      
      
      
  
      // Send module to remote module to set up its "messageSourceModule"
      Memory* memory = Update_Memory (String ("latest-gesture"), String (gestureName[classifiedGestureIndex])); // Memory* memory = Create_Memory (key, value);
      
      Memory* existingMemory = Get_Memory ("latest-gesture");
      if (existingMemory == NULL) {
        Append_Memory (memory);
      }
      
      
      
      
      // Update the previous gesture to the current gesture
      previousClassifiedGestureIndex = classifiedGestureIndex;
      
      // Indicate that the gesture has changed
      hasGestureChanged = true;
      hasGestureProcessed = false;
      
      // TODO: Process newly classified gesture
      // TODO: Make sure the transition can happen (with respect to timing, "transition cooldown")
    }
    
  }
  
  //
  // Gesture Interpreter:
  // Process gestures. Perform the action associated with the gesture.
  //
  
  // TODO: Enable repeat gestures!
  // TODO: Then try... interpret "shake" as "swing" and just use swing with 4 modes (dot, line, loop, none)
  
  // Process current gesture (if it hasn't been processed yet)
  if (hasGestureChanged) { // Only executed when the gesture has changed
    if (!hasGestureProcessed) { // Only executed when the gesture hasn't yet been processed
      
      // Handle gesture
      if (classifiedGestureIndex == 0) { // Check if gesture is "at rest"
        Handle_Gesture_At_Rest ();
      } else if (classifiedGestureIndex == 1) { // Check if gesture is "swing"
        Handle_Gesture_Swing ();
      } else if (classifiedGestureIndex == 2) { // Check if gesture is "tap to another, as left"
        Handle_Gesture_Tap (); // Handle_Gesture_Tap_As_Left ();
      } else if (classifiedGestureIndex == 3) { // Check if gesture is "tap to another, as right"
        Handle_Gesture_Tap (); // Handle_Gesture_Tap_As_Right ();
      } else if (classifiedGestureIndex == 4) { // Check if gesture is "shake"
        Handle_Gesture_Shake ();
      } else if (classifiedGestureIndex == 5) { // Check if gesture is "tilt left"
//        Handle_Gesture_Tilt_Left ();
      } else if (classifiedGestureIndex == 6) { // Check if gesture is "tilt right"
//        Handle_Gesture_Tilt_Right ();
      } else if (classifiedGestureIndex == 7) { // Check if gesture is "tilt forward"
//        Handle_Gesture_Tilt_Forward ();
      } else if (classifiedGestureIndex == 8) { // Check if gesture is "tilt backward"
//        Handle_Gesture_Tilt_Backward ();
      }
      
      hasGestureProcessed = true; // Set flag indicating gesture has been processed
    }
  }
  
  //
  // Process incoming messages in queue (if any)
  //
  
  if (incomingMessageQueue != NULL) { /// Check if there are any received messages to be processed
    
    // Get the next message on the queue of incoming messages
    Message* message = Dequeue_Incoming_Message ();
    
    Process_Message (message);
    
  }
  
  //
  // Update state of current module
  //
  
  //if (awaitingNextModuleConfirm) {
  if (awaitingNextModule) {
    unsigned long currentTime = millis ();
    if (currentTime - awaitingNextModuleStartTime > SEQUENCE_REQUEST_TIMEOUT) {
      Serial.println ("awaitingNextModule reset");
      awaitingNextModule = false;
      awaitingNextModuleConfirm = false;
    }
  }
  
  //if (awaitingPreviousModuleConfirm) {
  if (awaitingPreviousModule) {
    unsigned long currentTime = millis ();
    if (currentTime - awaitingPreviousModuleStartTime > SEQUENCE_REQUEST_TIMEOUT) {
      Serial.println ("awaitingPreviousModule reset");
      awaitingPreviousModule = false;
      awaitingPreviousModuleConfirm = false;
    }
  }
  
  //
  // Send outgoing messages (e.g., this module's updated gesture)
  //
  
  currentTime = millis ();
  if (currentTime - lastMessageSendTime > PACKET_WRITE_TIMEOUT) {
  
    // Process mesh message queue  
    if (outgoingMessageQueue != NULL) { // if (messageQueueSize > 0) {
      Send_Mesh_Message ();
    }
    
    // Update the time that a message was most-recently dispatched
    lastMessageSendTime = millis();
  }
}
