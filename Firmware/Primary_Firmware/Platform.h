//
// i.e., Device "virtual machine"
//
// TODO: Define "void setPinMode(Pixel pixel, int mode)" to update the state of another Pixel in the network (from any available Pixel)

#ifndef PLATFORM_H
#define PLATFORM_H

#include "Ports.h"

//#define PLATFORM_PORT_COUNT 24
//
//struct Platform;
//struct Platform_Schema;
//struct Transducer;
//struct Channel;
//
//struct Teensy_Schema {
//  void* Update_Pin;
//  void* Get_Pin;
//};

struct Platform;
struct Channel;

#define PLATFORM_CHANNEL_COUNT 24

Channel* Get_Channel (Platform* platform, int address);
int Get_Channel_Value (Channel* channel);
int Get_Channel_Previous_Value (Channel* channel);
void Update_Channel_Type (Channel* channel, int type);

// i.e., the "transcluded" device. Changes to this device are reflected in the associated physical device.
struct Platform {
  int uid; // Universally unique ID
  
  int ipAddress;
  int meshAddress;
  // TODO: Include "physicalDevice" reference.
  
  Channel* channels;
  
  boolean isUpdated;
  
//  void* schema; // TODO: Consider renaming this to "profile" or "description".
};

//! Channels are paths along which messages or data can be propagated, 
//! captured, or exchanged. 
//! For example, pins on a hardware platform (e.g., Arduino) are channels.
//! Channels can also be a web socket, HTTP interface, memory, cloud 
//! database, etc.
//!
struct Channel {
  int uid; // Universally unique ID
  
  Platform* platform;
  
  int address; // int pin; // The pin number
  int type; // i.e., digital, analog, pwm, touch
  int mode; // i.e., input or output
  int value; // i.e., high or low
  
  // TODO: Remove previousValue, replacing it by calls to the previous value in the timeline
  int previousValue; // i.e., the pervious value
  
  // Channel Filter (e.g., capacitative):
  int threshold;
  
//  int location; // Is it local or remote
  
  boolean isUpdated; // Denotes that the physical pin has been updated and it should be synced up with its virtual pins (and associated hardware).
  
  Channel* previous;
  Channel* next;
};

// Pixel's pin states (i.e., high or low)
#define PIN_VALUE_LOW 0
#define PIN_VALUE_HIGH 1
#define PIN_VALUE_NONE 2

// Pixel's pin modes (i.e., input or output)
#define PIN_MODE_INPUT 0
#define PIN_MODE_OUTPUT 1

// Pixel's pin type
#define PIN_TYPE_DIGITAL 0
#define PIN_TYPE_ANALOG 1
#define PIN_TYPE_PWM 2
#define PIN_TYPE_CAPCITIVE 3

Platform* Create_Platform ();
Channel* Create_Channel (Platform* platform, int address);

long platformUuid = 0L;
boolean hasPlatformUuid = false;

Platform* platform = NULL;

long Generate_Platform_Uuid () {
  const int maximumNumber = 1000L;
  long randomNumber = random (maximumNumber);
  return randomNumber;
}

boolean Setup_Platform_Uuid () {
  
  // Get the device UUID used for communication over the local mesh network.
  while (hasPlatformUuid == false) {
    
    platformUuid = Generate_Platform_Uuid (); // Assing the device UUID
    
    // Check if a valid UUID has been assigned to the platform
    if (platformUuid != 0L) {
      hasPlatformUuid = true;
      
      Serial.println (platformUuid);
      
      return false; // Setup failed
    }
  }
  
  return false; // Setup was successful
}

// NOTE: This is specific to Teensy, but it should be variable, so multiple platforms are supported (e.g., OS X, iOS, Android, Node.js, JavaScript in Web Browser)
boolean Setup_Platform () {
  
  // Create the platform
  platform = Create_Platform ();
  
  // Create the channels for the platform (i.e., set up access to the platform's I/O pins)
  for (int i = 0; i < PLATFORM_CHANNEL_COUNT; i++) {
    
    // Create a channel for the specified platform with the specified address
    Channel* channel = Create_Channel (platform, i);
  }
  
  // Set default types for each created channel
  Channel* inputChannel = Get_Channel (platform, MODULE_INPUT_PIN);
  Update_Channel_Type (inputChannel, PIN_TYPE_CAPCITIVE);
  
  Serial.print ("There are ");
  Serial.print (PLATFORM_CHANNEL_COUNT);
  Serial.print (" channels\n");
  
  return true; // Setup was successful
}

