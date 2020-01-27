/* Samuel Schindler
    Dr. Blythe
    Internet of Things Final Project
    November 27, 2019
*/

// --------------------------------------------------------------- Libraries -------------------------------------------------------
#include <ESP8266WiFi.h>    //Include to enable Node MCU Wifi
#include <SoftwareSerial.h> // Library for serial communication
// --------------------------------------------------------------- End Libraries ----------------------------------------------------
// --------------------------------------------------------------- Definitions ------------------------------------------------------
//Constants to connect to the web
const char* ssid = "Schindler";      //Network ID
const char* password = "samiamsgs";  //Network Pass

//Time String Variables to manipulate
String alarmTime;   //Full Clock Time
String theHour;     //Substring for hours
String theMinute;   //SubSstring for minutes
String fullTime;    //Rebuild the time from substrings

//Do I need these...?
int timeLength = fullTime.length() + 1;
char *dweetValue;

//Dweeting information to connect
const int port = 80;
const char* host = "www.dweet.io";
const char* thing = "SchindlerSleep";

//The WiFi Client
WiFiClient client;

bool AM;                  //Is is the morning or afternoon?
bool recorded = false;    //Has the specific value that was read been recorded?
bool valueRead = false;   //Was a value even read in the first place?
// --------------------------------------------------------------- End Definitions --------------------------------------------------
// --------------------------------------------------------------- Setup Code -------------------------------------------------------
void setup() {

  Serial.begin(9600); //Begin using serial monitor

  /* WIFI CONNECTION BLOCK */
  WiFi.begin(ssid, password);   //Start the Wifi Connection
  Serial.print("Connecting");   //Let the user know what is going on

  //Wifi Connection Code
  while (WiFi.status() != WL_CONNECTED)  // while not connected to Network
  {
    delay(500);
    Serial.print(".");  //Connecting...
  }

  //Print out local address
  Serial.print("IP address is: ");  //It was found
  Serial.println(WiFi.localIP());   //Here it is
  /* END WIFI CONNECTION BLOCK */
}
// --------------------------------------------------------------- End Setup Code ---------------------------------------------------
// --------------------------------------------------------------- Loop Code --------------------------------------------------------
void loop() {
  /* RECEIVE DATA FROM ARDUINO */
  if (Serial.available()) { //A Message is being sent
    
    alarmTime = Serial.readString(); //Store the value that was sent into a String

    if (!recorded) {
      String theHour = alarmTime.substring(0, 2);     //Grab the hour from the whole string
      String theMinute = alarmTime.substring(2, 5);   //Grab the minute from the whole string

      fullTime = theHour + ":" + theMinute; //Rebuild the String

      //Figure out if the time is AM or PM
      if (theHour.toInt() < 12) {     //it is the morning
        AM = true;
        fullTime += "AM";
      }
      else {                          //it is the evening
        AM = false;
        fullTime += "PM";
      }
      
      //Convert another Variable, the same as fullTime but an array of Char instead
      //Do I need this...?
      int timeLength = fullTime.length() + 1;
      char dweetValue[timeLength];
      fullTime.toCharArray(dweetValue, timeLength);

      //Update what's going on
      Serial.print("Alarm Triggered At ");
      Serial.println(fullTime);             //Print to serial monitor that the alarm went off
      recorded = true;                      //Something was Recorded
      valueRead = true;                     //A Value was read
    }
  }
  /* END RECEIVING DATA */
  recorded = false; //Prepre to receive another value

  /* SEND INFORMATION TO THE WEB */
  if (!client.connect(host, port)) //Make sure we have acess to dweet.io
  {
    Serial.println("Error Connecting to Dweet.io"); //Not Connected
  }

  //If a value is read(alarm went off), build and send a dweet stating the alarm time
  if (valueRead) {
    //Build the Dweet
    String url = "/dweet/for/";         //URL Prefix
    url += thing;                       //Dweet location
    url += "?";                         //Divider for actual data
    url += (char*) fullTime.c_str();    //The time alarm was triggered (in character array)

    //Verify the dweet that is about to be sent
    Serial.print("Requesting URL: ");
    Serial.println(url);

    //Actually send the dweet to the web
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +         //URL being dweeted
                 "Host: " + host + "\r\n" +                       //Host dweet is going to
                 "Connection: close\r\n\r\n");                    //Connection Status

    //Verify that the client isnt taking too long
    int timeout = millis() + 5000;               //Max Time
    while (client.available() == 0)
    {
      if (timeout - millis() < 0)
      {
        Serial.println(">>> Client Timeout");   //Took too long!
        client.stop();                          //Stop the client
        return;                                 //Exit
      }
    }

    //Print what's happening on the client (troubleshooting)
    while (client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }
  valueRead = false;  //Prepare to read another value (done with this one)
  /* END SENDING INFORMATION TO THE WEB */
}
// --------------------------------------------------------------- End Loop Code --------------------------------------------------------
