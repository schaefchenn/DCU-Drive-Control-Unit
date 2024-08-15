#include <ESP32Servo.h> // Servo library for ESP32

#define steeringPin 25     // Pin for steering servo
#define motorPin    26     // Pin for motor servo

const uint8_t steeringOffset = 30; // Steering offset to prevent servo damage
const uint8_t centerSteeringAngle = 90; // Center angle for steering (to account for joystick drift)
const uint8_t centerSteeringTolerance = 3; // Tolerance for centering the steering (to account for joystick drift)

Servo absimaServo; // Servo object for steering
Servo absimaMotor; // Servo object for motor control


//==================================================================================/

void setupMANEUVER () {
    // Steering Servo setup
    absimaServo.attach(steeringPin);
    Serial.println("Steering Ready");

    // Motor setup - the Absima motor controller allows the motor to be treated as a servo
    absimaMotor.attach(motorPin);
    absimaMotor.writeMicroseconds(1500); // Neutral position for the motor
    Serial.println("Motor Ready");
}


//==================================================================================/

void maneuver(int16_t throttle, uint8_t steeringAngle){
    // Center steering angle if within tolerance

    if (abs(steeringAngle - centerSteeringAngle) <= centerSteeringTolerance) {
        steeringAngle = centerSteeringAngle;
    }

    absimaServo.write(steeringAngle); // Set servo to steering angle
    absimaMotor.writeMicroseconds(throttle); // Set motor throttle
}