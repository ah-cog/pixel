#ifndef LOOP_H
#define LOOP_H

#include "Behavior.h"

// Behavior nodes that define the module's beahvior
#define DEFAULT_LOOP_CAPACITY 20
//Loop behaviorLoop[DEFAULT_LOOP_CAPACITY]; // TODO: FIx this!
//int loopSize = 0;
Behavior behaviorLoop[DEFAULT_LOOP_CAPACITY]; // Behavior nodes
int loopSize = 0;
int loopCounter = -1; // i.e., the "program counter"


#define PROPAGATION_SIZE 32

//! Propagation (i.e., Transformation) to propagate.
//!
//! TODO: Rename to Transformation
struct Propagation {
  // String data;
  char* data;
  int size;
  
  Propagation* previous;
  Propagation* next;
};

//! Propagator of behavior transformations.
//!
struct Propagator {
  Propagation* propagation;
};

//! Create behavior transformation propagator.
//!
Propagator* Create_Propagator () {
  
  Serial.println ("Create_Propagator");
  
  // Create substrate
  Propagator* propagator = (Propagator*) malloc (sizeof (Propagator));
  
  // Initialize sequence
  (*propagator).propagation = NULL;
  
  // Return sequence
  return propagator;
  
}

//! Deletes the propagator from dynamic memory.
//!
boolean Delete_Propagator (Propagator* propagator) {
  
  Serial.println ("Delete_Propagator");
  
  if (propagator != NULL) {
  
    // Delete propagator
    free (propagator);
    
    // Return success
    return true;
  
  }
  
  // Return failure
  return false;
  
}

//! Create propagation (i.e., a behavior transformation).
//!
Propagation* Create_Propagation (String data) {
  
  Serial.println ("Create_Propagation");
  Serial.print ("data = "); Serial.print (data); Serial.print ("\n");
  
  // Create substrate
  Propagation* propagation = (Propagation*) malloc (sizeof (Propagation));
  
  Serial.println ("Allocated Propagation.");
  
  // Initialize propagation
  (*propagation).size = PROPAGATION_SIZE;
  (*propagation).data = (char*) malloc (PROPAGATION_SIZE * sizeof (char));
  data.toCharArray ((*propagation).data, PROPAGATION_SIZE);
  (*propagation).previous = NULL;
  (*propagation).next = NULL;
  
  Serial.print ("Initialized Propagation "); Serial.print ((int) propagation); Serial.print ("\n");
  
  // Return sequence
  return propagation;
  
}

String Get_Propagation_Data (Propagation* propagation) {
  
  if (propagation != NULL) {
    
    return (*propagation).data;
    
  }
  
  return NULL;
  
}

//! Frees the Propagation from dynamic memory.
//!
boolean Delete_Propagation (Propagation* propagation) {
  
  Serial.println ("Delete_Propagation");
  
  if (propagation != NULL) {
    
    // Free the data payload
    if ((*propagation).data != NULL) {
      free ((*propagation).data);
    }
    
    // Free the propagation object
    free (propagation);
    
  }
  
}

//! Queue the transformation for propagation.
//!
Propagation* Queue_Propagation (Propagator* propagator, Propagation* propagation) {
  
  Serial.println ("Queue_Propagation");
  
  if ((*propagator).propagation == NULL) {
    
    // Push to the top of the stack (as the first element)
    (*propagator).propagation = propagation;
    
    // Set up the forward and back links
    (*propagation).previous = NULL;
    (*propagation).next     = NULL;
    
  } else {
    
    // Get the propagation at the end of the queue
    Propagation* currentPropagation = (*propagator).propagation;
    while (currentPropagation != NULL) {
      currentPropagation = (*currentPropagation).previous;
    }
    
    // Push to the top of the stack
    (*currentPropagation).previous = propagation;
    
    // Set up the backward and forward links
    (*propagation).next = currentPropagation;
    (*propagation).previous = NULL;
    
  }
  
  // Return sequence
  return propagation;
  
}

//! Pop propagation off the propagator's stack
//!
Propagation* Dequeue_Propagation (Propagator* propagator) {
  
  Serial.println ("Dequeue_Propagation");
  
  Propagation* propagation = NULL;
  
  if (propagator != NULL) {
    
    // Get the propagation at the front of the propagator's queue
    propagation = (*propagator).propagation;
    
    // Update the Propagator. Set the next of the propagator's propagation queue.
    (*propagator).propagation = (*propagation).previous;
    
    // Dissociate the dequeued propagation. Update the backward and forward links of the dequeued propagation.
    (*propagation).next = NULL;
    (*propagation).previous = NULL;
    
  }
  
  return propagation;
  
}

#define NO_CHANNEL 0
#define I2C_CHANNEL 1
// TODO: #define VISUAL_CHANNEL 2
// TODO: #define AURAL_CHANNEL 2
// TODO: #define TEXTUAL_CHANNEL 3

