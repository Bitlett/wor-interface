#include <Servo.h>

Servo myServo;  // Create a Servo object to control the servo

int servoPin = 9;  // Pin to which the servo is connected
int currentPos = 90;  // Store the current position of the servo (initially 90 degrees)
int targetPos = 90;  // Store the target position
int duration = 0;  // Store the duration for the movement
unsigned long lastUpdateTime = 0;  // Keep track of the last time the servo was updated
int stepDelay = 0;  // Time between each step in milliseconds
bool moving = false;  // Flag to indicate if the servo is currently moving

void setup() {
  Serial.begin(9600);   // Initialize serial communication at 9600 baud rate
  myServo.attach(servoPin);  // Attach the servo to the specified pin
  myServo.write(currentPos);  // Move servo to the initial position (90 degrees)
  Serial.println("Enter position (0-180) or 'position: X duration: Y' (e.g., 'position: 10 duration: 3'):");
}

void loop() {
  // Check if there is serial input available
  if (Serial.available() > 0) {
    // Read the input as a string
    String input = Serial.readStringUntil('\n');
    input.trim();  // Remove any extra whitespace/newlines

    // Check if input contains "position:" and "duration:"
    if (input.indexOf("position:") != -1 && input.indexOf("duration:") != -1) {
      // Parse the position and duration from the input string
      int posIndex = input.indexOf("position:") + 9;
      int durIndex = input.indexOf("duration:") + 9;
      targetPos = input.substring(posIndex, input.indexOf(" ", posIndex)).toInt();
      duration = input.substring(durIndex).toInt();

      // Check if the target position is within the valid range (0-180 degrees)
      if (targetPos >= 0 && targetPos <= 180 && duration > 0) {
        Serial.print("Moving to position: ");
        Serial.print(targetPos);
        Serial.print(" over ");
        Serial.print(duration);
        Serial.println(" seconds.");

        // Set up the movement
        stepDelay = (duration * 1000) / abs(targetPos - currentPos);  // Calculate the step delay
        moving = true;  // Start moving the servo
        lastUpdateTime = millis();  // Record the current time
      } else {
        Serial.println("Invalid input. Position must be between 0-180, and duration must be positive.");
      }
    } 
    // If no duration is specified, treat it as a simple position command
    else {
      targetPos = input.toInt();
      // Check if the position is within the valid range (0-180 degrees)
      if (targetPos >= 0 && targetPos <= 180) {
        Serial.print("Moving quickly to position: ");
        Serial.println(targetPos);
        myServo.write(targetPos);  // Move the servo to the specified position quickly
        currentPos = targetPos;  // Update the current position
        moving = false;  // Stop any ongoing smooth movement
      } else {
        Serial.println("Invalid position. Please enter a value between 0 and 180.");
      }
    }
  }

  // Check if we are in the process of moving the servo
  if (moving && millis() - lastUpdateTime >= stepDelay) {
    // Move the servo one step closer to the target position
    if (currentPos < targetPos) {
      currentPos++;
    } else if (currentPos > targetPos) {
      currentPos--;
    }

    myServo.write(currentPos);  // Update the servo position
    lastUpdateTime = millis();  // Update the time of the last movement

    // Check if we've reached the target position
    if (currentPos == targetPos) {
      moving = false;  // Stop the movement
      Serial.println("Movement complete.");
    }
  }
}