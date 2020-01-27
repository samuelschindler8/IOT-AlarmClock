/* Samuel Schindler
   Dr. Blythe
   Internet of Things Final Project
   November 27, 2019
*/

// --------------------------------------------------------------- Libraries -------------------------------------------------------
#include <DHT.h>    //Library for DHT11
#include <RTClib.h> //Library for RTC
#include <Wire.h>   //Library for RTC
#include <LiquidCrystal.h> //Library for LCD
#include <SoftwareSerial.h> //Library for serial communication
// --------------------------------------------------------------- End Libraries ----------------------------------------------------
// --------------------------------------------------------------- Definitions ------------------------------------------------------
#define DHTPIN 7        // Pin to read in temperature from DHT11
#define DHTTYPE DHT11   // Define type of DHT

//Device Definitions
DHT dht(DHTPIN, DHTTYPE);              // Initialize DHT
RTC_DS3231 rtc;                        //Initialize RTC
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //Initialize LCD
int dataTransfer;                      //Variable to hold data serially communicated
int HIGHVOLTAGE = 8;                   //Pin to turn on light
bool recorded = false;                 //Has the time been recorded?

//Alarm Time                    //****************//
int ALARM_HOUR = 11;            //   SET ALARM    //
int ALARM_MINUTE = 17;           //    HERE !!!    //
                                //****************//

//Build a single string to hold both values
String completeTime = ALARM_HOUR + ":" + ALARM_MINUTE;

// --------------------------------------------------------------- End Definitions --------------------------------------------------
// --------------------------------------------------------------- Setup Code -------------------------------------------------------
void setup() {
  Serial.begin(9600);                 //Begin using serial monitor
  dht.begin();                        //Begin Using DHT11
  rtc.begin();                        //Begin Using RTC
  lcd.begin(16, 2);                   //Begin using LCD
  pinMode(9, OUTPUT);                 //Buzzer Pin
  pinMode(10, OUTPUT);                //Buzzer Pin
  pinMode(HIGHVOLTAGE, OUTPUT);       //Lightbulb output
  digitalWrite(HIGHVOLTAGE, HIGH);    //Default off
}
// --------------------------------------------------------------- End Setup Code ---------------------------------------------------
// --------------------------------------------------------------- Loop Code --------------------------------------------------------

void loop() {
  /* TEMPERATURE BLOCK */
  //Read in temperature
  //Check to make sure it is a number
  float temperatureF = dht.readTemperature(true); //Read temperature in farenheit

  //make sure the temperature is a number
  if (isnan(temperatureF))
  {
    temperatureF = dht.readTemperature(true);
  }
  /* END TEMPERATURE BLOCK */

  /* RTC BLOCK */
  DateTime now = rtc.now(); //Continually update the time to whatever it currently is

  //Set the Cursor to the second row (Display the date first)
  lcd.setCursor(0, 1);

  //Display the Date
  lcd.print("Date: ");
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.day());
  lcd.print("/");
  lcd.print(now.year());

  //Display the temp
  lcd.print(" & Temp: ");
  lcd.print(temperatureF);
  lcd.print((char)223);
  lcd.print("F");

  //Set the Cursor back to the top to display the time
  lcd.setCursor(0, 0);

  //Display the Hour
  if (now.hour() < 10)
  {
    lcd.print("0"); //Hour is not double digits, first spot is a 0
  }
  lcd.print("Time: ");
  lcd.print(now.hour() - 1);  //Central Time zone (I set up my RTC in Eastern Time Zone)
  lcd.print(":");

  //Display the Minute
  if (now.minute() < 10)
  {
    lcd.print("0");
  }
  lcd.print(now.minute());
  lcd.print(":");

  //Display the Second
  if (now.second() < 10)
  {
    lcd.print("0");
  }
  lcd.print(now.second());
  lcd.print("  "); //Finish out the line of display
  /* END RTC BLOCK */

  /* BUZZER BLOCK */
  //Check and see if the alarm time is right now, if so make it go for 30 seconds
  if (now.hour() - 1 == ALARM_HOUR && now.minute() == ALARM_MINUTE && now.second() < 30)
  {
    /* HIGH VOLTAGE BLOCK */
    digitalWrite(HIGHVOLTAGE, LOW); //Turn the light on and keep it on, dont want it to blink
    /* END HIGH VOLTAGE BLOCK */

    /* RECORD TIME */
    if (!recorded) //Only send the time (serially) one time
    {
      //send time
      Serial.print(ALARM_HOUR);   //Send Hour
      Serial.print(ALARM_MINUTE); //Send Minute

      recorded = true;  //It has been recorded
    }
    /* END RECORD TIME */

    /* ALARM GOING OFF */
    while (now.second() < 30) { //Only for 30 seconds
      
      tone(9, 261);     //Make the first buzzer go off
      tone(10, 261);    //Make the second buzzer go off
      
      //Flash a message (Wake up and Weather Info)
      lcd.setCursor(0, 0);
      lcd.print("TIME TO WAKE UP!!");

      lcd.setCursor(0, 1);
      lcd.print("TEMP: ");
      lcd.print(temperatureF, 2);
      lcd.print((char)223);
      lcd.print("F    ");

      delay(1500);

      //Make Messages and tone flash or beep respectively
      lcd.clear();

      noTone(9);
      noTone(10);

      delay(1500);

      //Update the time so alarm will eventuall stop
      now = rtc.now();
      /* END ALARM GOING OFF */
    }
  }
  recorded = false; //The next time will be differnt

  //Stop the Buzzing and turn off lights
  noTone(9);
  noTone(10);
  digitalWrite(HIGHVOLTAGE, HIGH);
  /* END BUZZER BLOCK */

  /* SERIAL COMMUNICATION BLOCK */
  //Do I need this...?
  dataTransfer = Serial.read();
  delay(1000);
  /* END SERIAL COMMUNICATION BLOCK */
}
// --------------------------------------------------------------- End Loop Code --------------------------------------------------------
