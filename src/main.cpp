#include <Arduino.h>
#define MODEM_TX 27
#define MODEM_RX 26

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to the module)
#define SerialAT Serial1
#define Vsim800 23 // питание sim800

bool DEBUG = true;

String sendATCommand11(String cmd, const int timeout, boolean debug)
{
  String response = "";
  SerialAT.println(cmd);
  long int time = millis();
  while ((time + timeout) > millis())
  {
    while (SerialAT.available())
    {
      char c = SerialAT.read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.println(response);
  }
  return response;
}

int8_t sendATCommand(char *ATcommand, char *expected_answer1, unsigned int timeout, boolean debug)
{

  uint8_t x = 0, answer = 0;
  char response[200];
  unsigned long previous;
  // memset(response, '\0', 200);    // Initialize the string
  delay(100);
  while (SerialAT.available() > 0)
    SerialAT.read();           // Clean the input buffer
  SerialAT.println(ATcommand); // Send the AT command
  x = 0;
  previous = millis();
  // this loop waits for the answer
  do
  {
    if (SerialAT.available() != 0)
    {
      response[x] = SerialAT.read();
      //Serial.write(response[x]);
      //Serial.println(x);
      x++;
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer1) != NULL)
      {
        answer = 1;
      }
      if (x > 180)
        x = 0;
    }
    // Waits for the asnwer with time out
  } while ((answer == 0) && ((millis() - previous) < timeout));
  if (debug)
  {
    Serial.println(response);
  }

  return answer;
}

void setup()
{
  pinMode(Vsim800, OUTPUT);
  digitalWrite(Vsim800, HIGH);
  // Set console baud rate
  SerialMon.begin(115200);

  // Set GSM module baud rate and UART pins
  SerialAT.begin(19200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  sendATCommand("AT+CIPSHUT", "OK", 2000, DEBUG);
 // sendATCommand("AT+CFUN=1", "OK", 2000, DEBUG);                                    // Enable full functionality
  sendATCommand("AT+CIPMUX=0", "OK", 1000, DEBUG);                                  // Set single connection mode
  sendATCommand("AT+CGATT=1", "OK", 1000, DEBUG);                                   // Attach to GPRS service
  sendATCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", "OK", 1000, DEBUG);            // Set connection type to GPRS
  sendATCommand("AT+SAPBR=3,1,\"APN\",\"internet.beeline.ru\"", "OK", 1000, DEBUG); // Set APN
  sendATCommand("AT+SAPBR=3,1,\"USER\",\"beeline\"", "OK", 1000, DEBUG);
  sendATCommand("AT+SAPBR=3,1,\"PWD\",\"beeline\"", "OK", 1000, DEBUG);
  sendATCommand("AT+SAPBR=1,1", "OK", 1000, DEBUG); // Enable GPRS
  sendATCommand("AT+SAPBR=2,1", "OK", 1000, DEBUG); // Get IP address
  sendATCommand("AT+CDNSCFG=\"8.8.8.8\",\"8.8.4.4\"", "OK", 1000, DEBUG);
}
void sendData()
{
  sendATCommand("AT+CIPSTART=\"TCP\",\"narodmon.ru\",8283", "OK", 1000, DEBUG);
  sendATCommand("AT+CIPSEND", ">", 1000, DEBUG);
  String kk = "#ESP323481B2519140\n";
  kk += "#RSSI#10\n##";
  SerialAT.print(kk);
  SerialAT.write(byte(0x1A));
}
void loop()
{
  sendData();
  delay(20000); // delay 20 seconds
}