boolean Propagate (Propagator* propagator, int channel) {
  
  Serial.println ("Propagate");
  
  if (propagator != NULL) {
    
    if (channel == I2C_CHANNEL) {
      
      // "(create input behavior 38472934)" // Pass on the parentheticized "secret"
      
      // Create buffer for storing the bytes to be sent (over I2C)
      const int AVAILABLE_BUFFER_BYTES = 32;
      char buffer[AVAILABLE_BUFFER_BYTES];
      
      // Dequeue the next description to be sent by the specified propagator
      Propagation* propagation = Dequeue_Propagation (propagator);
      
      // TODO: Break up dequeued string to be sent into 32 byte segments, then queue them in the I2C outgoing data buffer.

      // Transmit data over via the I2C protocol
      Wire.write ("("); // Start transformation description
      Wire.write ((*propagation).data); // Write the serialized data
      Wire.write (")"); // Conclude transformation description
      
      // Free the propagation from memory (once sent via I2C)
      Delete_Propagation (propagation);
      
      return true;
      
    }
    
  }
  
  // Return false because a valid propagator or channel was not specified.
  return false;
  
}



Substrate* Create_Substrate ();
Sequence* Create_Sequence (Substrate* substrate);
boolean Update_Sequence_Substrate (Sequence* sequence, Substrate* substrate);
Behavior* Create_Behavior (Substrate* substrate);
boolean Remove_Sequence_Substrate (Sequence* sequence, Substrate* substrate);

boolean setupLooper () {
  
  // Create behavior substrate
  if (substrate == NULL) {
    substrate = Create_Substrate ();
  }
  
  // Create sequence
  // TODO: Add parameter "Substrate* substrate"
  if (substrate != NULL) {
    
    Sequence* sequence = Create_Sequence (substrate);
    
    boolean isAdded = Update_Sequence_Substrate (sequence, substrate);
    
//    return isAdded;
  }
  
  return true;
}

long generateUuid() {
  long uuid = random(65000L);
  return uuid;
}

// Methods:
//
// Create_Substrate
// Get_Substrate_Origin
// TODO: Get_Substrate_Behaviors
// TODO: Get_Substrate_Sequences
// Update_Substrate_Origin
// Delete_Substrate
// TODO: Deep_Delete_Substrate
//
// Create_Sequence
// Update_Sequence_Substrate
// Remove_Sequence_Substrate
// Delete_Sequence
// TODO: Deep_Delete_Sequence
// Dissociate_Sequence
//
// Create_Behavior
// Get_Behavior
// Update_Behavior
// Update_Behavior_Substrate
// Update_Behavior_Sequence
// Remove_Behavior_Sequence
// Delete_Behavior
// TODO: Deeper_Delete_Behavior/Network_Delete_Behavior (i.e., (1) deep delete behavior and (2) delete everything that was connected to it and everything that was connected to the things connected to it)
// Dissociate_Behavior

// Create behavior substrate
Substrate* Create_Substrate () {
  
  // Create substrate
  Substrate* substrate = (Substrate*) malloc (sizeof (Substrate));
  
  // Initialize sequence
  (*substrate).sequences = NULL;
  (*substrate).origin    = NULL;
  
  // Return sequence
  return substrate;
  
}

Sequence* Get_Substrate_Origin (Substrate* substrate) {
  Sequence* origin = NULL;
  
  if (substrate != NULL) {
    origin = (*substrate).origin;
  }
  
  return origin;
}

boolean Update_Substrate_Origin (Substrate* substrate, Sequence* origin) {
  if (substrate != NULL) {
    (*substrate).origin = origin;
  }
}

//! Dissociates all references to the specified sequence and returns it.
//!
Sequence* Dissociate_Sequence (Sequence* sequence) {
//  Sequence* sequence = NULL;
  if (sequence != NULL) {
    
//    while (sequence != NULL) {
    
    // Step 1. Dissociate from behaviors in the sequence
    Behavior* currentBehavior  = (*sequence).behavior;
    while (currentBehavior != NULL) {
      
//      Dissociate_Behavior (currentBehavior);
      // Dissociate a single behavior
      // TODO: Update to point to "Dissociated Substrate/NULL Substrate" rather than NULL.
      (*currentBehavior).substrate = NULL;
      
      // Proceed to the next behavior
      currentBehavior = (*currentBehavior).next;
    }
    
    // Step 2. Remove sequence from substrate
    boolean hasRemoved = Remove_Sequence_Substrate (sequence, (*sequence).substrate);
    
    // Dissociate from sequence
    // TODO: Update to point to "Dissociated Substrate/NULL Substrate" rather than NULL.
//    (*sequence).substrate = NULL;
    
//    // Proceed to next sequence
//    sequence = (*sequence).next;
//      
//    }
    
  }
  return sequence;
}

