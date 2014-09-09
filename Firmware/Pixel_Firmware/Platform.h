//
// i.e., Device "virtual machine"
//
// TODO: Define "void setPinMode(Pixel pixel, int mode)" to update the state of another Pixel in the network (from any available Pixel)

#ifndef PLATFORM_H
#define PLATFORM_H

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
//Platform platform;

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
  
//  int location; // Is it local or remote
  
  boolean isUpdated; // Denotes that the physical pin has been updated and it should be synced up with its virtual pins (and associated hardware).
  
  Channel* previous;
  Channel* next;
};

//Channel channels[PLATFORM_CHANNEL_COUNT];

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
#define PIN_TYPE_TOUCH 3

Platform* Create_Platform ();
Channel* Create_Channel (Platform* platform, int address);

Platform* platform = NULL;
void setupPlatform () {
  
  // Create the platform
  platform = Create_Platform ();
  
  // Create the channels for the platform (i.e., set up access to the platform's I/O pins)
  for (int i = 0; i < PLATFORM_CHANNEL_COUNT; i++) {
    
    // Create a channel for the specified platform with the specified address
    Channel* channel = Create_Channel (platform, i);
  }
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
//  (*platform).uid  = generateUuid ();
  (*platform).uid  = 0;
  
  // Return platform
  return platform;
  
}

//! Returns the channel on the specified platform with the specified address.
//!
Channel* Get_Channel (Platform* platform, int address) {
  Serial.println ("Get_Channel");
  
 // Get pointer to behavior node at specified index
  Channel* channel = NULL;
  
  // TODO: Search the sequences for the specified behavior
  
  // Search the loop for the behavior with the specified UID.
  if (platform != NULL) {
    
    // Get the last behavior in the loop
    Channel* lastChannel = (*platform).channels;
    while (lastChannel != NULL) {
      Serial.print ("Searching for channel "); Serial.print (address); Serial.print (" on "); Serial.print ((*lastChannel).address); Serial.print ("\n");
      
      // Return the behavior if it has been found
      if ((*lastChannel).address == address) {
        Serial.print ("Found channel "); Serial.print ((*lastChannel).address); Serial.print ("\n");
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
  (*channel).isUpdated = false;
  (*channel).previous  = NULL;
  (*channel).next  = NULL;
  
  // Generate UUID for the channel
  (*channel).uid  = 0; // (*channel).uid  = generateUuid ();
  (*channel).address = address;
  
  // Add the channel to the platform's list of channels
  if ((*platform).channels == NULL) {
    
    Serial.println("First channel");
    (*platform).channels = channel;
    
  } else {
    
    // Get the last channel on the platform
    Channel* lastChannel = (*platform).channels;
    while ((*lastChannel).next != NULL) {
      Serial.println("Next channel");
      lastChannel = (*lastChannel).next;
    }
    
    // Insert at end of the list (iterate to find the last channel)
    (*channel).previous = lastChannel; // Set up the pointer from the new channel to the previous channel.
    (*lastChannel).next = channel; // Finally, set up the link to the new channel.
  }
  
  // Return channel
  return channel;
  
};
  
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
  Serial.println ("Get_Channel_Type");
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
  Serial.println ("Get_Channel_mode");
  
  if (channel != NULL) {
    (*channel).isUpdated = true; // TODO: Propagate this change to the platform associated with the channel.
    return (*channel).mode;
  }
  // TODO: Return "none" if channel not found (if specified channel is NULL)
}

void Update_Channel_Value (Channel* channel, int value) {
  Serial.println ("Update_Channel_Value");
  
  if (channel != NULL) {
    
    // Set the channel mode corresponding to the specified value
    Update_Channel_Mode (channel, PIN_MODE_OUTPUT);
    
    // Set the channel value
    (*channel).value = value;
    
    // Set the actual hardware's pin's value
    // TODO: Move this to the platform's schema!
    digitalWrite ((*channel).address, ((*channel).value == PIN_VALUE_HIGH ? HIGH : LOW));
    
    (*channel).isUpdated = true; // TODO: Propagate this change to the platform associated with the channel.
  }
  // TODO: Return "none" if channel not found (if specified channel is NULL)
}

int Get_Current_Channel_Value (Channel* channel) {
  Serial.println ("Get_Current_Pin_Value");
  
  if (channel != NULL) {
    
    // Get the actual value from the channel's source
//    (*channel).value = digitalRead ((*channel).address);
    
    return (*channel).value;
  }
  // TODO: Return "none" if channel not found (if specified channel is NULL)
}

//! Gets the value on the specified channel.
//!
int Get_Channel_Value (Channel* channel) {
//int Get_Pin_Value (Channel* channel) {
  Serial.println ("Get_Pin_Value");
  
  if (channel != NULL) {
    
    Update_Channel_Mode (channel, PIN_MODE_INPUT);
    
    // Get the actual value from the channel's source
    (*channel).value = digitalRead ((*channel).address);

    (*channel).isUpdated = true; // TODO: Propagate this change to the platform associated with the channel.
    
    return (*channel).value;
  }
  // TODO: Return "none" if channel not found (if specified channel is NULL)
}



#endif
