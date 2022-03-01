/*
 * 
 * 1. Greeting: "AMETA Beeer Cooler"
 * 
 * 2. "Cooling" - blinking (T > 5°C)
 * 
 * 3. "Ready to draw beer!" (T = 5°C to T = 6°C) 
 * 
 * 
 * 
 * ROM = 28 FF 26 4F A8 15 1 C8
 * ROM = 28 57 91 B7 11 20 6 25  * 
 * ROM = 28 FE 1D 34 15 20 6 CA
 */

#include <Wire.h> 
#include <OneWire.h> 
#include <LiquidCrystal_PCF8574.h>
#include "DallasTemperature.h"

#define SENS_PIN 3
#define COOL1_PIN 5
#define COOL2_PIN 6
#define COOL3_PIN 7

#define INTERVAL 900000

OneWire oneWire(SENS_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1 = { 0x28, 0xFF, 0x26, 0x4F, 0xA8, 0x15, 0x1, 0xC8 };   // beer
DeviceAddress sensor2 = { 0x28, 0x57, 0x91, 0xB7, 0x11, 0x20, 0x6, 0x25 };   // water
DeviceAddress sensor3 = { 0x28, 0xFE, 0x1D, 0x34, 0x15, 0x20, 0x6, 0xCA };   // fan

LiquidCrystal_PCF8574 lcd(0x3f);        // Set the LCD I2C address

float temp1 = 99.0, temp2 =99.0, temp3 = 90.0;
float testTemp(10);

bool cooler1(0), cooler2(0), cooler3(0);
bool C1_ToBeChanged(0), C2_ToBeChanged(0), C3_ToBeChanged(0);
unsigned long C1_changeTime(0), C2_changeTime(0), C3_changeTime(0);

void greeting(const LiquidCrystal_PCF8574 &scr);
void screensaver(const LiquidCrystal_PCF8574 &scr);

void setup()
{
  Serial.begin(9600);
  
  sensors.begin();
  sensors.setResolution(12);
  
  lcd.begin(16, 2);               // initialize the lcd
  lcd.setBacklight(1);  
  
  Serial.println("Temp1  Temp2  Temp3");

  pinMode( COOL1_PIN, OUTPUT);
  pinMode( COOL2_PIN, OUTPUT);
  pinMode( COOL3_PIN, OUTPUT);  

  digitalWrite( COOL1_PIN, HIGH);
  digitalWrite( COOL2_PIN, HIGH);
  digitalWrite( COOL3_PIN, HIGH);  

  C1_changeTime = C2_changeTime = C3_changeTime = millis();
  
    
  greeting(lcd);
  delay(10000);
   
}


void loop()
{
  sensors.requestTemperatures(); // Send the command to get temperatures
  
  temp1 = sensors.getTempC(sensor1);    // Temp after heat exchange 
  temp2 = sensors.getTempC(sensor2);    // Temp at output of the cistern (before cooler)
  temp3 = sensors.getTempC(sensor3);    // Temp on the air radiator (alarm condition)


  // readData(); temp1 = testTemp;

  if ( cooler1 && temp1 < 5 ) {                     // The outer elements should be switched off
    if ( millis() - C1_changeTime > INTERVAL ){
      digitalWrite( COOL1_PIN, LOW);
      digitalWrite( COOL3_PIN, LOW);
      cooler1 = false;
      cooler3 = false;
      C1_changeTime = millis();
      C3_changeTime = millis();
    }
  }

  if ( !cooler1 && temp1 >= 5 ) {                     // The outer elements should be switched off
    if ( millis() - C1_changeTime > INTERVAL ){
      digitalWrite( COOL1_PIN, HIGH);
      digitalWrite( COOL3_PIN, HIGH);
      cooler1 = true;
      cooler3 = true;
      C1_changeTime = millis();
      C3_changeTime = millis();
    }
  }
  
  if ( cooler2 && temp1 < 3 ) {                   // The middle element should be switched off
    if ( millis() - C2_changeTime > INTERVAL ){
        digitalWrite( COOL2_PIN, LOW);
        cooler2 = false;
        C2_changeTime = millis();
      }
  } else if ( !cooler2 && temp1 >= 3 ) {
    if ( millis() - C2_changeTime > INTERVAL ){
        digitalWrite( COOL2_PIN, HIGH);
        cooler2 = true;

        Serial.println (millis() - C2_changeTime);
        C2_changeTime = millis();
      }
  }
  
  
  
  Serial.print(temp1); 
  Serial.print(",");
  Serial.print(temp2);
  Serial.print(",");
  Serial.println(temp3);  
 
   /***** LCD part *****/
  
   // greeting(lcd);

   // for ( uint8_t i = 0; i < 3; i++) alarm(lcd);
      
   delay(200);

   lcd_data(lcd, temp1, temp2, temp3);
}

void greeting(const LiquidCrystal_PCF8574 &scr)
{ //локального объекта Person не создаётся
  scr.setBacklight(0);
  delay(100);
  scr.setBacklight(1);
  
  scr.clear();   
  scr.print("      AMETA ");  
  scr.setCursor ( 0, 1 );        // go to the next line
  scr.print ("   Beer cooler   ");
  delay ( 300 );  
}

void alarm(const LiquidCrystal_PCF8574 &scr)
{ 
  scr.setBacklight(1);
  
  scr.clear();   
  scr.print("      ALARM !!! ");  
  scr.setCursor ( 0, 1 );        // go to the next line
  scr.print ("   Overheating !!!   ");
  
  delay ( 500 );  
  scr.setBacklight(0);
  delay ( 500 );
  
  scr.setBacklight(1);
}

void readData() {

    String Inp = "";
  
    Inp = Serial.readString();
    if ( Inp.length() > 0 && Inp.toFloat() ) testTemp = Inp.toFloat(); // If there is an input AND this input contains meaningful number
}

void lcd_data(const LiquidCrystal_PCF8574 &scr, float t1, float t2, float t3)
{ 
   lcd.clear();
   lcd.setCursor (0, 0);
   lcd.print("Beer Water Fan");
   lcd.setCursor (0, 1);

   if (t1 > 1 && t1 < 40) {                   
    if (t1 < 10 && t1 >=1 ) lcd.print(" ");
    lcd.print (t1, 1);  lcd.print (" ");}
   else if( t1 <= 1 && t1 > -127 ){                      // Beer temperature under 1 degree!
    lcd.print("errL"); lcd.print (" ");
    // alarm beer low temp!!!
    }
   else if( t1 >= 40 ){
    lcd.print("errH"); lcd.print (" ");    // Beer temperature over 40 degree!
    // alarm beer high temp!!!
    }
   else {                                    // Beer temperature sensor out of order! (-127)
      lcd.print("Sens"); lcd.print (" ");
      // alarm water sensor!!!
    }

   if (t2 > 0 && t2 < 30) {
      if (t2 < 10) lcd.print(" ");
      lcd.print (t2, 1);  lcd.print ("  "); }
   else if(t2 >= 30) {                        // Water temperature over 30 degree!
    lcd.print("errH"); lcd.print ("  ");
    // alarm water high temp!!
    }  
    else {                                    // Water temperature sensor out of order! (temp. under zero)
    lcd.print("Sens"); lcd.print ("  ");
    // alarm water sensor!!!
    }

   if (t3 > -10 && t3 < 40) {
      lcd.print (t3, 1);  lcd.print ("  "); }
   else if(t3 >= 40) {                        // Cooling elements overheating
    lcd.print("errH"); lcd.print ("  ");
    // alarm air!
    }  
    else if(t3 <= -10 && t3 > -127) {         // Cooling air under -10 degrees (???)
    lcd.print("errL"); lcd.print ("  ");
    // alarm air!
    }  
    else {                                    // Cooling air sensor out of order! (-127)
    lcd.print("Sens"); lcd.print ("  ");
    // alarm water sensor!!!
    }

   delay(1000);
}