//! Create platform structures.
//!
Platform* Create_Platform () {
//  Serial.println ("Create_Platform");
  
  // Create platform
  Platform* platform = (Platform*) malloc (sizeof (Platform));
  
  // Initialize platform
  (*platform).uid         = 0;
  (*platform).ipAddress   = 0;
  (*platform).meshAddress = 0;
  (*platform).channels    = NULL;
  (*platform).isUpdated   = false;
  
  // Generate UUID for the behavior
  (*platform).uid  = 0; // (*platform).uid  = generateUuid ();
  
  // Return platform
  return platform;
  
}

//! Returns the channel on the specified platform with the specified address.
//!
Channel* Get_Channel (Platform* platform, int address) {
  // DEBUG: Serial.println ("Get_Channel");
  
 // Get pointer to behavior node at specified index
  Channel* channel = NULL;
  
  // TODO: Search the sequences for the specified behavior
  
  // Search the loop for the behavior with the specified UID.
  if (platform != NULL) {
    
    // Get the last behavior in the loop
    Channel* lastChannel = (*platform).channels;
    while (lastChannel != NULL) {
      // DEBUG: Serial.print ("Searching for channel "); Serial.print (address); Serial.print (" on "); Serial.print ((*lastChannel).address); Serial.print ("\n");
      
      // Return the behavior if it has been found
      if ((*lastChannel).address == address) {
        // DEBUG: Serial.print ("Found channel "); Serial.print ((*lastChannel).address); Serial.print ("\n");
        return lastChannel;
      }
      
      lastChannel = (*lastChannel).next;
    }
    
  }
  
  return channel;
  
}

//! Creates a channel associated with the specified platform, available at the specified address.
//!
//! TODO: Change default "address" from 0 to something else, such as -1, which can mean "none".
//!
Channel* Create_Channel (Platform* platform, int address) {
//  Serial.println ("Create_Channel");
  // i.e., "address" is a "pin"

  // Create channel
  Channel* channel = (Channel*) malloc (sizeof (Channel));
  
  // Initialize channel
  (*channel).platform  = platform;
  (*channel).address   = 0; // e.g., pin
  (*channel).type      = PIN_TYPE_DIGITAL;
  (*channel).mode      = PIN_MODE_INPUT;
  (*channel).value     = 0; // Initialize value to 0
  (*channel).previousValue  = 0; // Initialize value to 0
  (*channel).isUpdated = false;
  (*channel).previous  = NULL;
  (*channel).next  = NULL;
  
  (*channel).threshold = 2500; // for capacitative
  
  // Generate UUID for the channel
  (*channel).uid  = 0; // (*channel).uid  = generateUuid ();
  (*channel).address = address;
  
  // Add the channel to the platform's list of channels
  if ((*platform).channels == NULL) {
    
//    Serial.println("First channel");
    (*platform).channels = channel;
    
  } else {
    
    // Get the last channel on the platform
    Channel* lastChannel = (*platform).channels;
    while ((*lastChannel).next != NULL) {
//      Serial.println("The next channel.");
      lastChannel = (*lastChannel).next;
    }
    
    // Insert at end of the list (iterate to find the last channel)
    (*channel).previous = lastChannel; // Set up the pointer from the new channel to the previous channel.
    (*lastChannel).next = channel; // Finally, set up the link to the new channel.
  }
  
  // Return channel
  return channel;
  
};

Channel* Get_Channel (int address) {
  // Get the last channel on the platform
  Channel* channel = (*platform).channels;
  while (channel != NULL) {
    if ((*channel).address == address) {
      return channel;
    }
    channel = (*channel).next;
  }
  return NULL;
}
  
void Update_Channel_Type (Channel* channel, int type) {
  Serial.println ("Update_Channel_Type");
//void Update_Channel_Type (int pin, int type) {
//void setPinType (int pin, int type) {
  if (channel != NULL) {
    (*channel).type = type;
    
    (*channel).isUpdated = true;
  }
  
  // NOTE: There's no "pin type" for Arduino. It's just a virtual construct for Pixel.
}

int Get_Channel_Type (Channel* channel) {
  // DEBUG: Serial.println ("Get_Channel_Type");
  if (channel != NULL) {
    (*channel).isUpdated = true; // TODO: Propagate this change to the platform associated with the channel.
    
    return (*channel).type;
  }
  // TODO: Return "none" if channel not found (if specified channel is NULL)
}

void Update_Channel_Mode (Channel* channel, int mode) {
  Serial.println ("Update_Channel_Mode");
  
  if (channel != NULL) {
    (*channel).mode = mode;
    
    // Set the actual hardware's pin's mode
    // TODO: Move this to the platform's schema!
    pinMode ((*channel).address, ((*channel).mode == PIN_MODE_INPUT ? INPUT : OUTPUT));
    
    (*channel).isUpdated = true; // TODO: Propagate this change to the platform associated with the channel.
  }
  // TODO: Return "none" if channel not found (if specified channel is NULL)
}

