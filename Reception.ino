#include <VirtualWire.h>
#include <arduinoFFT.h>
#define RF_RX_PIN 0 // Digital pin connected to the RF receiver module
float zms1;
int c=0; //to count the number of samples
void setup() {
  Serial.begin(600);
  vw_set_rx_pin(RF_RX_PIN);
  vw_setup(200); // Set the reception rate in bits per second
  vw_set_ptt_inverted(true); // Required for RF module
  vw_rx_start(); // Start the receiver

}

void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
 
 //microseconds = micros();
  if (vw_get_message(buf, &buflen)) {
    c++;
    delay(1000);

    char zmsStr[buflen + 1];
    strncpy(zmsStr, (char*)buf, buflen);
    zmsStr[buflen] = '\0';

    int zms = atoi(zmsStr); // Convert the received string back to an integer
    Serial.println("sample num: ");
    Serial.println(c);
    Serial.println("Received frequency: ");
    Serial.print(zms);
    Serial.println(" Hz");
  }     
}

    
  