////! Dissociates all references to the specified sequence and returns it.
////!
//Behavior* Dissociate_Behavior (Behavior* behavior) {
////  Behavior* behavior = NULL;
//  if (behavior != NULL) {
//      
//    // Dissociate behaviors
////    Behavior* currentBehavior  = (*sequence).behavior;
////    while (currentBehavior != NULL) {
//  
//  
//  
//  
//    // TODO: Substrate: Remove reference to behavior 
//    
//    // TODO: Sequence: Remove reference to behavior 
//
//    // TODO: Behavior.previous, Behavior.next: Remove reference to behavior 
//    
//    // Dissociate behavior
//    // TODO: Update to point to "Dissociated Substrate/NULL Substrate" rather than NULL.
//    (*behavior).substrate = NULL;
//      
//      
//      
//      
//      // Proceed to next behavior
////      currentBehavior = (*currentBehavior).next;
//    }
//      
//      // Dissociate sequence
//      // TODO: Update to point to "Dissociated Substrate/NULL Substrate" rather than NULL.
////      (*sequence).substrate = NULL;
//      
//      // Proceed to next sequence
////      sequence = (*sequence).next;
//      
//    }
//    
//  }
//  return sequence;
//}

//! Deletes the specified substrate. Removes all references to the substrate before deleting it.
//!
boolean Delete_Substrate (Substrate* substrate) {
  
  // TODO: Delete substrate "sequences" and "origin"
  
  if (substrate != NULL) {
    
    // TODO: Possibly delete all behaviors from the sequence (or only the ones that are only referenced in this sequence)
    
    // Update the sequence's topologies (for the sequences in the specified substrate being deleted)
    
    // Update sequence topology (i.e., Remove all sequences' references to the substrate)
    Sequence* currentSequence  = (*substrate).sequences;
    while (currentSequence != NULL) {
      
      // Dissociate behavior topology (i.e., Remove all behaviors' references to the substrate)
      Behavior* currentBehavior  = (*currentSequence).behavior;
      while (currentBehavior != NULL) {
        
        // Dissociate behavior
        // TODO: Update to point to "Dissociated Substrate/NULL Substrate" rather than NULL.
        (*currentBehavior).substrate = NULL;
        
        currentBehavior = (*currentBehavior).next;
      }
      
      // Dissociate sequence
      // TODO: Update to point to "Dissociated Substrate/NULL Substrate" rather than NULL.
      (*currentSequence).substrate = NULL;
      
      // Proceed to next sequence
      currentSequence = (*currentSequence).next;
      
    }
    
    // Free the substrate from memory
    free(substrate);
    
    return true;
  }
  
  return false;
  
}

Sequence* Create_Sequence (Substrate* substrate) {
  
  // Create sequence
  Sequence* sequence = (Sequence*) malloc (sizeof (Sequence));
  
  // Initialize sequence
  (*sequence).uid      = NULL;
  (*sequence).type     = SEQUENCE_TYPE_LOOP;
  (*sequence).behavior = NULL;
  (*sequence).size     = 0;
  (*sequence).previous = NULL;
  (*sequence).next     = NULL;
  (*sequence).schema   = NULL;
  
  // TODO: Create sequence schema
  
  // Return sequence
  return sequence;
  
}

//! Deletes a sequence structure, freeing it from memory (specifically, from the heap).
//!
boolean Delete_Sequence (Sequence* sequence) {
  
  if (sequence != NULL) {
    
    // TODO: Possibly delete all behaviors from the sequence (or only the ones that are only referenced in this sequence)
    
    // Update behavior topology
    Sequence* current  = sequence;
    Sequence* previous = (*current).previous;
    Sequence* next     = (*current).next;
    
    // Update the forward sequence
    if (previous != NULL) {
      (*previous).next = next;
    }
    
    // Update the backward sequence
    if (next != NULL) {
      (*next).previous = previous;
    }
    
    // Update substrate if needed
    if ((*substrate).sequences == current) {
      if ((*current).next == NULL) {
        (*substrate).sequences = NULL;
      } else {
        (*substrate).sequences = (*current).next;
      }
    }
    
    if ((*substrate).origin == sequence) {
      (*substrate).origin = NULL;
    }
    
    // Free the sequence from memory
    if ((*sequence).schema != NULL) {
      free((*sequence).schema);
    }
    free(sequence);
    
    return true;
  }
  
  return false;
  
}

boolean Update_Sequence_Substrate (Sequence* sequence, Substrate* substrate) {
  
  // Add sequence to substrate
  if ((*substrate).sequences == NULL) {
    
    Serial.println("First sequence");
    (*substrate).sequences = sequence;
    
  } else {
    
    // Get the last behavior in the loop
    Sequence* lastSequence = (*substrate).sequences;
    while ((*lastSequence).next != NULL) {
      Serial.println("Next sequence");
      lastSequence = (*lastSequence).next;
    }
    
    // Insert at end of the list (iterate to find the last behavior)
    (*sequence).previous = lastSequence; // Set up the pointer from the new behavior to the previous behavior.
    (*lastSequence).next = sequence; // Finally, set up the link to the new behavior.
    
//      (*coreLoop).size = (*coreLoop).size + 1;
  }
  
  return true;
  
}