int Get_Channel_Mode (Channel* channel) {
  // DEBUG: Serial.println ("Get_Channel_Mode");
  
  if (channel != NULL) {
    (*channel).isUpdated = true; // TODO: Propagate this change to the platform associated with the channel.
    return (*channel).mode;
  }
  // TODO: Return "none" if channel not found (if specified channel is NULL)
}

//int (*HwDigitalWrite)(int,int);
void (*HwDigitalWrite)(unsigned char,unsigned char);

// if input, update and buffer the value on the source of the input (e.g., pin)
// if output, update the output source with the buffered value (after updating from the source of output?)
void Update_Channel_Value (Channel* channel) {
  // DEBUG: Serial.println ("Update_Channel_Value");
  
  if (channel != NULL) {
    
    int channelMode = Get_Channel_Mode (channel);
    
    if (channelMode == PIN_MODE_INPUT) {
      
      // Get channel mode (e.g., input or output (pin); GET, POST, etc. (HTTP); (voice); (video))
      int channelType = Get_Channel_Type (channel); // Get channel type
      
      // Update the channel's value (update raw input or set raw output)
//      Update_Channel_Value (currentChannel);
      
      if (channelType == PIN_TYPE_CAPCITIVE) {
        
        // Store current value in the history
        (*channel).previousValue = (*channel).value;
        
        // Get raw input value on the specified "channel" for the specified "mode" (e.g., capacitive, digital, analog)
        (*channel).value = Get_Capacitive_Input_Continuous ((*channel).address); // TODO: Change MODULE_INPUT_PIN to Channel* (the other functions are lower level?)
        // DEBUG: Serial.println (touchInputMean); // Output value for debugging (or manual calibration)
        
      } else if (channelType == PIN_TYPE_DIGITAL) {
        
        // Store current value in the history
        (*channel).previousValue = (*channel).value;
        
        // TODO: Update this so it's stored in the platform model
        (*channel).value = digitalRead ((*channel).address); // Raw value on channel
        
      }
      
    } else if (channelMode == PIN_MODE_OUTPUT) {
      
      // TODO: Update the previous value only if the present value is different (or should it always be updated and put into the history?)
      
      // Set the channel mode corresponding to the specified value
//      Update_Channel_Mode (channel, PIN_MODE_OUTPUT);
      
      // Set the channel value
//      (*channel).value = value;

      // Get type (capacitive, digital, pwm, etc.) and configure and update output accordingly
      
      // Set the actual hardware's pin's value
      // TODO: Move this to the platform's schema!
      digitalWrite ((*channel).address, ((*channel).value == PIN_VALUE_HIGH ? HIGH : LOW)); // Call the function
      
      (*channel).isUpdated = true; // TODO: Propagate this change to the platform associated with the channel.
      
    }
//    else if (channelMode == CHANNEL_MODE_HTTP_REQUEST) {
//    }
  }
  // TODO: Return "none" if channel not found (if specified channel is NULL)
}

int Get_Channel_Switch_Threshold (Channel* channel) {
  
  if (channel != NULL) {
    
    return (*channel).threshold;
    
  }
  
  return NULL;
  
}

