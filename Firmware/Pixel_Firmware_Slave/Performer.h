#ifndef PERFORMER_H
#define PERFORMER_H

#include "Hardware.h"

// Create_Performer
// Get_Performer_Substrate
// Update_Performer_Substrate
// Delete_Performer
// Continue_Performance
// Perform_Behavior

//! Behavior Performer
//!
struct Performer {
  int uid;
  Substrate* substrate;
  Behavior* behavior; // i.e., The current behavior. This is akin to a "program counter".
  
  Behavior* origin; // i.e., The performer's first behavior.
  
//  Device* device; // i.e., The device upon which the performance will take place.
};

//! Creates a behavior performer in the specified substrate.
//!
Performer* Create_Performer (Substrate* substrate) {
  
  Performer* performer = NULL;
  
  if (substrate != NULL) {
    
    // Create a performer
    performer              = (Performer*) malloc (sizeof (Performer));
    (*performer).uid       = 0;
    (*performer).substrate = substrate;
    (*performer).behavior  = NULL;
    (*performer).origin    = NULL;
    
    // Generate UUID for the processor
    (*behavior).uid  = generateUuid();
    
    // Initialize the processor's current behavior
    if ((*substrate).origin != NULL) {
      (*performer).behavior = (*substrate).origin;
    }
  
  }
  
  return performer;
}

boolean Perform_Behavior (Performer* performer) {
  if (performer != NULL) {
    Behavior* behavior = (*performer).behavior;
    
    if (behavior != NULL) {
    
      if ((*behavior).type == BEHAVIOR_TYPE_OUTPUT) {
        Output* output = (Output*) (*behavior).schema;
        
        // Update the pin's state
        Update_Virtual_Pin ((*output).pin, (*output).signal, (*output).data);
      }
      
      // Continue behavior performance
      (*performer).behavior = (*((*performer).behavior)).next;
      
      return true;
    }
  }
  
  return false;
}

#endif