//! Removes the specified sequence from the specified substrate.
//!
boolean Remove_Sequence_Substrate (Sequence* sequence, Substrate* substrate) {
  
  // Update behavior topology
  Sequence* previousSequence = (*sequence).previous;
  Sequence* nextSequence     = (*sequence).next;
  
  // Update the forward sequence
  if (previousSequence != NULL) {
    (*previousSequence).next = nextSequence;
  }
  
  // Update the backward sequence
  if (nextSequence != NULL) {
    (*nextSequence).previous = previousSequence;
  }
  
  // Update sequence if needed
  if ((*substrate).sequences == sequence) {
    if ((*sequence).next == NULL) {
      (*substrate).sequences = NULL;
    } else {
      (*substrate).sequences = (*sequence).next;
    }
  }
  
  // Dissociate from the substrate
  (*sequence).substrate = NULL;
  
  // Resize the sequence
//  (*sequence).size = (*sequence).size - 1;
  
  return true;
}

boolean Update_Behavior_Sequence (Behavior* behavior, Sequence* sequence) {
  
//  Sequence* coreLoop = (*substrate).sequences;
  if ((*sequence).behavior == NULL) {
    
    Serial.println("First");
    (*sequence).behavior = behavior;
    
  } else {
    
    // Get the last behavior in the loop
    Behavior* lastBehavior = (*sequence).behavior;
    while ((*lastBehavior).next != NULL) {
      Serial.println("Next");
      lastBehavior = (*lastBehavior).next;
    }
    
    // Insert at end of the list (iterate to find the last behavior)
    (*behavior).previous = lastBehavior; // Set up the pointer from the new behavior to the previous behavior.
    (*lastBehavior).next = behavior; // Finally, set up the link to the new behavior.
  }
  (*sequence).size = (*sequence).size + 1;
  
  Serial.print("Loop size: "); Serial.print((*sequence).size); Serial.print("\n");
  
  return true;
  
}

boolean Remove_Behavior_Sequence (Behavior* behavior, Sequence* sequence) {
  
  // Update behavior topology
  Behavior* previousBehavior = (*behavior).previous;
  Behavior* nextBehavior     = (*behavior).next;
  
  // Update the forward sequence
  if (previousBehavior != NULL) {
    (*previousBehavior).next = nextBehavior;
  }
  
  // Update the backward sequence
  if (nextBehavior != NULL) {
    (*nextBehavior).previous = previousBehavior;
  }
  
  // Update sequence if needed
  if ((*sequence).behavior == behavior) {
    if ((*behavior).next == NULL) {
      (*sequence).behavior = NULL;
    } else {
      (*sequence).behavior = (*behavior).next;
    }
  }
  
  // Resize the sequence
  (*sequence).size = (*sequence).size - 1;
  
  return false;
}

//! Creates an Output
//!
Behavior* Create_Output_Behavior (Substrate* substrate, int pin, String signal, String data) {
  
  Behavior* behavior = NULL;
  
  Serial.println("Create_Output_Behavior");
  
  if (substrate != NULL) {
    
    Serial.println(pin);
    Serial.println(signal);
    Serial.println(data);
    
    // Parse and validate parameters
    int signal2 = 0;
    if (signal.compareTo("digital") == 0) {
      signal2 = SIGNAL_DIGITAL;
    } else if (signal.compareTo("analog") == 0) {
      signal2 = SIGNAL_ANALOG;
    } else {
      return NULL;
    }
    
    Serial.println("Parsed signal");
    
    int data2 = 0;
    if (data.compareTo("on") == 0) {
      Serial.println("on");
      data2 = DATA_ON;
    } else if (data.compareTo("off") == 0) {
      Serial.println("off");
      data2 = DATA_OFF;
    } else {
      Serial.println("NULL");
      Serial.println(data.length());
      return NULL;
    }
    
    Serial.println("Parsed data");
    
    Serial.println("CREATING OUTPUT BEHAVIOR");
    
    // Create the Output schema for Behavior
    Output* output   = (Output*) malloc (sizeof (Output));
    (*output).pin    = pin;
    (*output).signal = signal2;
    (*output).data   = data2;
    
    // Create the Behavior
    behavior = Create_Behavior (substrate);
    (*behavior).type   = BEHAVIOR_TYPE_OUTPUT;
    (*behavior).schema = (void *) output;
    
    // Associate the created Output schema with the corresponding created Behavior
    (*output).behavior = behavior;
    
    // Parse behavior schema parameters
    Serial.println(pin);
    Serial.println(signal);
    Serial.println(data);
    
//    // Set up the behavior schema
//    if ((*behavior).type == BEHAVIOR_TYPE_INPUT) {
//      Input* input = (Input*) malloc(sizeof(Input));
//      (*behavior).schema = input;
//    } else {
//      // TODO: Handle schema creation for other behavior types
//    }
    
//    Serial.println((int)(*behavior).schema);
    
//    if ((*behavior).type == BEHAVIOR_TYPE_INPUT) {
//      Input* in = (Input*) (*behavior).schema;
//    }
    
    // Add the behavior to the loop
//    Sequence* sequence = (*substrate).sequences; // HACK: TODO: Change this! Possibly add a pointer to the substrate and allow a NULL sequence.
//    sequence_addBehavior(sequence, behavior);
    
  }
  
  return behavior;
}

