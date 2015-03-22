#ifndef MEMORY_H
#define MEMORY_H

// TODO: Update this to store blobs that can be keys associated with any number (including zero) other blobs. The memory is a list of "memory nodes" that point to other memories, creating a (directed?) graph.
// TODO: Add function to check if has memory
// TODO: Add memory "categories" (e.g., for the modules that have focus) and relations/properties (e.g., "module M [has] focus")
//       - accessible from both firmware and higher level (or maybe not firmware?)
// (TODO: in Language.h, add "give focus" and "has focus" commands)

struct Memory;

Memory* Get_Memory (char* trigger);

struct Memory {
  char* trigger;
  int triggerSize;
  // TODO: type? or make a function to allow dynamic typing? probably dynamic :)
  
  char* content;
  int contentSize;
  // TODO: type? or make a function to allow dynamic typing? probably dynamic :)
  
  Memory* previous;
  Memory* next;
};

Memory* memories = NULL; // incoming memory queue

//! Create memory (i.e., a behavior transformation).
//!
Memory* Create_Memory (String trigger, String content) { // (trigger, content)
  
  Serial.println ("Create_Memory");
  
  // Create substrate
  Memory* memory = (Memory*) malloc (sizeof (Memory));
  
  // Initialize memory
  (*memory).triggerSize = trigger.length (); // NOTE: Add 1 to the length to account for the '\0' character.
  (*memory).trigger = (char*) malloc (((*memory).triggerSize + 1) * sizeof (char));
  trigger.toCharArray ((*memory).trigger, ((*memory).triggerSize + 1));
  
  (*memory).contentSize = content.length (); // NOTE: Add 1 to the length to account for the '\0' character.
  (*memory).content = (char*) malloc (((*memory).contentSize + 1) * sizeof (char));
  content.toCharArray ((*memory).content, ((*memory).contentSize + 1));
  
  (*memory).previous = NULL;
  (*memory).next = NULL;
  
  // Return sequence
  return memory;
  
}

//! Updates memory if it exists and creates it if it does not exist.
//!
Memory* Update_Memory (String trigger, String content) { // (trigger, content)
  
  Serial.println ("Update_Memory");
  
  // Check if memory already exists ("memory" will be "NULL" if so)
  char triggerBuffer[32]; // TODO: Update parameters to be char* rather than String for portability
  trigger.toCharArray (triggerBuffer, 32); // TODO: Update parameters to be char* rather than String for portability
  Memory* memory = Get_Memory (triggerBuffer);
  
  if (memory != NULL) { // Memory exists...
  
    // Free the memory content so it can be replaced...
    if ((*memory).content != NULL) {
      free ((*memory).content);
      (*memory).contentSize = 0;
    }
    
    // Allocate memory for new memory content and assign it to the memory...
    (*memory).contentSize = content.length (); // NOTE: Add 1 to the length to account for the '\0' character.
    (*memory).content = (char*) malloc (((*memory).contentSize + 1) * sizeof (char));
    content.toCharArray ((*memory).content, ((*memory).contentSize + 1));
  
  } else { // Memory doesn't exist...
  
    memory = Create_Memory (trigger, content);
    
  }
  
  // Return sequence
  return memory;
  
}

//! Frees the memory from dynamic memory.
//!
boolean Delete_Memory (Memory* memory) {
  
  Serial.println ("Delete_Memory");
  
  if (memory != NULL) {
    
    // Free the memory trigger
    if ((*memory).trigger != NULL) {
      free ((*memory).trigger);
    }
    
    // Free the memory content
    if ((*memory).content != NULL) {
      free ((*memory).content);
    }
    
    // Free the memory object
    free (memory);
    
  }
  
}

//! Queue the outgoing memory.
//!
Memory* Append_Memory (Memory* memory) {
  
  Serial.println ("Append_Memory");
  
  if (memories == NULL) {
    
    // Push to the top of the stack (as the first element)
    memories = memory;
    
    // Set up the forward and back links
    (*memory).previous = NULL;
    (*memory).next     = NULL;
    
  } else {
    
    // Get the last memory in the queue
    Memory* lastMemory = memories;
    while ((*lastMemory).next != NULL) {
      Serial.print ("\tnext");
      lastMemory = (*lastMemory).next;
    }
    
    // Push to the last position in the queue
    // i.e., add to [ m0, m1, ..., mn ] at m(n + 1) as in [ m0, m1, ..., mn, m(n + 1) ]
    (*lastMemory).next = memory;
    
    // Set up the backward and forward queue links
    (*memory).previous = lastMemory;
    (*memory).next = NULL;
    
  }
  
  // Return memory
  return memory;
  
}

//! Dequeue the next outgoing memory.
//!
Memory* Remove_Memory (char* trigger) {
  
  Serial.println ("Remove_Memory");
  
  Memory* memory = NULL;
  
  // Get the last memory in the queue
  Memory* currentMemory = memories;
  while (currentMemory != NULL) {
    
    if (strncmp ((*currentMemory).trigger, trigger, (*currentMemory).triggerSize) == 0) {
      memory = currentMemory;
      break;
    }
    
    currentMemory = (*currentMemory).next;
  }
  
  if (memory != NULL) {
    
    // Get the transformation at the front of the propagator's queue
    Memory* previous = (*memory).previous;
    Memory* next = (*memory).next;
    
    // Update connections
    if (previous != NULL) {
      (*previous).next = next;
    }
    if (next != NULL) {
      (*next).previous = previous;
    }
    
    // Dissociate the dequeued memory. Update the backward and forward links of the dequeued memory.
    (*memory).next = NULL;
    (*memory).previous = NULL;
    
  }
  
  return memory;
  
}

//! Dequeue the next outgoing memory.
//!
Memory* Get_Memory (char* trigger) {
  
  Serial.println ("Get_Memory");
  
  Memory* memory = NULL;
  
  // Get the last memory in the queue
  Memory* currentMemory = memories;
  while (currentMemory != NULL) {
    
    if (strncmp ((*currentMemory).trigger, trigger, (*currentMemory).triggerSize) == 0) {
      memory = currentMemory;
      break;
    }
    
    currentMemory = (*currentMemory).next;
  }
  
  return memory;
  
}

//! Dequeue the next outgoing memory.
//!
void Print_Memory () {
  
//  Serial.println ("Print_Memory");
  
  Memory* memory = memories;
  
  // Get the last memory in the queue
  while (memory != NULL) {
    
    Serial.print ((*memory).trigger);
    Serial.print (" -> ");
    Serial.print ((*memory).content);
    Serial.print ("\n");
    
    memory = (*memory).next;
  }
  
}

#endif
