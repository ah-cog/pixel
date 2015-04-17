#ifndef LOOPER_H
#define LOOPER_H

#include "Behavior.h"
#include "Platform.h"

//!
//! Looper Engine
//!

Context* Create_Context ();
Sequence* Create_Sequence (Context* context);
Behavior* Create_Behavior (Context* context);

boolean Contextualize_Sequence (Sequence* sequence, Context* context);
boolean Decontextualize_Sequence (Sequence* sequence, Context* context);

Behavior* Create_Behavior (Context* context);
boolean Desequence_Behavior (Behavior* behavior, Sequence* sequence);

struct Performer;
struct Propagator;

Propagator* propagator = NULL;
Performer* performer = NULL;

Performer* Create_Performer (Context* context);
Propagator* Create_Propagator ();

boolean Perform_Behavior_On_Platform (Behavior* behavior);
void Process_Immediate_Message (String message); // HACK



// Methods:
//
// Create_Context
// Get_Context_Origin
// TODO: Get_Context_Behaviors
// TODO: Get_Context_Sequences
// Update_Context_Origin
// Delete_Context
// TODO: Deep_Delete_Context
//
// Create_Sequence
// Update_Sequence_Context
// Remove_Sequence_Context
// Delete_Sequence
// TODO: Deep_Delete_Sequence
// Dissociate_Sequence
//
// Create_Behavior
// Get_Behavior
// Update_Behavior
// Update_Behavior_Context
// Update_Behavior_Sequence
// Remove_Behavior_Sequence
// Delete_Behavior
// TODO: Deeper_Delete_Behavior/Network_Delete_Behavior (i.e., (1) deep delete behavior and (2) delete everything that was connected to it and everything that was connected to the things connected to it)
// Dissociate_Behavior

//! Setup the Looper engine.
//!
boolean Setup_Looper () {
  
  // Create behavior context
  if (context == NULL) {
    context = Create_Context ();
  }
  
  // Create sequence
  // TODO: Add parameter "Context* context"
  if (context != NULL) {
    
    Sequence* sequence = Create_Sequence (context);
    
    boolean isAdded = Contextualize_Sequence (sequence, context);
    
    currentSequence = sequence; // TODO: Put this into the perspective!
    
//    return isAdded;
  }
  
  performer = Create_Performer (context);
  propagator = Create_Propagator ();
  
  return true;
}

//! Generate and return the UUID.
//!
long Generate_Uuid () {
  long uuid = random (65000L);
  return uuid;
}

//! Create behavior context
//!
Context* Create_Context () {
  
  // Create context
  Context* context = (Context*) malloc (sizeof (Context));
  
  // Initialize sequence
  (*context).sequences = NULL;
  (*context).origin    = NULL;
  
  // Return sequence
  return context;
  
}

//! Returns the context's "origin behavior" (or simply "origin"), which is the first 
//! behavior to be executed by a performer for the context.
//!
Sequence* Get_Context_Sequences (Context* context) {
  Sequence* sequences = NULL;
  
  if (context != NULL) {
    
    // Update sequence topology (i.e., Remove all sequences' references to the context)
    Sequence* currentSequence  = (*context).sequences;
    while (currentSequence != NULL) {
      
      Serial.print ("> sequence ");
      // Serial.print ((int) currentSequence);
      Serial.print ((*currentSequence).uid);
      Serial.print ("\n");
      
      // Proceed to next sequence
      currentSequence = (*currentSequence).next;
      
    }
    
  }
  
  return sequences;
}

//! Returns the context's "origin behavior" (or simply "origin"), which is the first 
//! behavior to be executed by a performer for the context.
//!
Sequence* Get_Context_Behaviors (Context* context) {
  Sequence* sequences = NULL;
  
  if (context != NULL) {
    
    // Update sequence topology (i.e., Remove all sequences' references to the context)
    Sequence* currentSequence  = (*context).sequences;
    while (currentSequence != NULL) {
      
      // Dissociate behavior topology (i.e., Remove all behaviors' references to the context)
      Behavior* currentBehavior  = (*currentSequence).behavior;
      while (currentBehavior != NULL) {
        
        Serial.print ("> behavior ");
        Serial.print ((int) (*currentBehavior).uid);
        Serial.print ("\n");
        
        currentBehavior = (*currentBehavior).next;
      }
      
      // Proceed to next sequence
      currentSequence = (*currentSequence).next;
      
    }
    
  }
  
  return sequences;
}

//! Returns the context's "origin behavior" (or simply "origin"), which is the first 
//! behavior to be executed by a performer for the context.
//!
Sequence* Get_Context_Origin (Context* context) {
  Sequence* origin = NULL;
  
  if (context != NULL) {
    origin = (*context).origin;
  }
  
  return origin;
}