//! Returns the Output schema for the Behavior.
//!
Output* Get_Output_Behavior (Behavior* behavior) {
  return ((Output*) (*behavior).schema);
}

//! Creates an Output
//!
Behavior* Create_Input_Behavior (Substrate* substrate, int pin, String signal, String data) {
  
  Behavior* behavior = NULL;
  
  Serial.println("Create_Input_Behavior");
  
  if (substrate != NULL) {
    
    Serial.println(pin);
    Serial.println(signal);
    Serial.println(data);
    
    // Parse and validate parameters
    int signal2 = 0;
    if (signal.compareTo("digital") == 0) {
      signal2 = SIGNAL_DIGITAL;
    } else if (signal.compareTo("analog") == 0) {
      signal2 = SIGNAL_ANALOG;
    } else {
      return NULL;
    }
    
    Serial.println("Parsed signal");
    
    int data2 = 0;
    if (data.compareTo("on") == 0) {
      Serial.println("on");
      data2 = DATA_ON;
    } else if (data.compareTo("off") == 0) {
      Serial.println("off");
      data2 = DATA_OFF;
    } else {
      Serial.println("NULL");
      Serial.println(data.length());
      return NULL;
    }
    
    Serial.println("Parsed data");
    
    Serial.println("CREATING INPUT BEHAVIOR");
    
    // Create the Output schema for Behavior
    Input* input   = (Input*) malloc (sizeof (Input));
    (*input).pin    = pin;
    (*input).signal = signal2;
    (*input).data   = data2;
    
    // Create the Behavior
    behavior = Create_Behavior (substrate);
    (*behavior).type   = BEHAVIOR_TYPE_INPUT;
    (*behavior).schema = (void *) input;
    
    // Associate the created Output schema with the corresponding created Behavior
    (*input).behavior = behavior;
    
    // Parse behavior schema parameters
    Serial.println(pin);
    Serial.println(signal);
    Serial.println(data);
    
//    // Set up the behavior schema
//    if ((*behavior).type == BEHAVIOR_TYPE_INPUT) {
//      Input* input = (Input*) malloc(sizeof(Input));
//      (*behavior).schema = input;
//    } else {
//      // TODO: Handle schema creation for other behavior types
//    }
    
//    Serial.println((int)(*behavior).schema);
    
//    if ((*behavior).type == BEHAVIOR_TYPE_INPUT) {
//      Input* in = (Input*) (*behavior).schema;
//    }
    
    // Add the behavior to the loop
//    Sequence* sequence = (*substrate).sequences; // HACK: TODO: Change this! Possibly add a pointer to the substrate and allow a NULL sequence.
//    sequence_addBehavior(sequence, behavior);
    
  }
  
  return behavior;
}

Input* Get_Input_Behavior (Behavior* behavior) {
  return ((Input*) (*behavior).schema);
}

// TODO: Consider: Behavior* Create_Behavior (String type, void* schema), at least internally to this method... called by the method as part of the process.
Behavior* Create_Behavior (Substrate* substrate) {
  
  // Create a behavior
  Behavior* behavior    = (Behavior*) malloc (sizeof (Behavior));
  (*behavior).uid       = 0;
  (*behavior).type      = BEHAVIOR_TYPE_NONE;
  (*behavior).schema    = NULL;
  (*behavior).substrate = substrate;
  (*behavior).previous  = NULL;
  (*behavior).next      = NULL;
  
  // Generate UUID for the behavior
  (*behavior).uid  = generateUuid();
  
//  // Parse behavior type parameters
//  if (type.compareTo("output") == 0) {
//    (*behavior).type = BEHAVIOR_TYPE_OUTPUT;
//  } else if (type.compareTo("input") == 0) {
//    (*behavior).type = BEHAVIOR_TYPE_INPUT;
//  } else if (type.compareTo("delay") == 0) {
//    (*behavior).type = BEHAVIOR_TYPE_DELAY;
//  } else {
//    (*behavior).type = BEHAVIOR_TYPE_NONE;
//  }
  
  // Set up the behavior schema and parse parameters accordingly
//  if ((*behavior).type == BEHAVIOR_TYPE_OUTPUT) {
//    Output* output = Create_Output_Behavior (behavior, pin, signal, data);
//    (*behavior).schema = (void *) output;
//  } else {
//    // TODO: Handle schema creation for other behavior types
//  }
  
//  // Add the behavior to the loop
//  Sequence* sequence = (*substrate).sequences; // HACK: TODO: Change this! Possibly add a pointer to the substrate and allow a NULL sequence.
//  sequence_addBehavior(sequence, behavior);
  
  return behavior;
}

