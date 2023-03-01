#include <Arduino.h>
#define MODEM_TX 27
#define MODEM_RX 26

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to the module)
#define SerialAT Serial1

void setup()
{
  // Set console baud rate
  SerialMon.begin(115200);

  // Set GSM module baud rate and UART pins
  SerialAT.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);
  
}

void loop()
{
  while (SerialAT.available())
  {
    SerialMon.write(SerialAT.read());
  }
  while (SerialMon.available())
  {
    SerialAT.write(SerialMon.read());
  }
}