boolean Update_Context_Origin (Context* context, Sequence* origin) {
  if (context != NULL) {
    (*context).origin = origin;
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
      // TODO: Update to point to "Dissociated Context/NULL Context" rather than NULL.
      (*currentBehavior).context = NULL;
      
      // Proceed to the next behavior
      currentBehavior = (*currentBehavior).next;
    }
    
    // Step 2. Remove sequence from context
    boolean hasRemoved = Decontextualize_Sequence (sequence, (*sequence).context);
    
    // Dissociate from sequence
    // TODO: Update to point to "Dissociated Context/NULL Context" rather than NULL.
//    (*sequence).context = NULL;
    
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
//    // TODO: Context: Remove reference to behavior 
//    
//    // TODO: Sequence: Remove reference to behavior 
//
//    // TODO: Behavior.previous, Behavior.next: Remove reference to behavior 
//    
//    // Dissociate behavior
//    // TODO: Update to point to "Dissociated Context/NULL Context" rather than NULL.
//    (*behavior).context = NULL;
//      
//      
//      
//      
//      // Proceed to next behavior
////      currentBehavior = (*currentBehavior).next;
//    }
//      
//      // Dissociate sequence
//      // TODO: Update to point to "Dissociated Context/NULL Context" rather than NULL.
////      (*sequence).context = NULL;
//      
//      // Proceed to next sequence
////      sequence = (*sequence).next;
//      
//    }
//    
//  }
//  return sequence;
//}

//! Deletes the specified context. Removes all references to the context before deleting it.
//!
boolean Delete_Context (Context* context) {
  
  // TODO: Delete context "sequences" and "origin"
  
  if (context != NULL) {
    
    // TODO: Possibly delete all behaviors from the sequence (or only the ones that are only referenced in this sequence)
    
    // Update the sequence's topologies (for the sequences in the specified context being deleted)
    
    // Update sequence topology (i.e., Remove all sequences' references to the context)
    Sequence* currentSequence  = (*context).sequences;
    while (currentSequence != NULL) {
      
      // Dissociate behavior topology (i.e., Remove all behaviors' references to the context)
      Behavior* currentBehavior  = (*currentSequence).behavior;
      while (currentBehavior != NULL) {
        
        // Dissociate behavior
        // TODO: Update to point to "Dissociated Context/NULL Context" rather than NULL.
        (*currentBehavior).context = NULL;
        
        currentBehavior = (*currentBehavior).next;
      }
      
      // Dissociate sequence
      // TODO: Update to point to "Dissociated Context/NULL Context" rather than NULL.
      (*currentSequence).context = NULL;
      
      // Proceed to next sequence
      currentSequence = (*currentSequence).next;
      
    }
    
    // Free the context from memory
    free(context);
    
    return true;
  }
  
  return false;
  
}

//! Creates a sequence into which behaviors can be placed.
//!
Sequence* Create_Sequence (Context* context) {
  Serial.println ("Create_Sequence");
  
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
  
  // Create UUID for the sequence
  (*sequence).uid      = Generate_Uuid ();
  
  // TODO: Create sequence schema
  
  Serial.print ("\ttype: "); Serial.print ((*sequence).type); Serial.print ("\n");
  
  // Return sequence
  return sequence;
  
}

//
// Returns a pointer to the sequence with the specified UID.
//
Sequence* Get_Sequence (int uid) {
    
  // The pointer to the sequence
  Sequence* sequence = NULL;
  
  // Search the loop for the behavior with the specified UID.
  if (context != NULL) {
    
    // Get the last behavior in the loop
    Sequence* currentSequence = (*context).sequences;
    while (currentSequence != NULL) {
      Serial.println("Searching sequence");

      // Return the sequence if it has been found
      if ((*currentSequence).uid == uid) {
        sequence = currentSequence;
        break;
      }
      
      currentSequence = (*currentSequence).next;
    }
    
  }
  
  return sequence;
}

int Get_Sequence_Type (Sequence* sequence) {
  if (sequence != NULL) {
    return (*sequence).type;
  }
  return SEQUENCE_TYPE_NONE;
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
    
    // Update context if needed
    if ((*context).sequences == current) {
      if ((*current).next == NULL) {
        (*context).sequences = NULL;
      } else {
        (*context).sequences = (*current).next;
      }
    }
    
    if ((*context).origin == sequence) {
      (*context).origin = NULL;
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

//! Adds the specified sequence to the specified context.
//!
boolean Contextualize_Sequence (Sequence* sequence, Context* context) {
  
  // Add sequence to context
  if ((*context).sequences == NULL) {
    
    Serial.println("First sequence");
    (*context).sequences = sequence;
    
    // Set the context's origin if it has not been set
    if ((*context).origin == NULL) {
      (*context).origin = sequence;
    }
    
  } else {
    
    // Get the last behavior in the loop
    Sequence* lastSequence = (*context).sequences;
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

//! Removes the specified sequence from the specified context.
//!
boolean Decontextualize_Sequence (Sequence* sequence, Context* context) {
  
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
  if ((*context).sequences == sequence) {
    if ((*sequence).next == NULL) {
      (*context).sequences = NULL;
    } else {
      (*context).sequences = (*sequence).next;
    }
  }
  
  // Dissociate from the context
  (*sequence).context = NULL;
  
  // Resize the sequence
//  (*sequence).size = (*sequence).size - 1;
  
  return true;
}

//! Adds the specified behavior to the specified sequence.
//!
boolean Sequence_Behavior (Behavior* behavior, Sequence* sequence) {
  Serial.println ("Sequence_Behavior");
  
  Serial.print ("\tsequence type: "); Serial.print ((*sequence).type); Serial.print ("\n");
  
//  Sequence* coreLoop = (*context).sequences;
  if ((*sequence).behavior == NULL) {
    
    Serial.println("First");
    (*sequence).behavior = behavior;
    
  } else {
    
    Serial.println("Next");
    
    // Get the last behavior in the loop
    Behavior* lastBehavior = (*sequence).behavior;
    while ((*lastBehavior).next != NULL) {
      Serial.println("Next (again)");
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

//! Removes the specified behavior from the specified sequence
//!
boolean Desequence_Behavior (Behavior* behavior, Sequence* sequence) {
  
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

// TODO: Hack the default behavior to add to the single default loop...
// TODO: Create Light behavior
// TODO: Create Memory behavior
// TODO: Create Communication behavior
// TODO: Remove Language behavior?
// TODO: Update Delay to Time behavior

//! Creates an Output
//!
Behavior* Create_Output_Behavior (Context* context, int pin, String signal, String data) {
  
  Behavior* behavior = NULL;
  
  Serial.println("Create_Output_Behavior");
  
  if (context != NULL) {
    
    Serial.println (pin);
    Serial.println (signal);
    Serial.println (data);
    
    // Parse and validate parameters
    int signal2 = 0;
    if (signal.compareTo ("digital") == 0) {
      signal2 = SIGNAL_DIGITAL;
    } else if (signal.compareTo ("analog") == 0) {
      signal2 = SIGNAL_ANALOG;
    } else {
      return NULL;
    }
    
    Serial.println ("Parsed signal");
    
    int data2 = 0;
    if (data.compareTo ("on") == 0) {
      Serial.println ("on");
      data2 = DATA_ON;
    } else if (data.compareTo ("off") == 0) {
      Serial.println ("off");
      data2 = DATA_OFF;
    } else {
      Serial.println ("NULL");
      Serial.println (data.length ());
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
    behavior = Create_Behavior (context);
    (*behavior).type   = BEHAVIOR_TYPE_OUTPUT;
    (*behavior).schema = (void *) output;
    
    // Associate the created Output schema with the corresponding created Behavior
    (*output).behavior = behavior;
    
    // Parse behavior schema parameters
    Serial.println (pin);
    Serial.println (signal);
    Serial.println (data);
    
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
//    Sequence* sequence = (*context).sequences; // HACK: TODO: Change this! Possibly add a pointer to the context and allow a NULL sequence.
//    sequence_addBehavior(sequence, behavior);
    
  }
  
  return behavior;
}

//! Returns the Output schema for the Behavior.
//!
Output* Get_Output_Behavior (Behavior* behavior) {
  return ((Output*) (*behavior).schema);
}

//! Creates an Input
//!
Behavior* Create_Input_Behavior (Context* context, int pin, String signal) {
  
  Behavior* behavior = NULL;
  
  Serial.println("Create_Input_Behavior");
  
  if (context != NULL) {
    
    Serial.println(pin);
    Serial.println(signal);
    
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
    
    Serial.println("CREATING INPUT BEHAVIOR");
    
    // Create the Output schema for Behavior
    Input* input   = (Input*) malloc (sizeof (Input));
    (*input).pin    = pin;
    (*input).signal = signal2;
//    (*input).data   = data2;
    
    // Create the Behavior
    behavior = Create_Behavior (context);
    (*behavior).type   = BEHAVIOR_TYPE_INPUT;
    (*behavior).schema = (void *) input;
    
    // Associate the created Output schema with the corresponding created Behavior
    (*input).behavior = behavior;
    
    // Parse behavior schema parameters
    Serial.println(pin);
    Serial.println(signal);
//    Serial.println(data);
    
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
//    Sequence* sequence = (*context).sequences; // HACK: TODO: Change this! Possibly add a pointer to the context and allow a NULL sequence.
//    sequence_addBehavior(sequence, behavior);
    
  }
  
  return behavior;
}

Input* Get_Input_Behavior (Behavior* behavior) {
  return ((Input*) (*behavior).schema);
}

//! Creates an Output
//!
Behavior* Create_Delay_Behavior (Context* context, int milliseconds) {
  
  Behavior* behavior = NULL;
  
  Serial.println("Create_Delay_Behavior");
  
  if (context != NULL) {
    
    Serial.println(milliseconds);
    
    // Parse and validate parameters
    if (milliseconds < 0) {
      return NULL;
    }
    
    Serial.println("Parsed data");
    
    Serial.println("CREATING DELAY BEHAVIOR");
    
    // Create the Output schema for Behavior
    Delay* delay = (Delay*) malloc (sizeof (Delay));
    (*delay).milliseconds = milliseconds;
    (*delay).startTime = 0L;
    (*delay).currentTime = 0L;
    
    // Create the Behavior
    behavior = Create_Behavior (context);
    (*behavior).type   = BEHAVIOR_TYPE_DELAY;
    (*behavior).schema = (void *) delay;
    
    // Associate the created Output schema with the corresponding created Behavior
    (*delay).behavior = behavior;
    
    // Parse behavior schema parameters
    Serial.println(milliseconds);
    
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
//    Sequence* sequence = (*context).sequences; // HACK: TODO: Change this! Possibly add a pointer to the context and allow a NULL sequence.
//    sequence_addBehavior(sequence, behavior);
    
  }
  
  return behavior;
}

Delay* Get_Delay_Behavior (Behavior* behavior) {
  return ((Delay*) (*behavior).schema);
}

//! Creates a Sound
//!
Behavior* Create_Sound_Behavior (Context* context, int note, int duration) {
  
  Behavior* behavior = NULL;
  
  Serial.println("Create_Sound_Behavior");
  
  if (context != NULL) {
    
    Serial.println (note);
    Serial.println (duration);
    
    // Parse and validate parameters
//    int signal2 = 0;
//    if (signal.compareTo ("digital") == 0) {
//      signal2 = SIGNAL_DIGITAL;
//    } else if (signal.compareTo ("analog") == 0) {
//      signal2 = SIGNAL_ANALOG;
//    } else {
//      return NULL;
//    }
    
//    Serial.println("Parsed signal");
    
    Serial.println("CREATING SOUND BEHAVIOR");
    
    // Create the Output schema for Behavior
    Sound* sound   = (Sound*) malloc (sizeof (Sound));
    (*sound).note     = note;
    (*sound).duration = duration;
    
    // Create the Behavior
    behavior = Create_Behavior (context);
    (*behavior).type   = BEHAVIOR_TYPE_SOUND;
    (*behavior).schema = (void *) sound;
    
    // Associate the created Output schema with the corresponding created Behavior
    (*sound).behavior = behavior;
    
    // Parse behavior schema parameters
    Serial.println (note);
    Serial.println (duration);
//    Serial.println(data);
    
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
//    Sequence* sequence = (*context).sequences; // HACK: TODO: Change this! Possibly add a pointer to the context and allow a NULL sequence.
//    sequence_addBehavior(sequence, behavior);
    
  }
  
  return behavior;
}

Sound* Get_Sound_Behavior (Behavior* behavior) {
  return ((Sound*) (*behavior).schema);
}

//! Create message (i.e., a behavior transformation).
//!
Behavior* Create_Immediate_Behavior (Context* context, String message) {
  
  Behavior* behavior = NULL;
  
  Serial.println("Create_Immediate_Behavior");
  
  if (context != NULL) {
    
//    Serial.println("CREATING IMMEDIATE BEHAVIOR");
    
    // Create the Output schema for Behavior
    Immediate* immediate   = (Immediate*) malloc (sizeof (Immediate));
    (*immediate).messageSize = message.length ();
    (*immediate).message = (char*) malloc (((*immediate).messageSize + 1) * sizeof (char));
    message.toCharArray ((*immediate).message, ((*immediate).messageSize + 1));    
    
    // Create the Behavior
    behavior = Create_Behavior (context);
    (*behavior).type = BEHAVIOR_TYPE_IMMEDIATE;
    (*behavior).schema = (void *) immediate;
    
    // Associate the created Output schema with the corresponding created Behavior
    (*immediate).behavior = behavior;
    
    // Parse behavior schema parameters
//    Serial.println ((*immediate).messageSize);
//    Serial.println ((*immediate).message);
    
  }
  
  return behavior;
  
}

//! Creates a Sequence Behavior
//!
Behavior* Create_Abstract_Behavior (Context* context, Sequence* sequence) {
  
  Behavior* behavior = NULL;
  
  Serial.println("Create_Sequence_Behavior");
  
  if (context != NULL) {
    
//    Serial.println ((int) sequence);
    
    // TODO: Parse and validate parameters
    
    Serial.println("CREATING SEQUENCE BEHAVIOR");
    
    // Create the Output schema for Behavior
    Abstract* abstract = (Abstract*) malloc (sizeof (Abstract));
//    Immediate* immediate   = (Immediate*) malloc (sizeof (Immediate));
    (*abstract).sequence = sequence;
    
    // Create the Behavior
    behavior           = Create_Behavior (context);
    (*behavior).type   = BEHAVIOR_TYPE_ABSTRACT;
    (*behavior).schema = (void *) abstract;
    
    // Associate the created Output schema with the corresponding created Behavior
    (*abstract).behavior = behavior;
    
    // Parse behavior schema parameters
//    Serial.println (pin);
//    Serial.println (signal);
//    Serial.println(data);
    
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
//    Sequence* sequence = (*context).sequences; // HACK: TODO: Change this! Possibly add a pointer to the context and allow a NULL sequence.
//    sequence_addBehavior(sequence, behavior);
    
  }
  
  return behavior;
}

Abstract* Get_Abstract_Behavior (Behavior* behavior) {
  return ((Abstract*) (*behavior).schema);
}

// TODO: Consider: Behavior* Create_Behavior (String type, void* schema), at least internally to this method... called by the method as part of the process.
Behavior* Create_Behavior (Context* context) {
  
  // Create a behavior
  Behavior* behavior    = (Behavior*) malloc (sizeof (Behavior));
  (*behavior).uid       = 0;
  (*behavior).type      = BEHAVIOR_TYPE_NONE;
  (*behavior).schema    = NULL;
  (*behavior).context = context;
  (*behavior).previous  = NULL;
  (*behavior).next      = NULL;
  
  // Generate UUID for the behavior
  (*behavior).uid  = Generate_Uuid ();
  
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
//  Sequence* sequence = (*context).sequences; // HACK: TODO: Change this! Possibly add a pointer to the context and allow a NULL sequence.
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
  if (context != NULL) {
    
    // Get the last behavior in the loop
    Sequence* currentSequence = (*context).sequences;
    while (currentSequence != NULL) {
      Serial.println("Searching sequence");
      
      // Get the last behavior in the loop
      Behavior* soughtBehavior = (*currentSequence).behavior;
      while (soughtBehavior != NULL) {
        Serial.println("Searching behavior");
        
        Serial.println ((*soughtBehavior).uid);
        
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
  if (context != NULL) {
    
    // Get the last behavior in the loop
    Sequence* currentSequence = (*context).sequences;
    while (currentSequence != NULL) {
      Serial.println ("Searching sequence");
      
      // Get the last behavior in the loop
      Behavior* soughtBehavior = (*currentSequence).behavior;
      while (soughtBehavior != NULL) {
        Serial.println ("Searching behavior");
        
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

boolean Delete_Behavior_Schema (Behavior* behavior) { // i.e., Delete_Schema_By_Behavior
  Serial.println ("Delete_Behavior_Schema");

  if (behavior != NULL) {
    
//    if ((*behavior).type == BEHAVIOR_TYPE_OUTPUT) {
//        free ((*behavior).schema); // Free the behavior's schema from memory
//        return true;
//    }

    free ((*behavior).schema); // Free the behavior's schema from memory
    return true;
    
  }
  
  return false;
  
}

//! Deletes all behaviors in the specified sequence
//!
boolean Delete_Sequence_Behaviors (Sequence* sequence) {
  Serial.println ("Delete_Sequence_Behaviors");
  
  if (sequence != NULL) {
    
    Behavior* behavior = (*sequence).behavior;
  
    while (behavior != NULL) {
      
      Serial.println("Deleting behavior");
      
      // TODO: Remove behavior from ALL sequences (maybe replace it with a "NONE" behavior in sequences, while still referencing the behavior in the behavior's history)
//      Sequence* currentSequence = (*context).sequences;
      int isRemoved = Desequence_Behavior (behavior, sequence);
            
      // Free the behavior from memory
      Delete_Behavior_Schema (behavior); // free ((*behavior).schema); // Free the behavior's schema from memory
      free (behavior); // Free the behavior from memory
      
//      behavior = NULL;
      
      
      
      // Get next behavior
//      Behavior* nextBehavior = (*behavior).next;
          
//      Serial.print ("current behavior: ");
//      Serial.println ((int) ((*performer).behavior));
//      Serial.print ("next behavior: ");
//      Serial.println ((int) (*((*performer).behavior)).next);
//      Serial.print ("origin: ");
//      Serial.println ((int) (*((*performer).origin)).behavior);
      
//      if (nextBehavior != NULL) {
        Serial.println("next behavior");
        // Go to next behavior in the sequence
        behavior = (*behavior).next;
//      }
//      else {
//        Serial.println("restart sequence from first behavior");
//        // The end of the looping sequence has been reached, so start again from the beginning of the performer's origin behavior sequence.
//        (*performer).behavior = (*((*performer).origin)).behavior;
//      }
      
    }
    
    return true;
    
  }
  
  return false;
  
}

//! Shows all behaviors in the specified sequence
//!
boolean Show_Sequence_Behaviors (Sequence* sequence) {
  Serial.println ("Show_Sequence_Behaviors");
  
  if (sequence != NULL) {
    
    Behavior* behavior = (*sequence).behavior;
  
    while (behavior != NULL) {
      
      Serial.print ("Behavior ");
      Serial.println ((int) behavior);
      
//      Serial.println("Deleting behavior");
      
      // TODO: Remove behavior from ALL sequences (maybe replace it with a "NONE" behavior in sequences, while still referencing the behavior in the behavior's history)
//      Sequence* currentSequence = (*context).sequences;
//      int isRemoved = Remove_Behavior_Sequence (behavior, sequence);
//            
//      // Free the behavior from memory
//      Delete_Behavior_Schema (behavior); // free ((*behavior).schema); // Free the behavior's schema from memory
//      free (behavior); // Free the behavior from memory
      
//      behavior = NULL;
      
      
      
      // Get next behavior
//      Behavior* nextBehavior = (*behavior).next;
          
//      Serial.print ("current behavior: ");
//      Serial.println ((int) ((*performer).behavior));
//      Serial.print ("next behavior: ");
//      Serial.println ((int) (*((*performer).behavior)).next);
//      Serial.print ("origin: ");
//      Serial.println ((int) (*((*performer).origin)).behavior);
      
//      if (nextBehavior != NULL) {
//        Serial.println("next behavior");
        // Go to next behavior in the sequence
        behavior = (*behavior).next;
//      }
//      else {
//        Serial.println("restart sequence from first behavior");
//        // The end of the looping sequence has been reached, so start again from the beginning of the performer's origin behavior sequence.
//        (*performer).behavior = (*((*performer).origin)).behavior;
//      }
      
    }
    
    return true;
    
  }
  
  return false;
  
}

boolean Delete_Behavior (Behavior* behavior) {
  Serial.println ("Delete_Behavior");
  
  if (behavior != NULL) {
    
    Serial.println("Deleting behavior");
    
    // TODO: Remove behavior from ALL sequences (maybe replace it with a "NONE" behavior in sequences, while still referencing the behavior in the behavior's history)
    Sequence* currentSequence = (*context).sequences;
    int isRemoved = Desequence_Behavior (behavior, currentSequence);
          
    // Free the behavior from memory
    Delete_Behavior_Schema (behavior); // free ((*behavior).schema); // Free the behavior's schema from memory
    free (behavior); // Free the behavior from memory
    
    behavior = NULL;
    
    return true;
    
  }
  
  return false;
  
}

// i.e., delete by behavior URI or "address"
boolean Delete_Behavior_By_Address (int uid) {
  Serial.println ("Delete_Behavior_By_Address");
    
  // Get pointer to behavior node at specified index
  Behavior* behavior = NULL;
  
  // Search the loop for the behavior with the specified UID.
  if (context != NULL) {
    
    // Get the last behavior in the loop
    Sequence* currentSequence = (*context).sequences;
    while (currentSequence != NULL) {
      Serial.println("Searching sequence");
      
      // Get the last behavior in the loop
      Behavior* soughtBehavior = (*currentSequence).behavior;
      while (soughtBehavior != NULL) {
        Serial.println("Searching behavior");
        
        // Return the behavior if it has been found
        if ((*soughtBehavior).uid == uid) {
          
          boolean result = Delete_Behavior (soughtBehavior);
          
          return result;
        }
        
        soughtBehavior = (*soughtBehavior).next;
      }
      
      currentSequence = (*currentSequence).next;
    }
    
  }

  return false;
}



//!
//! Behavior Performer
//!

// Create_Performer
// Get_Performer_Context
// Update_Performer_Context
// Delete_Performer
// Continue_Performance
// Perform_Behavior

//! Behavior Performer
//!
//! The "behavior performer" is something like a program counter. It is what 
//! follows a path of behavior to its conclusion (if any). In a sense, it's like
//! a game piece used in a board came. There can be multiple performers.
//!
//! Performs run according to their own time-scale. That is, one can perform 
//! a sequence of actions faster than another.
//!
//! TODO: Give each performer it's own memory, performance history... essentially its own state and history, as if it is a "Computer" running the behavior.
//!
struct Performer {
  int uid;
  Context* context;
  Behavior* behavior; // i.e., The current behavior. This is akin to a "program counter".
  
  Sequence* origin; // i.e., The performer's first behavior sequence.
  // Behavior* origin; // i.e., The performer's first behavior.
  
  Behavior* immediateBehaviorQueue; // FIFO Queue of immediate behaviors to perform before the next behavior
  
//  Device* device; // i.e., The device upon which the performance will take place.
};

//! Queue the outgoing message.
//! Returns the queue size.
//!
int Queue_Immediate_Behavior (Performer* performer, Behavior* behavior) {
  Serial.println ("Queue_Immediate_Behavior");

  int behaviorCount = 0;

  if (performer != NULL) {
  
    if ((*performer).immediateBehaviorQueue == NULL) {
      
      // Push to the top of the stack (as the first element)
      (*performer).immediateBehaviorQueue = behavior;
      
      // Set up the forward and back links
      (*behavior).previous = NULL;
      (*behavior).next     = NULL;
      
      behaviorCount = 1; // The queue size is now 1
      
    } else {
      
      // Get the last message in the queue
      Behavior* lastBehavior = (*performer).immediateBehaviorQueue;
      while ((*lastBehavior).next != NULL) {
        Serial.print ("\tnext immediate behavior");
        lastBehavior = (*lastBehavior).next;
        
        behaviorCount++; // Count the behavior
      }
      
      // Push to the last position in the queue
      // i.e., add to [ m0, m1, ..., mn ] at m(n + 1) as in [ m0, m1, ..., mn, m(n + 1) ]
      (*lastBehavior).next = behavior;
      
      // Set up the backward and forward queue links
      (*behavior).previous = lastBehavior;
      (*behavior).next = NULL;
      
      behaviorCount++; // Count the behavior just queued
      
    }
    
  }
  
  return behaviorCount;
  
}

//! Dequeue the next outgoing message.
//!
Behavior* Dequeue_Immediate_Behavior (Performer* performer) {
  Serial.println ("Dequeue_Immediate_Behavior");
  
  Behavior* behavior = NULL;
  
  if (performer != NULL) {
  
    if ((*performer).immediateBehaviorQueue != NULL) {
      
      // Get the transformation at the front of the propagator's queue
      behavior = (*performer).immediateBehaviorQueue;
      
      // Update the message queue. Set the message following the dequeued message as the front of the queue.
      (*performer).immediateBehaviorQueue = (*behavior).next;
      
      // Dissociate the dequeued message. Update the backward and forward links of the dequeued message.
      (*behavior).next = NULL;
      (*behavior).previous = NULL;
      
    }
    
  }
  
  return behavior;
  
}

////! Behavior Performer Dialect (e.g., Teensy, Arduino)
////!
//struct TeensyPerformer {
//  int uid;
//  Context* context;
//  Behavior* behavior; // i.e., The current behavior. This is akin to a "program counter".
//  
//  Sequence* origin; // i.e., The performer's first behavior sequence.
//  // Behavior* origin; // i.e., The performer's first behavior.
//  
////  Device* device; // i.e., The device upon which the performance will take place.
//};

//! Creates a behavior performer in the specified context.
//!
Performer* Create_Performer (Context* context) {
  Serial.println ("Create_Performer");
  
  Performer* performer = NULL;
  
  if (context != NULL) {
    
    // Create a performer
    performer              = (Performer*) malloc (sizeof (Performer));
    (*performer).uid       = 0;
    (*performer).context = context;
    (*performer).behavior  = NULL;
    (*performer).origin    = NULL;
    
    (*performer).immediateBehaviorQueue = NULL;
    
    // Generate UUID for the processor
    (*performer).uid  = Generate_Uuid ();
    
    // Initialize the processor's current behavior
    if ((*context).origin != NULL) {
      Serial.println ("Setting performer's origin and behavior.");
      (*performer).origin = (*context).origin;
      (*performer).behavior = (*((*context).origin)).behavior;
      
      Serial.print ("\tsequence type: "); Serial.print ((*((*performer).origin)).type); Serial.print ("\n");
    }
  
  }
  
  return performer;
}

//! Starts the Performer's behavior performance. This is essentially a behavior
//! interpreter (i.e., analogous to a JavaScript interpreter).
//!
boolean Perform_Behavior (Performer* performer) {
  // Serial.println ("Perform_Behavior");
  
  // TODO: Make sure runs the right sequence for the context! 
  
  boolean sustainBehavior = false;
  boolean performanceResult = false;
  
//  boolean deleteBehavior = false;
  
  if (performer != NULL) {
    
    // Perform all immediate behaviors (if any) then delete them before moving on
//    Serial.print ("immediateBehaviorQueue = ");
//    Serial.println ((int) (*performer).immediateBehaviorQueue);
    while ((*performer).immediateBehaviorQueue != NULL) {
      Behavior* immediateBehavior = Dequeue_Immediate_Behavior (performer);
      
      boolean sustainBehavior2 = Perform_Behavior_On_Platform (immediateBehavior);
      
      Delete_Behavior (immediateBehavior);
    }
    
    // Update the Performer
    if ((*performer).behavior == NULL) {
      // Serial.println ("Updating performer's origin and behavior.");
      (*performer).origin = (*((*performer).context)).origin; // (*context).origin
      (*performer).behavior = (*((*((*performer).context)).origin)).behavior;
      
      // Serial.print ("\tsequence type: "); Serial.print ((*((*performer).origin)).type); Serial.print ("\n");
    }
    
    Behavior* behavior = (*performer).behavior;
    // Serial.println ("performer != NULL");
    
    if (behavior != NULL) { // Check if the Behavior is valid.  
      // Serial.println ("behavior != NULL");
      
      sustainBehavior = Perform_Behavior_On_Platform (behavior);
    
//      if ((*behavior).type == BEHAVIOR_TYPE_OUTPUT) {
//        Output* output = (Output*) (*behavior).schema;
//        
//        Serial.print ("Output "); Serial.print ((*output).pin); Serial.print ("\n");
//        Serial.print ("\tdata: "); Serial.print ((*output).data); Serial.print ("\n");
//        
//        // TODO: Call device-specific routine (retreived from cloud to change the device itself).
//        Channel* channel = Get_Channel (platform, (*output).pin);
//        Update_Channel_Value (channel, (*output).data);
////        Get_Channel_Value (channel);
////        Propagate_Channel_Value (channel);
//        
//        // Update the pin's state
////        Update_Virtual_Pin ((*output).pin, (*output).signal, (*output).data);
//      } else if ((*behavior).type == BEHAVIOR_TYPE_INPUT) {
//        Input* input = (Input*) (*behavior).schema;
//        
//        Serial.print ("Input "); Serial.print ((*input).pin); Serial.print ("\n");
//        
//        // TODO: Call device-specific routine (retreived from cloud to change the device itself).
//        Channel* channel = Get_Channel (platform, (*input).pin);
////        Update_Channel_Value (channel, PIN_VALUE_HIGH);
//        Get_Channel_Value (channel);
////        Propagate_Channel_Value (channel);
//        
//      } else if ((*behavior).type == BEHAVIOR_TYPE_DELAY) {
//        Delay* delay = (Delay*) (*behavior).schema;
//        
//        // TO DEBUG: Uncomment the following to monitor the timer
//        // Serial.print ("Delay "); Serial.print ((*delay).milliseconds); Serial.print ("\n");
//        // Serial.print ("\tstartTime: "); Serial.print ((*delay).startTime); Serial.print ("\n");
//        // Serial.print ("\tcurrentTime: "); Serial.print ((*delay).currentTime); Serial.print ("\n");
//        
//        // Update timers
//        if ((*delay).startTime == 0) {
//          (*delay).startTime = millis ();
//          (*delay).currentTime = (*delay).startTime;
//        }
//          
//        // Update timer with current time
//        (*delay).currentTime = millis ();
//        
//        // Check if timer has expired
//        if ((*delay).currentTime - (*delay).startTime >= (*delay).milliseconds) {
//          // Reset timers
//          (*delay).startTime = 0L;
//          (*delay).currentTime = 0L;
//        } else {
//          sustainBehavior = true;
//        }
//        
//        // TODO: Call device-specific routine (retreived from cloud to change the device itself).
//      } else if ((*behavior).type == BEHAVIOR_TYPE_SOUND) {
//        Sound* sound = (Sound*) (*behavior).schema;
//        
//        Serial.print ("Sound "); Serial.print ((*sound).note); Serial.print ("\n");
//        Serial.print ("\tDuration: "); Serial.print ((*sound).duration); Serial.print ("\n");
//        
//        // TODO: Call device-specific routine (retreived from cloud to change the device itself).
//        //Play_Note (NOTE_C6, 250);
//        Play_Note ((*sound).note, (*sound).duration);
//        
//        
////        Channel* channel = Get_Channel (platform, (*input).pin);
//////        Update_Channel_Value (channel, PIN_VALUE_HIGH);
////        Get_Channel_Value (channel);
//////        Propagate_Channel_Value (channel);
//        
//      } else if ((*behavior).type == BEHAVIOR_TYPE_MOTION) {
//        Motion* motion = (Motion*) (*behavior).schema;
//        
//        Serial.print ("Motion "); Serial.print ("\n");
//        Serial.print ("\tEnd: "); Serial.print ((*motion).first); Serial.print ("\n");
//        Serial.print ("\tEnd: "); Serial.print ((*motion).end); Serial.print ("\n");
//        
//        // TODO: Call device-specific routine (retreived from cloud to change the device itself).
//        //Play_Note (NOTE_C6, 250);
//        //Sweep_Motion ((*motion).first, (*motion).end, (*motion).increment, (*motion).lenMicroSecondsOfPeriod, (*motion).lenMicroSecondsOfPulse);
//        Move_Motion ((*motion).position);
//        
//        
////        Channel* channel = Get_Channel (platform, (*input).pin);
//////        Update_Channel_Value (channel, PIN_VALUE_HIGH);
////        Get_Channel_Value (channel);
//////        Propagate_Channel_Value (channel);
//        
//      } else if ((*behavior).type == BEHAVIOR_TYPE_IMMEDIATE) {
//        
//        Immediate* immediate = (Immediate*) (*behavior).schema;
//        
////        Serial.print ("Language "); Serial.print ("\n");
////        Serial.print ("Size "); Serial.print ((*language).messageSize); Serial.print ("\n");
////        Serial.print ("Message "); Serial.print ((*language).message); Serial.print ("\n");
//        
//        // TODO: Call device-specific routine (retreived from cloud to change the device itself).
//        //Play_Note (NOTE_C6, 250);
////        Play_Note ((*sound).note, (*sound).duration);
//        Serial.print ("PERFORMING!!!: ");
//        Serial.println (String ((*immediate).message));
//        Perform_Immediate_Behavior (String ((*immediate).message));
//        
//        // Flag behavior for deletion now that it's been performed
//        deleteBehavior = false;
//        
//      } else if ((*behavior).type == BEHAVIOR_TYPE_NONE) {
//        
//        // TODO: Call device-specific routine (retreived from cloud to change the device itself).
//        
//      } else {
//        
//        // NOTE: The Behavior type is invalid or not supported.
//        // NOTE: To add additional Behaviors, append this if-else control flow.
//        
//      }
      
      // Continue behavior performance
      if (sustainBehavior == false) {
        // TODO: Get next behavior (following logic specific to loops, lines, dots).
        //Serial.print ("behavior's sequence type: "); Serial.print ((*(*behavior).sequence).type); Serial.print ("\n");
//        Serial.print ("behavior's sequence type: "); Serial.print ((*((*performer).origin)).type); Serial.print ("\n");
        // if ((*(*behavior).sequence).type == SEQUENCE_TYPE_LOOP) {
        if ((*((*performer).origin)).type == SEQUENCE_TYPE_LOOP) {
          
          // TODO: If the current behavior is a delay, only proceed if the delay period has passed, otherwise, remain on the delay behavior.
          
          Behavior* nextBehavior = (*((*performer).behavior)).next;
          
//          Serial.print ("current behavior: ");
//          Serial.println ((int) ((*performer).behavior));
//          Serial.print ("next behavior: ");
//          Serial.println ((int) (*((*performer).behavior)).next);
//          Serial.print ("origin: ");
//          Serial.println ((int) (*((*performer).origin)).behavior);
          
          if (nextBehavior != NULL) {
//            Serial.println("next behavior");
            // Go to next behavior in the sequence
            (*performer).behavior = (*((*performer).behavior)).next;
          } else {
//            Serial.println("restart sequence from first behavior");
            // The end of the looping sequence has been reached, so start again from the beginning of the performer's origin behavior sequence.
            (*performer).behavior = (*((*performer).origin)).behavior;
            
            // Check if the next behavior is the behavior just performed and is about to be deleted... if so, performer is trying to perform a loop that is (about to be) empty, so set to NULL
//            if (deleteBehavior) { // check if the behavior just performed is about to be deleted...
//              if ((*performer).behavior == behavior) { // ...if so, check if behavior being deleted is the behavior the performer will next try to perform...
//                (*performer).behavior = NULL; // ...if so, point the performer to "NULL" stopping it
//              }
//            }
          }
          performanceResult = true;
          
        } else {
          
          // TODO: Implement "next" code for Dot and Line (i.e., they are terminal, non-repeating, so only execute once)
          
          performanceResult = false; // Return false, indicating "no next/more behaviors"
          
        }
      }
      
      performanceResult = false;
    }
    
//    // Delete behavior if needed
//    if (deleteBehavior == true) {
//      
//        // Remove behavior from loop and free it from memory
//        Delete_Behavior (behavior);
//    }
    
    return performanceResult;
    
  }
  
  return false;
  
}

// Applies the platform-specific performance
boolean Perform_Behavior_On_Platform (Behavior* behavior) {
  
  boolean sustainBehavior = false;
  
  if (behavior != NULL) {
  
    if ((*behavior).type == BEHAVIOR_TYPE_OUTPUT) {
      Output* output = (Output*) (*behavior).schema;
      
      Serial.print ("Output "); Serial.print ((*output).pin); Serial.print ("\n");
      Serial.print ("\tdata: "); Serial.print ((*output).data); Serial.print ("\n");
      
      // TODO: Call device-specific routine (retreived from cloud to change the device itself).
      Channel* channel = Get_Channel (platform, (*output).pin);
      Set_Channel_Value (channel, (*output).data);
//        Get_Channel_Value (channel);
//        Propagate_Channel_Value (channel);
      
      // Update the pin's state
//        Update_Virtual_Pin ((*output).pin, (*output).signal, (*output).data);
    } else if ((*behavior).type == BEHAVIOR_TYPE_INPUT) {
      Input* input = (Input*) (*behavior).schema;
      
      Serial.print ("Input "); Serial.print ((*input).pin); Serial.print ("\n");
      
      // TODO: Call device-specific routine (retreived from cloud to change the device itself).
      Channel* channel = Get_Channel (platform, (*input).pin);
//        Update_Channel_Value (channel, PIN_VALUE_HIGH);
      Get_Channel_Value (channel);
//        Propagate_Channel_Value (channel);
      
    } else if ((*behavior).type == BEHAVIOR_TYPE_DELAY) {
      Delay* delay = (Delay*) (*behavior).schema;
      
      // TO DEBUG: Uncomment the following to monitor the timer
      // Serial.print ("Delay "); Serial.print ((*delay).milliseconds); Serial.print ("\n");
      // Serial.print ("\tstartTime: "); Serial.print ((*delay).startTime); Serial.print ("\n");
      // Serial.print ("\tcurrentTime: "); Serial.print ((*delay).currentTime); Serial.print ("\n");
      
      // Update timers
      if ((*delay).startTime == 0) {
        (*delay).startTime = millis ();
        (*delay).currentTime = (*delay).startTime;
      }
        
      // Update timer with current time
      (*delay).currentTime = millis ();
      
      // Check if timer has expired
      if ((*delay).currentTime - (*delay).startTime >= (*delay).milliseconds) {
        // Reset timers
        (*delay).startTime = 0L;
        (*delay).currentTime = 0L;
      } else {
        sustainBehavior = true;
      }
      
      // TODO: Call device-specific routine (retreived from cloud to change the device itself).
    } else if ((*behavior).type == BEHAVIOR_TYPE_SOUND) {
      Sound* sound = (Sound*) (*behavior).schema;
      
      Serial.print ("Sound "); Serial.print ((*sound).note); Serial.print ("\n");
      Serial.print ("\tDuration: "); Serial.print ((*sound).duration); Serial.print ("\n");
      
      // TODO: Call device-specific routine (retreived from cloud to change the device itself).
      //Play_Note (NOTE_C6, 250);
      Play_Note ((*sound).note, (*sound).duration);
      
      
//        Channel* channel = Get_Channel (platform, (*input).pin);
////        Update_Channel_Value (channel, PIN_VALUE_HIGH);
//        Get_Channel_Value (channel);
////        Propagate_Channel_Value (channel);
      
    } else if ((*behavior).type == BEHAVIOR_TYPE_MOTION) {
      Motion* motion = (Motion*) (*behavior).schema;
      
      Serial.print ("Motion "); Serial.print ("\n");
      Serial.print ("\tEnd: "); Serial.print ((*motion).first); Serial.print ("\n");
      Serial.print ("\tEnd: "); Serial.print ((*motion).end); Serial.print ("\n");
      
      // TODO: Call device-specific routine (retreived from cloud to change the device itself).
      //Play_Note (NOTE_C6, 250);
      //Sweep_Motion ((*motion).first, (*motion).end, (*motion).increment, (*motion).lenMicroSecondsOfPeriod, (*motion).lenMicroSecondsOfPulse);
      Move_Motion ((*motion).position);
      
      
//        Channel* channel = Get_Channel (platform, (*input).pin);
////        Update_Channel_Value (channel, PIN_VALUE_HIGH);
//        Get_Channel_Value (channel);
////        Propagate_Channel_Value (channel);
      
    } else if ((*behavior).type == BEHAVIOR_TYPE_IMMEDIATE) {
      
      Immediate* immediate = (Immediate*) (*behavior).schema;
      
//        Serial.print ("Language "); Serial.print ("\n");
//        Serial.print ("Size "); Serial.print ((*language).messageSize); Serial.print ("\n");
//        Serial.print ("Message "); Serial.print ((*language).message); Serial.print ("\n");
      
      // TODO: Call device-specific routine (retreived from cloud to change the device itself).
      //Play_Note (NOTE_C6, 250);
//        Play_Note ((*sound).note, (*sound).duration);
//      Serial.print ("PERFORMING!!!: ");
//      Serial.println (String ((*immediate).message));
      Process_Immediate_Message (String ((*immediate).message));
      
      // Flag behavior for deletion now that it's been performed
//      deleteBehavior = false;
      
    } else if ((*behavior).type == BEHAVIOR_TYPE_NONE) {
      
      // TODO: Call device-specific routine (retreived from cloud to change the device itself).
      
    } else {
      
      // NOTE: The Behavior type is invalid or not supported.
      // NOTE: To add additional Behaviors, append this if-else control flow.
      
    }
      
  }
  
  return sustainBehavior;
  
}

//!
//! Behavior Propagator
//!

#define PROPAGATION_SIZE 32

//! Transformation (i.e., Transformation) to propagate.
//!
//! TODO: Rename to Transformation
struct Transformation {
  // String data;
  char* data;
  int size;
  
  Transformation* previous;
  Transformation* next;
};

//! Propagator of behavior transformations.
//!
struct Propagator {
  Transformation* transformation;
};

//! Create behavior transformation propagator.
//!
Propagator* Create_Propagator () {
  
  Serial.println ("Create_Propagator");
  
  // Create context
  Propagator* propagator = (Propagator*) malloc (sizeof (Propagator));
  
  // Initialize sequence
  (*propagator).transformation = NULL;
  
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

//! Create transformation (i.e., a behavior transformation).
//!
Transformation* Create_Transformation (String data) {
  
  Serial.println ("Create_Transformation");
  Serial.print ("data = "); Serial.print (data); Serial.print ("\n");
  
  // Create context
  Transformation* transformation = (Transformation*) malloc (sizeof (Transformation));
  
  Serial.println ("Allocated Transformation.");
  
  // Initialize transformation
  (*transformation).size = PROPAGATION_SIZE;
  (*transformation).data = (char*) malloc (PROPAGATION_SIZE * sizeof (char));
  data.toCharArray ((*transformation).data, PROPAGATION_SIZE);
  (*transformation).previous = NULL;
  (*transformation).next = NULL;
  
  Serial.print ("Initialized Transformation "); Serial.print ((int) transformation); Serial.print ("\n");
  
  // Return sequence
  return transformation;
  
}

String Get_Transformation_Data (Transformation* transformation) {
  
  if (transformation != NULL) {
    
    return (*transformation).data;
    
  }
  
  return NULL;
  
}

//! Frees the Transformation from dynamic memory.
//!
boolean Delete_Transformation (Transformation* transformation) {
  
  Serial.println ("Delete_Transformation");
  
  if (transformation != NULL) {
    
    // Free the data payload
    if ((*transformation).data != NULL) {
      free ((*transformation).data);
    }
    
    // Free the transformation object
    free (transformation);
    
  }
  
}

//! Queue the transformation for transformation.
//!
Transformation* Queue_Transformation (Propagator* propagator, Transformation* transformation) {
  
  Serial.println ("Queue_Transformation");
  
  Serial.print ("\tpropagator: "); Serial.print ((int) propagator); Serial.print ("\n");
  Serial.print ("\ttransformation: "); Serial.print ((int) transformation); Serial.print ("\n");
  
  if ((*propagator).transformation == NULL) {
    Serial.println ("NULL");
    
    // Push to the top of the stack (as the first element)
    (*propagator).transformation = transformation;
    
    // Set up the forward and back links
    (*transformation).previous = NULL;
    (*transformation).next     = NULL;
    
  } else {
    Serial.println ("NOT NULL");
    
    // Get the transformation at the end of the queue
    Transformation* currentTransformation = (*propagator).transformation;
    while ((*currentTransformation).previous != NULL) {
      Serial.print ("\tprevious");
      currentTransformation = (*currentTransformation).previous;
    }
    
    Serial.println ("\tfinished loop");
    
    // Push to the top of the stack
    (*currentTransformation).previous = transformation;
    
    // Set up the backward and forward links
    (*transformation).previous = NULL;
    (*transformation).next = currentTransformation;
    
    Serial.println ("finished scope");
    
  }
  
  // Return sequence
  return transformation;
  
}

//! Pop transformation off the propagator's stack
//!
Transformation* Dequeue_Transformation (Propagator* propagator) {
  
  Serial.println ("Dequeue_Transformation");
  
  Transformation* transformation = NULL;
  
  if (propagator != NULL) {
    
    // Get the transformation at the front of the propagator's queue
    transformation = (*propagator).transformation;
    
    // Update the Propagator. Set the next of the propagator's transformation queue.
    (*propagator).transformation = (*transformation).previous;
    
    // Dissociate the dequeued transformation. Update the backward and forward links of the dequeued transformation.
    (*transformation).next = NULL;
    (*transformation).previous = NULL;
    
  }
  
  return transformation;
  
}

#define NO_CHANNEL 0
#define I2C_CHANNEL 1
// TODO: #define VISUAL_CHANNEL 2
// TODO: #define AURAL_CHANNEL 2
// TODO: #define TEXTUAL_CHANNEL 3

//! Propagate the next message in the queue on the specified channel.
//!
boolean Propagate (Propagator* propagator, int channel) {
  
  Serial.println ("Propagate");
  
  if (propagator != NULL) {
    
    if (channel == I2C_CHANNEL) {
      
      // "(create input behavior 38472934)" // Pass on the parentheticized "secret"
      
      // Create buffer for storing the bytes to be sent (over I2C)
      const int AVAILABLE_BUFFER_BYTES = 32;
      char buffer[AVAILABLE_BUFFER_BYTES];
      
      // Dequeue the next description to be sent by the specified propagator
      Transformation* transformation = Dequeue_Transformation (propagator);
      
      // TODO: Break up dequeued string to be sent into 32 byte segments, then queue them in the I2C outgoing data buffer.

      // Transmit data over via the I2C protocol
      Wire.write ("("); // Start transformation description
      Wire.write ((*transformation).data); // Write the serialized data
      Wire.write (")"); // Conclude transformation description
      
      // Free the transformation from memory (once sent via I2C)
      Delete_Transformation (transformation);
      
      return true;
      
    }
    
  }
  
  // Return false because a valid propagator or channel was not specified.
  return false;
  
}



//!
//! Miscellaneous
//!

//struct TransformationBuffer {
//  
//};

struct Behavior_Transformation {
  int type;
  void* behavior; // i.e., Context, Behavior, Sequence
};

#endif