/**
 * Returns a pointer to behavior node at specified index.
 */
Behavior* Get_Behavior (int uid) {
    
  // Get pointer to behavior node at specified index
  Behavior* behavior = NULL;
  
  // TODO: Search the sequences for the specified behavior
  
  // Search the loop for the behavior with the specified UID.
  if (substrate != NULL) {
    
    // Get the last behavior in the loop
    Sequence* currentSequence = (*substrate).sequences;
    while (currentSequence != NULL) {
      Serial.println("Searching sequence");
      
      // Get the last behavior in the loop
      Behavior* soughtBehavior = (*currentSequence).behavior;
      while (soughtBehavior != NULL) {
        Serial.println("Searching behavior");
        
        // Return the behavior if it has been found
        if ((*soughtBehavior).uid == uid) {
          return soughtBehavior;
        }
        
        soughtBehavior = (*soughtBehavior).next;
      }
      
      currentSequence = (*currentSequence).next;
    }
    
  }
  
  return behavior;
}

Behavior* Update_Behavior (int uid) {
    
  // Get pointer to behavior node at specified index
  Behavior* behavior = NULL;
  
  // Search the loop for the behavior with the specified UID.
  if (substrate != NULL) {
    
    // Get the last behavior in the loop
    Sequence* currentSequence = (*substrate).sequences;
    while (currentSequence != NULL) {
      Serial.println("Searching sequence");
      
      // Get the last behavior in the loop
      Behavior* soughtBehavior = (*currentSequence).behavior;
      while (soughtBehavior != NULL) {
        Serial.println("Searching behavior");
        
        // Return the behavior if it has been found
        if ((*soughtBehavior).uid == uid) {
          
          // TODO: Update the behavior
          
          return soughtBehavior;
        }
        
        soughtBehavior = (*soughtBehavior).next;
      }
      
      currentSequence = (*currentSequence).next;
    }
    
  }
  
  return behavior;
}

boolean Delete_Behavior (int uid) {
    
  // Get pointer to behavior node at specified index
  Behavior* behavior = NULL;
  
  // Search the loop for the behavior with the specified UID.
  if (substrate != NULL) {
    
    // Get the last behavior in the loop
    Sequence* currentSequence = (*substrate).sequences;
    while (currentSequence != NULL) {
      Serial.println("Searching sequence");
      
      // Get the last behavior in the loop
      Behavior* soughtBehavior = (*currentSequence).behavior;
      while (soughtBehavior != NULL) {
        Serial.println("Searching behavior");
        
        // Return the behavior if it has been found
        if ((*soughtBehavior).uid == uid) {
          
          Serial.println("Deleting behavior");
          
          int isRemoved = Remove_Behavior_Sequence (soughtBehavior, currentSequence);
          
          // Free the behavior from memory
//          assert(soughtBehavior != NULL);
          free((*soughtBehavior).schema); // Free the behavior's schema from memory
          free(soughtBehavior); // Free the behavior from memory
          
          return true;
        }
        
        soughtBehavior = (*soughtBehavior).next;
      }
      
      currentSequence = (*currentSequence).next;
    }
    
  }

  return false;
}










//struct PropagationBuffer {
//  
//};

struct Behavior_Transformation {
  int type;
  void* behavior; // i.e., Substrate, Behavior, Sequence
};

//struct BehaviorDescription {
//  int type;
//  void* behavior;
//};

//boolean Propagate_Behavior_Transformation (int transformationType) {
//}

boolean Propagate_Create_Loop () {
  return false;
}

boolean Propagate_Update_Loop () {
  return false;
}

//boolean appendLoopNode (Behavior* behavior) {
//boolean Propagate_Behavior (Behavior* behavior) {
//boolean Propagate (int description) {
//boolean Propagate_Request (Behavior* behavior) {
boolean Propagate_Create_Output_Behavior (Behavior* behavior) {
  
  // POST
  // /behavior
  // ?type=output
  // &pin=
  // &signal=
  // &data=
  
//  // Loop
//  #define CREATE_LOOP 1
//  
//  // Behavior
//  #define CREATE_BEHAVIOR 10
//  #define GET_BEHAVIOR 11

//  "1 CREATE_LOOP"
//  "1 uid 34234"
//  "1 DONE"

//  "1 CREATE_BEHAVIOR"
//  "1 type BEHAVIOR_TYPE_INPUT"
//  "1 pin 13"
//  "1 signal SIGNAL_DIGITAL"
//  "1 data DATA_ON"
//  "1 DONE"



  
//  if (behavior != NULL) {
//    
//    if ((*behavior).type == BEHAVIOR_TYPE_NONE) {
//      
//    } else if ((*behavior).type == BEHAVIOR_TYPE_OUTPUT) {
//      
//      
//      
//    } else if ((*behavior).type == BEHAVIOR_TYPE_INPUT) {
//      
//      Input* input = (Input*) (*behavior).schema;
//      Queue_Behavior_Propagation ((*input).pin, (*input).signal, BEHAVIOR_TYPE_INPUT, (*input).data);
//      
//    } else if ((*behavior).type == BEHAVIOR_TYPE_DELAY) {
//      Delay* delay = (Delay*) (*behavior).schema;
//    }
//    
//  }
  
//  if (loopSize < DEFAULT_LOOP_CAPACITY) {
//    // Add behavior to queue
////    behaviorLoop[loopSize].id = generateBehaviorIdentifier();
//    behaviorLoop[loopSize].operation = operation;
//    behaviorLoop[loopSize].pin = pin;
//    behaviorLoop[loopSize].type = type;
//    // behaviorLoop[loopSize].mode = mode;
//    behaviorLoop[loopSize].value = value;
//    
//    loopSize++; // Increment the loop size
//    
//    return true;
//  }
//  
//  return false;
}

