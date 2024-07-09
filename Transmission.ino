#include <TinyWireM.h>  // TinyWireM library - used for I2C communication
#include <arduinoFFT.h>
#include<VirtualWire.h>
arduinoFFT FFT = arduinoFFT();
#define RF_TX_PIN 1 
#define SAMPLES 16
#define SAMPLING_FREQUENCY 128
int ADXL345 = 0x53; // The ADXL345 sensor I2C address

float Z_out, zg, zms;//Y_out, Z_out;  // Outputsunsigned int sampling_period_us;
unsigned int sampling_period_us;
int max; //to store resonance frequency
double maxR; //to store max ampiltude
double freq[SAMPLES];
double vReal[SAMPLES];
double vImag[SAMPLES];

void setup() {
 sampling_period_us = round(1000 * (1.0 / SAMPLING_FREQUENCY));
  TinyWireM.begin(); // Initiate the TinyWireM library
  // Set ADXL345 in measuring mode
  TinyWireM.beginTransmission(ADXL345); // Start communicating with the device
  TinyWireM.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  TinyWireM.write(8); // Bit D3 High for measuring enable (8dec -> 0000 1000 binary)
  TinyWireM.endTransmission();
  delay(10);

  //calibration Z-axis to set 1g=256 digital counts
  TinyWireM.beginTransmission(ADXL345);
  TinyWireM.write(0x20);
  TinyWireM.write(5);
  TinyWireM.endTransmission();
  delay(10);

  pinMode(RF_TX_PIN,OUTPUT);
  vw_set_tx_pin(RF_TX_PIN);
  vw_setup(200);  // Set the transmission rate in bits per second
  vw_set_ptt_inverted(true); // Required for RF module

}

void loop() {
  for (int i = 0; i < SAMPLES; i++)
    {
  TinyWireM.beginTransmission(ADXL345);
  TinyWireM.write(0x36); // Start with register 0x32 (ACCEL_XOUT_H)
  TinyWireM.endTransmission(false);
  TinyWireM.requestFrom(ADXL345, 2); // Read 6 registers total, each axis value is stored in 2 registers
  Z_out = ( TinyWireM.read() | TinyWireM.read() << 8); // Z-axis value
  zg = Z_out / 256.00; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  
  zms= zg*9.81;
  
  vReal[i] = zms;   
                             
  vImag[i] = 0.0;
  delay(sampling_period_us);
    }
    delay(100);
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
  maxR=0.0;
  //Serial.println("+++ Freq / Amplitude +++++");
  for(int i=1; i<(SAMPLES/2); i++)  {
      freq[i]=(i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES;
      if(vReal[i]>maxR)
 {
     maxR=vReal[i];
     max = static_cast<int>(freq[i]);
 }
  }
 
  char maxStr[6];
  itoa(max, maxStr, 10); // Convert temperature to a string
  vw_send((uint8_t*)maxStr, strlen(maxStr));
  vw_wait_tx(); // Wait for the transmission to complete

  delay(1000);
   
}  
