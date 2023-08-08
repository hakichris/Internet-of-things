 
// These constants won't change. They're used to give names to the pins used:
const int LDR = 39;  // Analog input pin that the potentiometer is attached to
const int led= 15; //  output digitalt pin that the RED  LED is attached to
const int button= 13;
const int buzzer = 12;
 

#define GREEN_LED 2 //
#define BLUE_LED 4 //
#define LIGHT_INTENSITY_THRESHOLD 75 
 
int lightIntensity_reads = 0;
 
int buttonState = 0;
int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)
 
 
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>
#include "DHT.h"
WiFiMulti wifiMulti;


String Location = "room";


// temperature 
#define YELLOW_LED 5 // AC 

#define DHTPIN 25   //  digital pin value for the  DHT11 sensor 
#define DHTTYPE DHT22   // DHT 22 (AM23002)

#define USE_SERIAL Serial
#define TEMP_UPPER_THRESHOLD 28
#define TEMP_LOWER_THRESHOLD 20

#define FIVE_SECONDS 5000
unsigned long last_check = 0; // checking the last run time of the yellow led 
DHT dht(DHTPIN, DHTTYPE);
unsigned long current_time =0;

float temp;
int dht_sensor_reads = 0;
float average_dht_temp = 0;



void setup() {
  //temperature 
  // initialize serial communications at 9600 bps:

  // initialize serial communications at 9600 bps:
  Serial.begin(115200);
  pinMode(button, INPUT);
  pinMode(led, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
 
  pinMode(BLUE_LED ,OUTPUT);
  pinMode(buzzer, OUTPUT);
 
  dht.begin();
 
    USE_SERIAL.begin(115200);
 
    // WIFI 
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }
 
    wifiMulti.addAP("Buce", "ragnarok");

}

void loop() {

  Temperature_Part();

  ALARM();
 
  LDR_Part();


}




void Temperature_Part(){

  delay(2000);
     
    current_time=millis(); 
             
// section B - put in a func if necessary 

    if (current_time - last_check >= FIVE_SECONDS ) {
        

        // save the last time   yellow LED blinked
        last_check = current_time;
              
        // Read temperature as Celsius- default
        temp = dht.readTemperature();
        
        Serial.print("Temperature: ");
        Serial.print(temp);
          Serial.println(F("°C "));
          

         // Check if any reads failed and exit early and try again.
        if (isnan( temp)) {
            Serial.println(F("Failed to read from DHT sensor!"));
        return;
        }

        

//Check if thresholds have been exceeded
        if (temp >= TEMP_UPPER_THRESHOLD || temp <= TEMP_LOWER_THRESHOLD){
            digitalWrite( YELLOW_LED, HIGH);  //using yellow LED for the Air-conditioner 
          }
         else {
          
            digitalWrite( YELLOW_LED, LOW);
          }

          
        if (dht_sensor_reads < 3){
            average_dht_temp =  average_dht_temp + (temp/3);
          }
        else{
          String  actuatorValue = digitalRead( YELLOW_LED)? "ON" : "OFF"; // using ternary operator 
          //send the average of the 3 readings and reset the values
         sendSensorData("DHT22",  "Temperature", average_dht_temp, last_check, "YELLOWLED", actuatorValue);
          
            dht_sensor_reads = 0;
            average_dht_temp = 0;
  
          }

        //increment sensor reads
        dht_sensor_reads ++;
     
    }

 }


void ALARM(){
 
  buttonState = digitalRead(button);
 
//outputValue > 75 and outputValue < 90) or
 
  if (buttonState == HIGH){ // using the button as an intruder
    
 
for (int i =0; i<=30;i++){
   
     digitalWrite(led,HIGH);
     digitalWrite(buzzer,LOW);
     delay(1000);
    
 
     digitalWrite(led,LOW);
     digitalWrite(buzzer,HIGH);
     delay(1000);
 
    
 
}
     
  }
  else {
     digitalWrite(led, LOW);
     digitalWrite(buzzer,LOW);
    
  }
  // print the results to the Serial Monitor:
  Serial.print("sensor = ");
  Serial.print(outputValue);
    Serial.print("\n");
 
 
   
 
}
 
 
 
void LDR_Part(){
 
  //section D to be in one function
    lightIntensity_reads  = analogRead(LDR);
    outputValue = map(lightIntensity_reads, 0, 1023, 0, 20);
 
 
  Serial.println("lightIntensity_reads ");
  Serial.println(lightIntensity_reads );
 
  Serial.print("ln");
  Serial.println("outputvalue");
  Serial.println(outputValue);
 
   
 
    // Check if any light  intensity read  failed and exit early (to try again).
        if (isnan(outputValue )) {
            Serial.println(F("Failed to read from light intensity sensor!"));
        return;
 
        }
 
       
         //Check if thresholds have been exceeded
        if (outputValue >= LIGHT_INTENSITY_THRESHOLD ){
            digitalWrite( GREEN_LED, HIGH);
            digitalWrite( BLUE_LED, LOW);
          }
         else{
            digitalWrite( BLUE_LED, HIGH);
            digitalWrite( GREEN_LED, LOW);
          }
 
        }
 

 
void sendSensorData(String sensorName,
                    String sensorType,
                    float averageSensorValue,
                    unsigned long MillisTime,
                    String ActuatorType,
                    String ActuatorValue) {

    // wait for WiFi connection
    if((wifiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");

        http.begin ("http://192.168.64.2/server_side/insert.php?insert&SensorName="+ sensorName + "&SensorType="+ sensorType
                                                + "&averageSensorValue="+  float (averageSensorValue)  + "&ActuatorType=" + ActuatorType + "&ActuatorValue=" + ActuatorValue + "&Location="+ Location + "&MillisTime=" + long (MillisTime)) ;

          USE_SERIAL.print(" SensorName= ");
         USE_SERIAL.print(sensorName);
         USE_SERIAL.print(" SensorType= ");
         USE_SERIAL.print(sensorType);
         USE_SERIAL.print(" averageSensorValue= ");
         USE_SERIAL.print(averageSensorValue);
         USE_SERIAL.print(" Location= ");
         USE_SERIAL.print(" ActuatorType= ");
         USE_SERIAL.print(ActuatorType);
         USE_SERIAL.print(" ActuatorValue= ");
         USE_SERIAL.println(ActuatorValue);
         USE_SERIAL.print(" MillisTime= ");
         USE_SERIAL.print(MillisTime);
         USE_SERIAL.print(" Location= ");
         USE_SERIAL.print(Location);
         
         
                                                      

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
                Serial.println("data sent to database sucessfully");
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            Serial.println("data  not sent to database sucessfully");
        }

        http.end();
    }

}



    