//boolean Queue_Behavior_Propagation (int pin, int operation, int type, int value) {
//  // TODO: Add message to queue... and use sendMessage to send the messages...
//  
//  if (loopSize < DEFAULT_LOOP_CAPACITY) {
//    // Add behavior to queue
////    behaviorLoop[loopSize].id = generateBehaviorIdentifier();
//    behaviorLoop[loopSize].operation = operation;
//    behaviorLoop[loopSize].pin = pin;
//    behaviorLoop[loopSize].type = type;
//    // behaviorLoop[loopSize].mode = mode;
//    behaviorLoop[loopSize].value = value;
//    
//    loopSize++; // Increment the loop size
//    
//    return true;
//  }
//  
//  return false;
//}


/**
 * Insert a behavior node into the behavior loop at the specified index.
 */
//boolean appendLoopNode(int pin, int operation, int type, int value) {
//  // TODO: Add message to queue... and use sendMessage to send the messages...
//  
//  if (loopSize < DEFAULT_LOOP_CAPACITY) {
//    // Add behavior to queue
////    behaviorLoop[loopSize].id = generateBehaviorIdentifier();
//    behaviorLoop[loopSize].operation = operation;
//    behaviorLoop[loopSize].pin = pin;
//    behaviorLoop[loopSize].type = type;
//    // behaviorLoop[loopSize].mode = mode;
//    behaviorLoop[loopSize].value = value;
//    
//    loopSize++; // Increment the loop size
//    
//    return true;
//  }
//  
//  return false;
//}

/**
 * Insert a behavior node into the behavior loop at the specified index.
 */
