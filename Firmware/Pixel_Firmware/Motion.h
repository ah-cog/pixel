#ifndef MOTION_H
#define MOTION_H

#define SERVO_PIN 5 // i.e., the output port pin

//pinMode (SERVO_PIN, OUTPUT);

// TODO: Move this procedure into the cloud and make it available for automatically downloading if used in Looper. Once downloaded, it's cached until it's no longer needed or there's no longer space available, however, it is requested again if it's removed from cache to make room for another remove procedure.
boolean Sweep_Motion (int first, int end, int increment, int lenMicroSecondsOfPeriod, int lenMicroSecondsOfPulse) {
  
    int current = 0;
    for(current = first; current <end; current+=increment){
         // Servos work by sending a 25 ms pulse.  
         // 0.7 ms at the start of the pulse will turn the servo to the 0 degree position
         // 2.2 ms at the start of the pulse will turn the servo to the 90 degree position 
         // 3.7 ms at the start of the pulse will turn the servo to the 180 degree position 
         // Turn voltage high to start the period and pulse
         digitalWrite(SERVO_PIN, HIGH);

         // Delay for the length of the pulse
         delayMicroseconds(current);

         // Turn the voltage low for the remainder of the pulse
         digitalWrite(SERVO_PIN, LOW);

         // Delay this loop for the remainder of the period so we don't
         // send the next signal too soon or too late
         delayMicroseconds(lenMicroSecondsOfPeriod - current); 
    }
    for(current = end; current >first; current-=increment){
         // Servos work by sending a 20 ms pulse.
         // 0.7 ms at the start of the pulse will turn the servo to the 0 degree position
         // 2.2 ms at the start of the pulse will turn the servo to the 90 degree position
         // 3.7 ms at the start of the pulse will turn the servo to the 180 degree position
         // Turn voltage high to start the period and pulse
         digitalWrite(SERVO_PIN, HIGH);

         // Delay for the length of the pulse
         delayMicroseconds(current);

         // Turn the voltage low for the remainder of the pulse
         digitalWrite(SERVO_PIN, LOW);

         // Delay this loop for the remainder of the period so we don't
         // send the next signal too soon or too late
         delayMicroseconds(lenMicroSecondsOfPeriod - current);
    }

}

boolean Move_Motion (int position) {
  
  pinMode(SERVO_PIN, OUTPUT);
  
  int lenMicroSecondsOfPeriod = 25 * 1000; // 25 milliseconds (ms)
  int lenMicroSecondsOfPulse = 1 * 1000; // 1 ms is 0 degrees
  
//  int position = 0.5 * 1000;
  
   // Servos work by sending a 25 ms pulse.  
   // 0.7 ms at the start of the pulse will turn the servo to the 0 degree position
   // 2.2 ms at the start of the pulse will turn the servo to the 90 degree position 
   // 3.7 ms at the start of the pulse will turn the servo to the 180 degree position 
   // Turn voltage high to start the period and pulse
   digitalWrite(SERVO_PIN, HIGH);

   // Delay for the length of the pulse
   delayMicroseconds(position);

   // Turn the voltage low for the remainder of the pulse
   digitalWrite(SERVO_PIN, LOW);

   // Delay this loop for the remainder of the period so we don't
   // send the next signal too soon or too late
   delayMicroseconds(lenMicroSecondsOfPeriod - position); 

}

#endif
