#ifndef MQ135_H
#define MQ135_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif


#define RLOAD 0.977
#define RZERO 162.27
#define PARA 116.6020682
#define PARB 2.769034857
#define CORA 0.00035
#define CORB 0.02718
#define CORC 1.39538
#define CORD 0.0018


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


String ssid     = "Simulator Wifi";
String password = "";
String host     = "api.thingspeak.com";
const int httpPort   = 80;
String uri		 = "/update?api_key=NTX73L1H0DKA9IKH&field1= ";


void setup() {
  
  lcd.begin(16,2);
  
  lcd.setCursor(1,0);
  lcd.print("Start....");
  
   Serial.begin(115200); 
  
  float rzero = gasSensor.getRZero();
  
  
  
  Serial.println("AT");	
  delay(10);			
    
  
  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  delay(10);			
  
  Serial.println("AT+CIPMUX=1");
  delay(50);
  Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
  delay(50);		
  

}


char outstr[4];

void loop()
{
   int sensorTemp = analogRead(A0);
  float Temperature = (150- (147.00/299.00)*(307-sensorTemp));
  lcd.print(Temperature);
  Serial.print(Temperature);
  delay(500);
  float sensor_volt;
    float RS_gas; 
    float ratio; 
    int sensorValue = analogRead(A4);
    sensor_volt=(float)sensorValue/1024*5.0;
    RS_gas = (5.0-sensor_volt)/sensor_volt;

     
    ratio = RS_gas/1.02;  // ratio = RS/R0
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

    
     Serial.print("AT+CIPSEND=");
     Serial.println(length);
     delay(10); 
     
 
     Serial.print(httpPacket);
     delay(10); 
     
     lcd.setCursor(1, 0);
     lcd.print("DataSentSuccess");
    delay(1000);
    lcd.clear();
  

  
}
