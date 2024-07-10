#include <Wire.h> 
#include <Adafruit_MPR121.h> 
#include <Servo.h> 

#ifndef _BV
#define _BV(bit) (1 << (bit)) // Define a macro for bitwise operations
#endif

Adafruit_MPR121 cap = Adafruit_MPR121(); 

Servo servo_A, servo_B; 
int servo_socket_A = 12; 
int servo_socket_B = 11; 
int delayTime = 5; // Delay time to control speed; the higher the value, the slower the speed
bool actionInProgress = false; // Flag to indicate if an action is currently in progress
unsigned long lastActionTime = 0; // Timestamp of the last completed action
int currentSensor = -1;

void setup() {
  Serial.begin(9600); 
  servo_A.attach(servo_socket_A);
  servo_B.attach(servo_socket_B); 

  if (!cap.begin(0x5A)) { // Initialize the MPR121 sensor with address 0x5A
    Serial.println("MPR121 not found, check wiring?"); // If sensor is not found, print error message
    while (1); // Infinite loop to stop program execution
  }
  Serial.println("MPR121 found!"); // If sensor is found, print success message
}

void loop() {
  uint16_t touched = cap.touched(); // Read the touch status from the capacitive touch sensor

  for (uint8_t i = 0; i < 8; i++) { // Check touch electrodes 0 to 7
    if (touched & _BV(i)) { // Check if a specific touch electrode is touched
      if (actionInProgress && currentSensor != i) { // If an action is in progress and the current touch is different from the ongoing action
        actionInProgress = false; // Stop the current action
      }
      currentSensor = i; // Update the current sensor being processed
      executeAction(i); // Execute the corresponding action
      lastActionTime = millis(); // Update the timestamp of the last action
      return; // Respond to only one touch at a time
    }
  }

  if (actionInProgress && millis() - lastActionTime > 3000) { // If no new input is received 3 seconds after the action is completed
    moveServos(90, 90); // Return to the initial position
    actionInProgress = false; // End the action
  }
}

void executeAction(int sensor) {
  actionInProgress = true; // Mark the start of the action
  // Execute the corresponding action based on the touched sensor
  switch(sensor) {
    case 0: // North direction action
      Serial.println("North touched");
      moveServos(70, 180);
      break;
    case 1: // East direction action
      Serial.println("East touched");
      moveServos(0, 70);
      break;
    case 2: // South direction action
      Serial.println("South touched");
      moveServos(130, 0);
      break;
    case 3: // West direction action
      Serial.println("West touched");
      moveServos(180, 120);
      break;
    case 4: // Northeast direction action
      Serial.println("Northeast touched");
      moveServos(0, 116); // northeast coordinates
      break;
    case 5: // Southeast direction action
      Serial.println("Southeast touched");
      moveServos(65, 0); // southeast coordinates
      break;
    case 6: // Northwest direction action
      Serial.println("Northwest touched");
      moveServos(110, 180); // northwest coordinates
      break;
    case 7: // Southwest direction action
      Serial.println("Southwest touched");
      moveServos(180, 70); // southwest coordinates
      break;
  }
}

void moveServos(int targetPositionA, int targetPositionB) {
  int currentPositionA = servo_A.read(); // Read the current position of servo A
  int currentPositionB = servo_B.read(); // Read the current position of servo B

  int deltaA = abs(targetPositionA - currentPositionA); // Calculate the angle that servo A needs to move
  int deltaB = abs(targetPositionB - currentPositionB); // Calculate the angle that servo B needs to move
  int maxDelta = max(deltaA, deltaB); // Determine the maximum angle to move

  for (int i = 0; i <= maxDelta; i++) {
    int stepA = currentPositionA + i * (targetPositionA - currentPositionA) / maxDelta;
    int stepB = currentPositionB + i * (targetPositionB - currentPositionB) / maxDelta;
    servo_A.write(stepA);
    servo_B.write(stepB);
    delay(delayTime); // Delay to control movement speed
    if (!actionInProgress) break; 
  }
}


