/*
 * file eau01.ino
 * 
 *  water quality monitoring system
 *  sensors used:
 *  Gravity: Analog pH Sensor/Meter Kit V2, SKU:SEN0161-V2
 *  temperature sensor  DS18B20
 *  Gravity: Analog TDS Sensor/ Meter for Arduino
 *  Gravity: Analog Turbidity Sensor For Arduino 
 *  
 *  software: arduino ide
 *  Android App "Arduino Bluetooth Data"
 *  
 * date  27-05-2022
 */


#include <EEPROM.h>
#include "GravityTDS.h"  /* tds sensor library */
#include "DFRobot_PH.h"  /* ph sensor library */
#include <OneWire.h>         // temprature
#include <SoftwareSerial.h>
#include <DallasTemperature.h>


#define TurbidityPin A0//
#define TdsSensorPin A1
#define PH_PIN A2
#define ONE_WIRE_BUS 5  //DS18S20 Signal pin on digital 5

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);//Temperature chip i/o

SoftwareSerial BTserial(10, 11); // RX | TX bluetooth module hc05



float temperature=25;

GravityTDS gravityTds;
float tdsValue = 0;

float turbidityValue;

float voltage,phValue;
DFRobot_PH ph;

void setup()
{
    //Serial.begin(115200);
      
    sensors.begin();  // start temprature sensor
    ph.begin();             // start PH sensor
    BTserial.begin(9600);   // start Bluetooth module

    
    gravityTds.setPin(TdsSensorPin);  // set TDS srnsor pin
    gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();  //initialization of TDS
}

void loop()
{
       
        temperature = readTemperature(); // read your temperature sensor to execute temperature compensation
        phValue = readPH();  // read PH sensor data
        tdsValue = readTDS();  // read TDS sensor 
        turbidityValue=readTurbidity(); // read turbidity sensor 

        sendToMobile();  // display on mobile phone

    /*
        Serial.print(tdsValue,0);
        Serial.println("ppm");
   
        Serial.print("    Trubidity value: ");
        Serial.println( Turbidityval);//
        Serial.print("temperature:");
        Serial.print(temperature,1);
        Serial.print("^C  pH:");
        Serial.println(phValue,2);
   
    ph.calibration(voltage,temperature);           // calibration process by Serail CMD
    */
     delay(1000);
}
//--------------------------------------------------------
//--------------------------------------------------------

float readTemperature()
{
  // code  to get the temperature from your temperature sensor
  float celcius;
  sensors.requestTemperatures(); 
  celcius=sensors.getTempCByIndex(0);
  return celcius;
}

//----------------------------

float readPH()
{
  // code  to get the PH from PH sensor
  float phvalue;
    voltage = analogRead(PH_PIN)/1024.0*5000;  // read the voltage
    phValue = ph.readPH(voltage,temperature);  // convert voltage to pH with temperature compensation
    
  return phvalue;
}



//----------------------------

float readTDS()
{
  // code  to get the TDS from TDS sensor
  float TDSvalue;
    gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
    gravityTds.update();  //sample and calculate
    TDSvalue = gravityTds.getTdsValue();  // then get the value
    
  return TDSvalue;
}

//-----------------------------

float readTurbidity()//
{

   // code  to get the turbidity from Turbidity sensor
   float  Turbidityval; 
   Turbidityval=analogRead(TurbidityPin)/1024.0*5.0;

   return Turbidityval;
   
   
  }
//-----------------------------
 void sendToMobile()
 {
  
   //IMPORTANT: The complete String has to be of the Form: 1234,1234,1234,1234;

   //(every Value has to be seperated through a comma (',') and the message has to

   //end with a semikolon (';'))
   
   String myString = "";     // empty string
   
   myString.concat(temperature);
   BTserial.print(myString);

   BTserial.print(",");

   myString="";
   myString.concat(phValue);
   BTserial.print(myString);

   BTserial.print(",");

   myString="";
   myString.concat(tdsValue);
   BTserial.print(myString);

   BTserial.print(",");

   myString="";
   myString.concat(turbidityValue);
   BTserial.print(myString);


   BTserial.print(";");

   //message to the receiving device

   delay(20);
  
  
  }
  
