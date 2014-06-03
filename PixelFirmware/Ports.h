#ifndef PORTS_H
#define PORTS_H

#define MODULE_OUTPUT_PORT 6

boolean setupPorts() {
  // Set up pin mode for I/O
  pinMode(MODULE_OUTPUT_PORT, OUTPUT); // pinMode(13, OUTPUT);
}

#endif