int Get_Channel_Digital_Value (Channel* channel) {
  
  if (channel != NULL) {
    
    int channelType = Get_Channel_Type (channel);
  
    if (channelType == PIN_TYPE_CAPCITIVE) {
      
      // Get raw input value on the specified "channel" for the specified "mode" (e.g., capacitive, digital, analog)
  //        Get_Capacitive_Input_Continuous (MODULE_INPUT_PIN); // TODO: Change MODULE_INPUT_PIN to Channel* (the other functions are lower level?)
      // DEBUG: Serial.println (touchInputMean); // Output value for debugging (or manual calibration)
      
      // Get the capacitative threshold for the specified channel
//      int capacitiveThresholdValue = 2500;
      
      int currentValue = Get_Channel_Value (channel);
      int previousValue = Get_Channel_Previous_Value (channel);
      int thresholdValue = Get_Channel_Switch_Threshold (channel);
      
      if (currentValue > thresholdValue && previousValue <= thresholdValue) { // Check if state changed to "pressed" from "not pressed"
      
        // Update input pin value to low (off)
  //        Channel* moduleInputChannel = Get_Channel (platform, MODULE_INPUT_PIN);
  //          Set_Channel_Value (currentChannel, PIN_VALUE_HIGH);
  //          Propagate_Channel_Value (currentChannel);
      
//        // Update output pin value to low (off)
//        if (outputPinRemote == false) {
//          Serial.println ("OUT");
//          // Output port is on this module!
//          //Update_Channel_Value (MODULE_OUTPUT_PIN, PIN_VALUE_HIGH);
//          Channel* moduleOutputChannel = Get_Channel (platform, MODULE_OUTPUT_PIN);
//          Set_Channel_Value (moduleOutputChannel, PIN_VALUE_HIGH);
//          Propagate_Channel_Value (moduleOutputChannel);
//          Blink_Light (2); // BUG! wrong number
//        } else {
//          // Output port is on a different module than this one!
//          Blink_Light (3); // BUG! wrong number
//  //        Queue_Message (BROADCAST_ADDRESS, ACTIVATE_MODULE_OUTPUT);
//          Queue_Message (platformUuid, BROADCAST_ADDRESS, String ("turn output on"));
//        }

        return 1;
    
      } else if (currentValue <= thresholdValue && previousValue > thresholdValue) { // Check if state changed to "not pressed" from "pressed"
      
        // Update input pin value to high (on)
  //        Channel* moduleInputChannel = Get_Channel (platform, MODULE_INPUT_PIN);
  //          Set_Channel_Value (currentChannel, PIN_VALUE_LOW);
  //          Propagate_Channel_Value (currentChannel);
      
        // Update output pin value to high (on)
//        if (outputPinRemote == false) {
//          Serial.println ("out");
//  //        Update_Channel_Value (MODULE_OUTPUT_PIN, PIN_VALUE_LOW);
//  //        syncPinValue(MODULE_OUTPUT_PIN);
//          Channel* moduleOutputChannel = Get_Channel (platform, MODULE_OUTPUT_PIN);
//          Set_Channel_Value (moduleOutputChannel, PIN_VALUE_LOW);
//          Propagate_Channel_Value (moduleOutputChannel);
//        } else {
//  //        Queue_Message (BROADCAST_ADDRESS, DEACTIVATE_MODULE_OUTPUT);
//          Queue_Message (platformUuid, BROADCAST_ADDRESS, String ("turn output off"));
//        }
        
        return 0;
    
      }
    }
    
  }
  
  return -1; // Return "no op" by default
    
}

void Set_Channel_Value (Channel* channel, int value) {
  Serial.println ("Set_Channel_Value");
  
  if (channel != NULL) {
    
    // Set the channel mode corresponding to the specified value
    Update_Channel_Mode (channel, PIN_MODE_OUTPUT);
    
    // Store current value in the history
    (*channel).previousValue = (*channel).value;
    
    // Set the channel value
    (*channel).value = value;
    
    // Set the actual hardware's pin's value
    // TODO: Move this to the platform's schema!
    HwDigitalWrite = &digitalWrite;
    (*HwDigitalWrite) ((*channel).address, ((*channel).value == PIN_VALUE_HIGH ? HIGH : LOW)); // Call the function
    
    (*channel).isUpdated = true; // TODO: Propagate this change to the platform associated with the channel.
  }
  // TODO: Return "none" if channel not found (if specified channel is NULL)
}

//! Gets the "current" value on the specified channel.
//!
int Get_Channel_Value (Channel* channel) {
  // DEBUG: Serial.println ("Get_Channel_Value");
  
  if (channel != NULL) {
    
    // Get the actual value from the channel's source
//    (*channel).value = digitalRead ((*channel).address);
    
    return (*channel).value;
  }
  // TODO: Return "none" if channel not found (if specified channel is NULL)
}

//! Gets the "current" value on the specified channel.
//!
int Get_Channel_Previous_Value (Channel* channel) {
  // DEBUG: Serial.println ("Get_Channel_Previous_Value");
  
  if (channel != NULL) {
    
    // Get the actual value from the channel's source
//    (*channel).value = digitalRead ((*channel).address);
    
    return (*channel).previousValue;
  }
  // TODO: Return "none" if channel not found (if specified channel is NULL)
}

////! Gets the value on the specified channel.
////!
//int Get_Current_Channel_Value (Channel* channel) {
//  Serial.println ("Get_Channel_Value");
//  
//  if (channel != NULL) {
//    
//    Update_Channel_Mode (channel, PIN_MODE_INPUT);
//    
//    // Get the actual value from the channel's source
//    (*channel).value = digitalRead ((*channel).address);
//
//    (*channel).isUpdated = true; // TODO: Propagate this change to the platform associated with the channel.
//    
//    return (*channel).value;
//  }
//  // TODO: Return "none" if channel not found (if specified channel is NULL)
//}

#endif
