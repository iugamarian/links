// Original program with schematic diagram: https://projecthub.arduino.cc/jackaless/parking-sensor-4aa9d0
// Modified program for my own taste with noise filtering:
// To Upload to Arduino on Ubuntu:
// sudo usermod -a -G dialout $USER
// sudo snap install arduino
// reboot (for the GUI to update the user groups)
// https://snapcraft.io/install/arduino/ubuntu#install
/**************************************************************
 *  PARKING SENSOR WITH HC-SR04                               *
 *                                                            *
  *  This code receives data from the HC-SR04 proximity        *
 *  sensor, analyses  them, sends them to the serial monitor   *
 *  and produces intermittent sounds  to warn of an obstacle.  *
 **************************************************************/

//  Definition of trigger, echo, beep pins and other constants
#define trigger      2
#define  echo         3
#define beep        11
#define beep_start 60
#define min_distance  20

// Definition of sound speed (centimetres / microsecond)
#define c 0.0343

//  Definition of the variables
long tempo;
long approachdelay;
float pastspaceone = 100;
float pastspacetwo = 100;
float presentspace;
float space;

void setup() {
  // Definition of input and output
  pinMode(trigger, OUTPUT);
  pinMode(echo,  INPUT);
  pinMode(beep, OUTPUT);

  // Serial communication initialisation  (optional)
  Serial.begin(9600);
}

void loop() {
  // Before measurement,  the trigger is set to low level
  digitalWrite(trigger, LOW);
  delayMicroseconds(5);

  // Send one pulse (trigger goes high level for 10 microseconds)
  digitalWrite(trigger,  HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  //  Reading echo, via pulseIn, which returns the duration of the impuse (in microseconds)
  // The acquired data is then divided by 2 (forward and backward)
  tempo =  pulseIn(echo, HIGH) / 2;
  // Computation of distance in centimetres
  presentspace  = tempo * c;
  space = ((pastspaceone + pastspacetwo + presentspace) / 3);
  pastspaceone = pastspacetwo;
  pastspacetwo = presentspace;
 
  // space is displayed in the serial monitor ([Ctrl] + [Shift]  + M)
  // approximated to the first decimal place
  Serial.println("Distance = " + String(space, 1) + " cm");

  // If the distance is less than one  metre
  if (space < beep_start) { 
    // Emits sounds at intervals proportional  to distance (1 m = 400 ms)
    tone(beep, 700); 
    delay(20);
    //  Below min_distance cm it emits a continuous sound
    if (space > min_distance)  {
      noTone(beep); 
      approachdelay = ((((space * space) / 10) + 30) / 2);
      delay(approachdelay);
    }
  } 

  else {
  // Waits  300 milliseconds before another measurement when no sound is emmited
  delay(200);
  }
}
