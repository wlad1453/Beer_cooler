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
 * ROM = 28 F 7B A0 12 20 6 97
 * ROM = 28 57 91 B7 11 20 6 25  * 
 * ROM = 28 FE 1D 34 15 20 6 CA
 */



#include <Wire.h> 
#include <OneWire.h> 
 // #include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_PCF8574.h>
#include "DallasTemperature.h"

#define SENS_PIN 3

OneWire oneWire(SENS_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1 = { 0x28, 0xF, 0x7B, 0xA0, 0x12, 0x20, 0x6, 0x97 };    // beer
DeviceAddress sensor2 = { 0x28, 0x57, 0x91, 0xB7, 0x11, 0x20, 0x6, 0x25 };   // water
DeviceAddress sensor3 = { 0x28, 0xFE, 0x1D, 0x34, 0x15, 0x20, 0x6, 0xCA };   // fan

LiquidCrystal_PCF8574 lcd(0x3f);        // Set the LCD I2C address

// Creat a set of new characters

const uint8_t charBitmap[][8] = {
   { 0xc, 0x12, 0x12, 0xc, 0, 0, 0, 0 },
   { 0x6, 0x9, 0x9, 0x6, 0, 0, 0, 0 },
   { 0x0, 0x6, 0x9, 0x9, 0x6, 0, 0, 0x0 },
   { 0x0, 0xc, 0x12, 0x12, 0xc, 0, 0, 0x0 },
   { 0x0, 0x0, 0xc, 0x12, 0x12, 0xc, 0, 0x0 },
   { 0x0, 0x0, 0x6, 0x9, 0x9, 0x6, 0, 0x0 },
   { 0x0, 0x0, 0x0, 0x6, 0x9, 0x9, 0x6, 0x0 },
   { 0x0, 0x0, 0x0, 0xc, 0x12, 0x12, 0xc, 0x0 }
   
};

float temp1 = 99.0, temp2 =99.0, temp3 = 90.0;

bool pumpOn(0), fanOn(0), oneThirdCooler(0), twoThirdCooler(0);

void greeting(const LiquidCrystal_PCF8574 &scr);
void screensaver(const LiquidCrystal_PCF8574 &scr);


void setup()
{
  Serial.begin(9600);
  
  sensors.begin();
  sensors.setResolution(12);
  
  lcd.begin(16, 2);               // initialize the lcd
  lcd.setBacklight(1);  
  
   int charBitmapSize = (sizeof(charBitmap ) / sizeof (charBitmap[0]));
   for ( int i = 0; i < charBitmapSize; i++ )
   {
      lcd.createChar ( i, (uint8_t *)charBitmap[i] );
   }  

   Serial.println("Temp1  Temp2  Temp3");
    
   greeting(lcd);
   delay(10000);
   
}


void loop()
{
  // unsigned long tm = millis();
  
  sensors.requestTemperatures(); // Send the command to get temperatures

  // Serial.println(millis() - tm);
  
  temp1 = sensors.getTempC(sensor1);    // Temp after heat exchange 
  temp2 = sensors.getTempC(sensor2);    // Temp at output of the cistern (before cooler)
  temp3 = sensors.getTempC(sensor3);    // Temp on the air radiator (alarm condition)

  // Serial.println(millis() - tm);
  
  Serial.print(temp1); 
  Serial.print(",");
  Serial.print(temp2);
  Serial.print(",");
  Serial.println(temp3);  
 
   /***** LCD part *****/
  
   // greeting(lcd);

   // screensaver(lcd);

   // for ( uint8_t i = 0; i < 3; i++) alarm(lcd);
      
   delay(200);

   lcd_data(lcd, temp1, temp2, temp3);

   /*

   lcd.clear();
   lcd.setCursor (0, 0);
   lcd.print("Beer  Water  Fan");
   lcd.setCursor (0, 1);
   lcd.print (temp1, 2);  lcd.print (" ");   
   lcd.print (temp2, 2);  lcd.print ("  ");   
   lcd.print ("54.");

   delay(1000);
   */
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

void screensaver(const LiquidCrystal_PCF8574 &scr)
{  
   scr.clear();
   for ( int i = 0; i < 2; i++ )
   {
      for ( int j = 0; j < 16; j++ )
      {        
        
        if( i == 0 ) {
          scr.setCursor ( j, i);
          if ( j < 8 )    scr.print (char(7 - j));
          else            scr.print (char(j - 8));
        } else {
          scr.setCursor ( (15 - j), i);
          if ( j < 8 )    scr.print (char(j));
          else            scr.print (char(15 - j));          
          }
          delay(25);
      }      
  }
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
