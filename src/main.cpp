#include <Arduino.h>
#include <CANBUS.h>
#include <XBOX.h>

// Core definitions (assuming you have dual-core ESP32)
static const BaseType_t pro_cpu = 0; // protocol core
static const BaseType_t app_cpu = 1; // application core

// Initialize CPU cores
TaskHandle_t Task1;
TaskHandle_t Task2;

// Set CAN ID
#define CANBUS_ID 0x12    // put your CAN ID here

// CAN send values
int8_t msg1;
int16_t throttle;
int8_t msg3;

// CAN recieve values
int CANthrottle;

uint driveMode = 1; // 1 = XBOX Controller; 0 = CANBUS Drive Input


//==================================================================================/

void CANBUS (void * pvParameters) {
  while (1){
    CANBUS_recv recvMSG = canReceiver();

    driveMode = recvMSG.driveMode;
    CANthrottle = recvMSG.throttleValue;

    // yield
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}


//==================================================================================//

void ECU (void * pvParameters){
  while(1){
    switch (driveMode){
      case 0:
        throttle = CANthrottle;

      case 1:
        XBOX xboxData = getXboxData();
        throttle = map(xboxData.rightTrigger - xboxData.leftTrigger, -1023, 1023, 1000, 2000);
        canSender(CANBUS_ID, 1, throttle, 0);
        Serial.println(throttle);
    }

    // yield
    vTaskDelay(12 / portTICK_PERIOD_MS);
  }
}


//==================================================================================//

void setup() {
  // Initialize serial communication at 115200 baud rate
  Serial.begin(115200);
  while (!Serial);

  // Setup CAN communication and ECU Components
  setupCANBUS();
  setupXBOX();


  // Wait a moment to start (so we don't miss Serial output)
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  // Start CANcommunication (priority set to 1, 0 is the lowest priority)
  xTaskCreatePinnedToCore(CANBUS,                                  // Function to be called
                          "Controller Area Network Message Recieving",  // Name of task
                          4096,                                         // Increased stack size
                          NULL,                                         // Parameter to pass to function
                          2,                                            // Increased priority
                          NULL,                                         // Task handle
                          pro_cpu);                                     // Assign to protocol core

  // Start CANcommunication (priority set to 1, 0 is the lowest priority)
  xTaskCreatePinnedToCore(ECU,                                          // Function to be called
                          "Electromic Controll Unit Functionality",     // Name of task
                          8192,                                         // Increased stack size
                          NULL,                                         // Parameter to pass to function
                          2,                                            // Increased priority
                          NULL,                                         // Task handle
                          app_cpu);                                     // Assign to protocol core  
}

void loop() {
  // put your main code here, to run repeatedly:
}