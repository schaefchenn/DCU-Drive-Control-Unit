#include <CAN.h>

#define TX_GPIO_NUM   17  // Connects to CTX
#define RX_GPIO_NUM   16  // Connects to CRX

struct CANBUS_recv {
  bool recieved;
  bool extended;
  bool rtr;
  uint32_t id;
  int8_t requestedLength;
  int8_t length;
  int8_t driveMode;
  int16_t throttleValue;
  int8_t steeringAngle;
  uint8_t acknowledged;
};


//==================================================================================//

void setupCANBUS() {
  Serial.println ("CAN Receiver/Receiver");

  // Set the pins
  CAN.setPins (RX_GPIO_NUM, TX_GPIO_NUM);

  // start the CAN bus at 500 kbps
  if (!CAN.begin (1E6)) {
    Serial.println ("Starting CAN failed!");
    while (1);
  }
  else {
    Serial.println ("CAN Initialized");
  }
}


//==================================================================================//

void canSender(int CANBUS_ID, int8_t value1, int16_t value2, int8_t value3) {
  // Serial.print("Sending packet ... ");

  CAN.beginPacket(CANBUS_ID);  // Sets the ID and clears the transmit buffer

  CAN.write(value1); // Write 1 byte

  // Break val2 into two bytes and write them
  CAN.write((uint8_t)(value2 >> 8)); // High byte
  CAN.write((uint8_t)(value2 & 0xFF)); // Low byte

  CAN.write(value3); // Write 1 byte

  CAN.endPacket();

  // Serial.println("done");
}

CANBUS_recv canReceiver() {
  CANBUS_recv recvMSG;  // Create an instance of the struct to hold the return values

  int packetSize = CAN.parsePacket();

  if (packetSize) {
    recvMSG.recieved = true;

    if (CAN.packetExtended()) {
      recvMSG.extended = true;
    }

    if (CAN.packetRtr()) {
      recvMSG.rtr = true;
    }

    recvMSG.id = CAN.packetId(), HEX;

    if (CAN.packetRtr()) {
      recvMSG.requestedLength = CAN.packetDlc();
    } else {
      recvMSG.length = packetSize;

      // Read and print integer values
      if (packetSize >= 4) { // Ensure we have at least 4 bytes
        recvMSG.driveMode = CAN.read(); // Read 8-bit signed integer

        // Read the next two bytes and combine them into a int16_t
        uint8_t highByte = CAN.read();
        uint8_t lowByte = CAN.read();
        int16_t val2 = (highByte << 8) | lowByte; // Combine bytes
        // Interpret as signed integer
        if (val2 & 0x8000) { // Check if the sign bit is set
          val2 |= 0xFFFF0000; // Sign-extend to 32-bit
        }
        recvMSG.throttleValue = val2;

        recvMSG.acknowledged = CAN.read(); // Read 8-bit signed integer
      }
    }
    
    return recvMSG;
  }
}