boolean applyBehaviorTransformation(int index, int pin, int operation, int type, int value) {
  // TODO: Add message to queue... and use sendMessage to send the messages...
  
  if (operation == BEHAVIOR_DELETE) {
    
    // Check if index is in valid range
    if (index >= 0 && index < DEFAULT_LOOP_CAPACITY) {
      Serial.print("index = "); Serial.print(index); Serial.println();
      
      // Move any behaviors back if needed
      //for (int i = index; i < loopSize; i++) {
      for (int i = index; i < loopSize; i++) {
//        behaviorLoop[i].uid = behaviorLoop[i + 1].uid;
//        behaviorLoop[i].operation = behaviorLoop[i + 1].operation;
//        behaviorLoop[i].pin = behaviorLoop[i + 1].pin;
//        behaviorLoop[i].type = behaviorLoop[i + 1].type;
//        // behaviorLoop[i].mode = behaviorLoop[i + 1].mode;
//        behaviorLoop[i].value = behaviorLoop[i + 1].value;
      }
      
      // Update the beahvior counter if needed
      if (loopCounter >= index) {
        loopCounter--;
      }
      
      loopSize--;
      
      return true;
    }
    
    return false;
    
  } else if (operation == BEHAVIOR_UPDATE) {

    // Check if the behavior index is in bounds
    if (index < 0  || index >= loopSize) {
      return false;
    }
    
    Serial.print("index = "); Serial.print(index); Serial.println();
    
//    // Add behavior to queue
//    //behaviorLoop[loopSize].uid = generateBehaviorIdentifier();
//    //behaviorLoop[index].operation = operation;
//    behaviorLoop[index].pin = pin;
//    behaviorLoop[index].type = type;
//    // behaviorLoop[index].mode = mode;
//    behaviorLoop[index].value = value;
    
//    Serial.print("value = "); Serial.print(behaviorLoop[index].value); Serial.println();
    
//    setPinValue2 (index, value);
    
    // TODO: Update the delay behavior.
    // Set up support structures for the behavior
//    if (operation == BEHAVIOR_DELAY) {
//      // Set up timer
//      delays[delayCount].startTime = 0; // Initialize/Reset the timer
//      delays[delayCount].duration = behaviorLoop[index].value;
//      delays[delayCount].behavior = &behaviorLoop[index];
//      
//      Serial.print("Creating delay...");
//      Serial.print(delays[delayCount].duration);
//      Serial.println();
//      
//      delayCount++;
//    }

    return true;
    
  } else if (operation == BEHAVIOR_REBOOT) {
    
    int SLAVE_DEVICE_ADDRESS = 2;
    
    // Send message to slave to reboot
    char buf[8];
    Wire.beginTransmission(SLAVE_DEVICE_ADDRESS); // transmit to device #4
    Wire.write("reboot  ");
    Wire.endTransmission();    // stop transmitting
    
    _reboot_Teensyduino_();
//    while(1) { /* NOTE: This is an infinite loop! */ }
    
    return true;
    
  } else if (operation == STATUS_WIFI_CONNECTED) {
    
    Serial.println("Wi-Fi Connected!");
    
    return true;
    
  } else if (loopSize < DEFAULT_LOOP_CAPACITY) {

    // If index is -1 or greater than the loopSize then add the beahvior transformation to the end of the loop
    if (index == -1 || index > loopSize) {
      index = loopSize;
    }
    
    Serial.print("index = "); Serial.print(index); Serial.println();
    
    // Move any behaviors back if needed
    //for (int i = index; i < loopSize; i++) {
    for (int i = loopSize; i > index; i--) {
//      behaviorLoop[i].uid = behaviorLoop[i - 1].uid;
//      behaviorLoop[i].operation = behaviorLoop[i - 1].operation;
//      behaviorLoop[i].pin = behaviorLoop[i - 1].pin;
//      behaviorLoop[i].type = behaviorLoop[i - 1].type;
//      // behaviorLoop[i].mode = behaviorLoop[i - 1].mode;
//      behaviorLoop[i].value = behaviorLoop[i - 1].value;
    }
    
    // Update the beahvior counter if needed
    if (loopCounter >= index) {
      loopCounter++;
    }
    
//    // Add behavior to queue
//    behaviorLoop[loopSize].uid = generateBehaviorIdentifier();
//    behaviorLoop[index].operation = operation;
//    behaviorLoop[index].pin = pin;
//    behaviorLoop[index].type = type;
//    // behaviorLoop[index].mode = mode;
//    behaviorLoop[index].value = value;
    
    loopSize++; // Increment the loop size
    
    return true;
  }
  
  return false;
}

///**
// * Insert a behavior node into the behavior loop at the specified index.
// */
//boolean insertLoopNode(int index, int pin, int operation, int type, int mode, int value) {
//  // TODO: Add message to queue... and use sendMessage to send the messages...
//  
//  if (behaviorNodeCount < BEHAVIOR_NODE_CAPACITY) {
//    // Add behavior to queue
//    behaviorLoop[behaviorNodeCount].pin = pin;
//    behaviorLoop[behaviorNodeCount].operation = operation;
//    behaviorLoop[behaviorNodeCount].type = type;
//    behaviorLoop[behaviorNodeCount].mode = mode;
//    behaviorLoop[behaviorNodeCount].value = value;
//    behaviorNodeCount++; // Increment the behavior node count
//  }
//  
////  Serial.print("queueing message (size: ");
////  Serial.print(messageQueueSize);
////  Serial.print(")\n");
//}

/**
 * Removes the behavior node at the specified index from the sequence of nodes (if it exists).
 */
boolean removeLoopNode(int index) {
  
  if (loopSize > 0) { // Make sure there at least one behavior node exists
    
    if (index >= 0 && index < loopSize) {
      // Remove the behavior node from the sequence of behavior nodes
      for (int i = index; i < loopSize - 1; i++) {
        behaviorLoop[i] = behaviorLoop[i + 1];
      }
      loopSize--;
      
      return true;
    }
  }

  return false;
}

/**
 * Removes all behaviors from the loop (if any exist).
 */
boolean eraseLoop() {
  
  loopSize = 0; // Reset behavior loop
//  delayCount = 0; // Reset delay structures
  return true;
}


///**
// * Returns a copy of the behavior node at the specified index.
// */
//Behavior getBehaviorCopy(int index) {
//  
//  if (index >= 0 && index < loopSize) {
//    
//    // Copy the behavior node at the specified index
//    Behavior behaviorNode = {
//      behaviorLoop[index].pin,
//      behaviorLoop[index].operation,
//      behaviorLoop[index].type,
//      behaviorLoop[index].mode,
//      behaviorLoop[index].value
//    }; // Get behavior node at specified index
//    
//    return behaviorNode;
//  }
//}

/**
 * Returns a pointer to behavior node at specified index.
 */
Behavior* getBehaviorByIndex(int index) {
  
  if (index >= 0 && index < loopSize) {
    
    // Get pointer to behavior node at specified index
    Behavior* behaviorNode = NULL;
    behaviorNode = &behaviorLoop[index]; // Get behavior node at specified index
    
    return behaviorNode;
  }
  
  return NULL;
}

#endif
