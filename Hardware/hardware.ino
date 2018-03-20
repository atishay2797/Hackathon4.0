#ifndef MQ135_H
#define MQ135_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/// The load resistance on the board
//#define RLOAD 10.0
#define RLOAD 0.977
/// Calibration resistance at atmospheric CO2 level
#define RZERO 162.27
/// Parameters for calculating ppm of CO2 from sensor resistance
#define PARA 116.6020682
#define PARB 2.769034857

/// Parameters to model temperature and humidity dependence
#define CORA 0.00035
#define CORB 0.02718
#define CORC 1.39538
#define CORD 0.0018

/// Atmospheric CO2 level for calibration purposes
#define ATMOCO2 397.13

class MQ135 {
 private:
  uint8_t _pin;

 public:
  MQ135(uint8_t pin);
  float getCorrectionFactor(float t, float h);
  float getResistance();
  float getCorrectedResistance(float t, float h);
  float getPPM();
  float getCorrectedPPM(float t, float h);
  float getRZero();
  float getCorrectedRZero(float t, float h);
};
#endif

MQ135::MQ135(uint8_t pin) {
  _pin = pin;
}



float MQ135::getCorrectionFactor(float t, float h) {
  return CORA * t * t - CORB * t + CORC - (h-33.)*CORD;
}


float MQ135::getResistance() {
  int val = analogRead(_pin);
  return ((1023./(float)val) * 5. - 1.)*RLOAD;
}


float MQ135::getCorrectedResistance(float t, float h) {
  return getResistance()/getCorrectionFactor(t, h);
}


float MQ135::getPPM() {
  return PARA * pow((getResistance()/RZERO), -PARB);
}

float MQ135::getCorrectedPPM(float t, float h) {
  return PARA * pow((getCorrectedResistance(t, h)/RZERO), -PARB);
}


float MQ135::getRZero() {
  return getResistance() * pow((ATMOCO2/PARA), (1./PARB));
}


float MQ135::getCorrectedRZero(float t, float h) {
  return getCorrectedResistance(t, h) * pow((ATMOCO2/PARA), (1./PARB));

}
#include <LiquidCrystal.h>
#include <stdlib.h>
//#include <String.h>
#define pinMQ135     A5
MQ135 gasSensor = MQ135(pinMQ135);
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);


String ssid     = "Simulator Wifi";	// SSID to connect to
String password = ""; // Our virtual wifi has no password (so dont do your banking stuff on this network)

String host     = "api.thingspeak.com"; // Open Weather Map API
const int httpPort   = 80;
String uri		 = "/update?api_key=NTX73L1H0DKA9IKH&field1= ";

// the setup routine runs once when you press reset:
void setup() {
  // Setup LCD and put some information text on there
  lcd.begin(16,2);
  
  lcd.setCursor(1,0);
  lcd.print("Start....");
  
   Serial.begin(115200); //Initialize serial port - 9600 bps
  
  float rzero = gasSensor.getRZero();
  
  
  // Start our ESP8266 Serial Communication
 	// Serial connection over USB to computer
  Serial.println("AT");		// Serial connection on Tx / Rx port to ESP8266
  delay(10);				// Wait a little for the ESP to respond
    
  // Connect to 123D Circuits Simulator Wifi
  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  delay(10);				// Wait a little for the ESP to respond
  
  Serial.println("AT+CIPMUX=1");
  delay(50);
  
    
 
  // Open TCP connection to the host:
  
  Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
  delay(50);				// Wait a little for the ESP to respond
  

}


char outstr[4];

void loop()
{
   int sensorTemp = analogRead(A0);
  
 // float Temperature = (150- (147.00/299.00)*(307-sensorTemp));
  float Temperature =150;
  lcd.print(Temperature);
  Serial.print(Temperature);
  delay(500);
  
  float sensor_volt;
    float RS_gas; // Get value of RS in a GAS
    float ratio; // Get ratio RS_GAS/RS_air
    int sensorValue = analogRead(A4);
    sensor_volt=(float)sensorValue/1024*5.0;
    RS_gas = (5.0-sensor_volt)/sensor_volt; // omit *RL

          /*-Replace the name "R0" with the value of R0 in the demo of First Test -*/
    ratio = RS_gas/1.02;  // ratio = RS/R0
          /*-----------------------------------------------------------------------*/

    Serial.print("sensor_volt = ");
    Serial.println(sensor_volt);
    Serial.print("RS_ratio = ");
    Serial.println(RS_gas);
    Serial.print("Rs/R0 = ");
    Serial.println(ratio);
   lcd.print(ratio);

    Serial.print("\n\n");

    delay(100);
  
  
  
  
  
  float co2ppm = gasSensor.getPPM();   
  Serial.print(F("CO2ppm "));
  Serial.println(co2ppm);  
  lcd.setCursor(0, 0);  
  lcd.print("CO2ppm          ");            
  lcd.setCursor(7, 0);  
  lcd.print(co2ppm,0);

  String httpPacket = "GET " + uri;
  lcd.setCursor(3, 1);
  lcd.print(outstr);
  delay(500);
  httpPacket +=co2ppm;
   httpPacket +="&field2=";
  httpPacket +=ratio;
   httpPacket +="&field3=";
    httpPacket +=Temperature;
  httpPacket += " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";

  int length = httpPacket.length();

     // Send our message length
     Serial.print("AT+CIPSEND=");
     Serial.println(length);
     delay(10); // Wait a little for the ESP to respond
     
  // Send our http request
     Serial.print(httpPacket);
     delay(10); // Wait a little for the ESP to respond
     
     lcd.setCursor(1, 0);
     lcd.print("DataSentSuccess");
    delay(1000);
    lcd.clear();
  
  
  //Serial.println("AT+CIPCLOSE");
  

  
}
