//******************************************************************************************
//  File: ST_Anything_Multiples_ESP8266WiFi.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one NodeMCU ESP8266 to 
//            implement a multi input/output custom device for integration into SmartThings.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the NodeMCU ESP8266's WiFi.
//
//            ST_Anything_Multiples implements the following ST Capabilities as a demo of what is possible with a single NodeMCU ESP8266
//              - 1 x Alarm device (using a simple digital output)
//              - 1 x Contact Sensor devices (used to monitor magnetic door sensors)
//              - 1 x Switch devices (used to turn on a digital output (e.g. LED, relay, etc...)
//              - 1 x Motion devices (used to detect motion)
//              - 1 x Smoke Detector devices (using simple digital input)
//              - 1 x Temperature Measurement devices (Temperature from Dallas Semi 1-Wire DS18B20 device)
//              - 1 x Relay Switch devices (used to turn on a digital output for a set number of cycles And On/Off times (e.g.relay, etc...))
//              - 2 x Button devices (sends "pushed" if held for less than 1 second, else sends "held"
//              - 1 x Water Sensor devices (using the 1 analog input pin to measure voltage from a water detector board)
//    
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//    2017-02-12  Dan Ogorchock  Revised to use the new SMartThings v2.0 library
//    2017-04-17  Dan Ogorchock  New example showing use of Multiple device of same ST Capability
//                               used with new Parent/Child Device Handlers (i.e. Composite DH)
//    2017-05-25  Dan Ogorchock  Revised example sketch, taking into account limitations of NodeMCU GPIO pins
//    2018-02-09  Dan Ogorchock  Added support for Hubitat Elevation Hub
//
//******************************************************************************************
//******************************************************************************************
// SmartThings Library for ESP8266WiFi
//******************************************************************************************
#include <SmartThingsESP8266WiFi.h>

//******************************************************************************************
// ST_Anything Library 
//******************************************************************************************
#include <Constants.h>       //Constants.h is designed to be modified by the end user to adjust behavior of the ST_Anything library
#include <Device.h>          //Generic Device Class, inherited by Sensor and Executor classes
#include <Sensor.h>          //Generic Sensor Class, typically provides data to ST Cloud (e.g. Temperature, Motion, etc...)
#include <Executor.h>        //Generic Executor Class, typically receives data from ST Cloud (e.g. Switch)
#include <InterruptSensor.h> //Generic Interrupt "Sensor" Class, waits for change of state on digital input 
#include <PollingSensor.h>   //Generic Polling "Sensor" Class, polls Arduino pins periodically
#include <Everything.h>      //Master Brain of ST_Anything library that ties everything together and performs ST Shield communications

#include <PS_Illuminance.h>  //Implements a Polling Sensor (PS) to measure light levels via a photo resistor

#include <IS_Motion.h>       //Implements an Interrupt Sensor (IS) to detect motion via a PIR sensor
#include <IS_Contact.h>      //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin

//*************************************************************************************************
//NodeMCU v1.0 ESP8266-12e Pin Definitions (makes it much easier as these match the board markings)
//*************************************************************************************************
//#define LED_BUILTIN 16
//#define BUILTIN_LED 16
//
//#define D0 16  //no internal pullup resistor
//#define D1  5
//#define D2  4
//#define D3  0  //must not be pulled low during power on/reset, toggles value during boot
//#define D4  2  //must not be pulled low during power on/reset, toggles value during boot
//#define D5 14
//#define D6 12
//#define D7 13
//#define D8 15  //must not be pulled high during power on/reset

//******************************************************************************************
//Define which Arduino Pins will be used for each device
//******************************************************************************************

#define PIN_CONTACT_1             D2  //SmartThings Capabilty "Contact Sensor"
#define PIN_MOTION_1              D3  //SmartThings Capabilty "Motion Sensor" (HC-SR501 PIR Sensor)

//******************************************************************************************
//ESP8266 WiFi Information
//******************************************************************************************
String str_ssid     = "YOURWIFI";                           //  <---You must edit this line!
String str_password = "YOURPASSWORD";                   //  <---You must edit this line!
IPAddress ip(192, 168, 1, 44);       //Device IP Address       //  <---You must edit this line!
IPAddress gateway(192, 168, 1, 1);    //Router gateway          //  <---You must edit this line!
IPAddress subnet(255, 255, 255, 0);   //LAN subnet mask         //  <---You must edit this line!
IPAddress dnsserver(192, 168, 1, 1);  //DNS server              //  <---You must edit this line!
const unsigned int serverPort = 8090; // port to run the http server on

// Smartthings / Hubitat Hub TCP/IP Address
IPAddress hubIp(192, 168, 1, 24);    // smartthings/hubitat hub ip //  <---You must edit this line!

// SmartThings / Hubitat Hub TCP/IP Address: UNCOMMENT line that corresponds to your hub, COMMENT the other
const unsigned int hubPort = 39500;   // smartthings hub port
//const unsigned int hubPort = 39501;   // hubitat hub port

//******************************************************************************************
//st::Everything::callOnMsgSend() optional callback routine.  This is a sniffer to monitor 
//    data being sent to ST.  This allows a user to act on data changes locally within the 
//    Arduino sktech.
//******************************************************************************************
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Servo.h>

MDNSResponder mdns;
ESP8266WebServer server(80);

int servoPin = D1; //  <---You must edit this line!
const int buttonPin = D4;//  <---You must edit this line!
int buttonState = 0;
int direction = 0;
int setting = 0; //Startup with blinds closed
int position = 45;
Servo servoA;

String webPage = "";
void callback(const String &msg)
{
//  Serial.print(F("ST_Anything Callback: Sniffed data = "));
//  Serial.println(msg);
  
  //TODO:  Add local logic here to take action when a device's value/state is changed
  
  //Masquerade as the ThingShield to send data to the Arduino, as if from the ST Cloud (uncomment and edit following line)
  //st::receiveSmartString("Put your command here!");  //use same strings that the Device Handler would send
}

//******************************************************************************************
//Arduino Setup() routine
//******************************************************************************************
void setup()
{

  //******************************************************************************************
  //Declare each Device that is attached to the Arduino
  //  Notes: - For each device, there is typically a corresponding "tile" defined in your 
  //           SmartThings Device Hanlder Groovy code, except when using new COMPOSITE Device Handler
  //         - For details on each device's constructor arguments below, please refer to the 
  //           corresponding header (.h) and program (.cpp) files.
  //         - The name assigned to each device (1st argument below) must match the Groovy
  //           Device Handler names.  (Note: "temphumid" below is the exception to this rule
  //           as the DHT sensors produce both "temperature" and "humidity".  Data from that
  //           particular sensor is sent to the ST Hub in two separate updates, one for 
  //           "temperature" and one for "humidity")
  //         - The new Composite Device Handler is comprised of a Parent DH and various Child
  //           DH's.  The names used below MUST not be changed for the Automatic Creation of
  //           child devices to work properly.  Simply increment the number by +1 for each duplicate
  //           device (e.g. contact1, contact2, contact3, etc...)  You can rename the Child Devices
  //           to match your specific use case in the ST Phone Application.
  //******************************************************************************************
  //Polling Sensors
  
  //Interrupt Sensors 
 static st::IS_Contact             sensor1(F("contact1"), PIN_CONTACT_1, LOW, true, 1000);
 static st::IS_Motion              sensor2(F("motion1"), PIN_MOTION_1, HIGH, false, 1000);
  
  //*****************************************************************************
  //  Configure debug print output from each main class 
  //  -Note: Set these to "false" if using Hardware Serial on pins 0 & 1
  //         to prevent communication conflicts with the ST Shield communications
  //*****************************************************************************
  st::Everything::debug=true;
  st::Executor::debug=true;
  st::Device::debug=true;
  st::PollingSensor::debug=true;
  st::InterruptSensor::debug=true;

  //*****************************************************************************
  //Initialize the "Everything" Class
  //*****************************************************************************

  //Initialize the optional local callback routine (safe to comment out if not desired)
  st::Everything::callOnMsgSend = callback;
  
  //Create the SmartThings ESP8266WiFi Communications Object
    //STATIC IP Assignment - Recommended
    st::Everything::SmartThing = new st::SmartThingsESP8266WiFi(str_ssid, str_password, ip, gateway, subnet, dnsserver, serverPort, hubIp, hubPort, st::receiveSmartString);
 
    //DHCP IP Assigment - Must set your router's DHCP server to provice a static IP address for this device's MAC address
    //st::Everything::SmartThing = new st::SmartThingsESP8266WiFi(str_ssid, str_password, serverPort, hubIp, hubPort, st::receiveSmartString);

  //Run the Everything class' init() routine which establishes WiFi communications with SmartThings Hub
  st::Everything::init();
  
  //*****************************************************************************
  //Add each sensor to the "Everything" Class
  //*****************************************************************************
  st::Everything::addSensor(&sensor1);
  st::Everything::addSensor(&sensor2);
      
  //*****************************************************************************
  //Add each executor to the "Everything" Class
  //*****************************************************************************
    
  //*****************************************************************************
  //Initialize each of the devices which were added to the Everything Class
  //*****************************************************************************
  st::Everything::initDevices();
  
// SmartBlinds Webpage build
WiFi.mode(WIFI_STA);
  webPage += "<h1>ESP8266 Web Server</h1><p>Blinds<br>";
  webPage += "<a href=\"0\"><button>0</button></a><br>";
  webPage += "<a href=\"10\"><button>10</button></a><br>";
  webPage += "<a href=\"20\"><button>20</button></a><br>";
  webPage += "<a href=\"30\"><button>30</button></a><br>";
  webPage += "<a href=\"40\"><button>40</button></a><br>";
  webPage += "<a href=\"50\"><button>50</button></a><br>";
  webPage += "<a href=\"60\"><button>60</button></a><br>";
  webPage += "<a href=\"70\"><button>70</button></a><br>";
  webPage += "<a href=\"80\"><button>80</button></a><br>";
  webPage += "<a href=\"90\"><button>90</button></a><br>";
  webPage += "<a href=\"100\"><button>100</button></a><br>";

  delay(1000);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  delay(500);
  Serial.println("Blind Startup Sequence");
  delay(500);
  Serial.println();
  delay(500);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    ESP.wdtFeed();
    if (i > 40)                                        // Try 40 times to connect to Wifi
      Serial.print("Restarting Wifi"); ESP.reset();    // Reset Wifi stack if more than 40 trys
    i++;
}

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

server.on("/", []() { server.send(200, "text/html", webPage); });
server.on("/100", []() { server.send(200, "text/html", webPage); setting = 180; });
server.on("/99", []() { server.send(200, "text/html", webPage);  setting = 180; });
server.on("/98", []() { server.send(200, "text/html", webPage);  setting = 180; });
server.on("/97", []() { server.send(200, "text/html", webPage);  setting = 171; });
server.on("/96", []() { server.send(200, "text/html", webPage);  setting = 171; });
server.on("/95", []() { server.send(200, "text/html", webPage);  setting = 171; });
server.on("/94", []() { server.send(200, "text/html", webPage);  setting = 171; });
server.on("/93", []() { server.send(200, "text/html", webPage);  setting = 171; });
server.on("/92", []() { server.send(200, "text/html", webPage);  setting = 162; });
server.on("/91", []() { server.send(200, "text/html", webPage);  setting = 162; });
server.on("/90", []() { server.send(200, "text/html", webPage);  setting = 162; });

server.on("/89", []() { server.send(200, "text/html", webPage);  setting = 162; });
server.on("/88", []() { server.send(200, "text/html", webPage);  setting = 162; });
server.on("/87", []() { server.send(200, "text/html", webPage);  setting = 153; });
server.on("/86", []() { server.send(200, "text/html", webPage);  setting = 153; });
server.on("/85", []() { server.send(200, "text/html", webPage);  setting = 153; });
server.on("/84", []() { server.send(200, "text/html", webPage);  setting = 153; });
server.on("/83", []() { server.send(200, "text/html", webPage);  setting = 153; });
server.on("/82", []() { server.send(200, "text/html", webPage);  setting = 144; });
server.on("/81", []() { server.send(200, "text/html", webPage);  setting = 144; });
server.on("/80", []() { server.send(200, "text/html", webPage);  setting = 144; });

server.on("/79", []() { server.send(200, "text/html", webPage);  setting = 144; });
server.on("/78", []() { server.send(200, "text/html", webPage);  setting = 144; });
server.on("/77", []() { server.send(200, "text/html", webPage);  setting = 135; });
server.on("/76", []() { server.send(200, "text/html", webPage);  setting = 135; });
server.on("/75", []() { server.send(200, "text/html", webPage);  setting = 135; });
server.on("/74", []() { server.send(200, "text/html", webPage);  setting = 135; });
server.on("/73", []() { server.send(200, "text/html", webPage);  setting = 135; });
server.on("/72", []() { server.send(200, "text/html", webPage);  setting = 126; });
server.on("/71", []() { server.send(200, "text/html", webPage);  setting = 126; });
server.on("/70", []() { server.send(200, "text/html", webPage);  setting = 126; });

server.on("/69", []() { server.send(200, "text/html", webPage);  setting = 126; });
server.on("/68", []() { server.send(200, "text/html", webPage);  setting = 126; });
server.on("/67", []() { server.send(200, "text/html", webPage);  setting = 117; });
server.on("/66", []() { server.send(200, "text/html", webPage);  setting = 117; });
server.on("/65", []() { server.send(200, "text/html", webPage);  setting = 117; });
server.on("/64", []() { server.send(200, "text/html", webPage);  setting = 117; });
server.on("/63", []() { server.send(200, "text/html", webPage);  setting = 117; });
server.on("/62", []() { server.send(200, "text/html", webPage);  setting = 108; });
server.on("/61", []() { server.send(200, "text/html", webPage);  setting = 108; });
server.on("/60", []() { server.send(200, "text/html", webPage);  setting = 108; });

server.on("/59", []() { server.send(200, "text/html", webPage);  setting = 108; });
server.on("/58", []() { server.send(200, "text/html", webPage);  setting = 108; });
server.on("/57", []() { server.send(200, "text/html", webPage);  setting = 99; });
server.on("/56", []() { server.send(200, "text/html", webPage);  setting = 99; });
server.on("/55", []() { server.send(200, "text/html", webPage);  setting = 99; });
server.on("/54", []() { server.send(200, "text/html", webPage);  setting = 99; });
server.on("/53", []() { server.send(200, "text/html", webPage);  setting = 99; });
server.on("/52", []() { server.send(200, "text/html", webPage);  setting = 90; });
server.on("/51", []() { server.send(200, "text/html", webPage);  setting = 90; });
server.on("/50", []() { server.send(200, "text/html", webPage);  setting = 90; });

server.on("/49", []() { server.send(200, "text/html", webPage);  setting = 90; });
server.on("/48", []() { server.send(200, "text/html", webPage);  setting = 90; });
server.on("/47", []() { server.send(200, "text/html", webPage);  setting = 81; });
server.on("/46", []() { server.send(200, "text/html", webPage);  setting = 81; });
server.on("/45", []() { server.send(200, "text/html", webPage);  setting = 81; });
server.on("/44", []() { server.send(200, "text/html", webPage);  setting = 81; });
server.on("/43", []() { server.send(200, "text/html", webPage);  setting = 81; });
server.on("/42", []() { server.send(200, "text/html", webPage);  setting = 72; });
server.on("/41", []() { server.send(200, "text/html", webPage);  setting = 72; });
server.on("/40", []() { server.send(200, "text/html", webPage);  setting = 72; });


server.on("/39", []() { server.send(200, "text/html", webPage);  setting = 72; });
server.on("/38", []() { server.send(200, "text/html", webPage);  setting = 72; });
server.on("/37", []() { server.send(200, "text/html", webPage);  setting = 63; });
server.on("/36", []() { server.send(200, "text/html", webPage);  setting = 63; });
server.on("/35", []() { server.send(200, "text/html", webPage);  setting = 63; });
server.on("/34", []() { server.send(200, "text/html", webPage);  setting = 63; });
server.on("/33", []() { server.send(200, "text/html", webPage);  setting = 63; });
server.on("/32", []() { server.send(200, "text/html", webPage);  setting = 54; });
server.on("/31", []() { server.send(200, "text/html", webPage);  setting = 54; });
server.on("/30", []() { server.send(200, "text/html", webPage);  setting = 54; });

server.on("/29", []() { server.send(200, "text/html", webPage);  setting = 54; });
server.on("/28", []() { server.send(200, "text/html", webPage);  setting = 54; });
server.on("/27", []() { server.send(200, "text/html", webPage);  setting = 45; });
server.on("/26", []() { server.send(200, "text/html", webPage);  setting = 45; });
server.on("/25", []() { server.send(200, "text/html", webPage);  setting = 45; });
server.on("/24", []() { server.send(200, "text/html", webPage);  setting = 45; });
server.on("/23", []() { server.send(200, "text/html", webPage);  setting = 45; });
server.on("/22", []() { server.send(200, "text/html", webPage);  setting = 36; });
server.on("/21", []() { server.send(200, "text/html", webPage);  setting = 36; });
server.on("/20", []() { server.send(200, "text/html", webPage);  setting = 36; });

server.on("/19", []() { server.send(200, "text/html", webPage);  setting = 36; });
server.on("/18", []() { server.send(200, "text/html", webPage);  setting = 36; });
server.on("/17", []() { server.send(200, "text/html", webPage);  setting = 27; });
server.on("/16", []() { server.send(200, "text/html", webPage);  setting = 27; });
server.on("/15", []() { server.send(200, "text/html", webPage);  setting = 27; });
server.on("/14", []() { server.send(200, "text/html", webPage);  setting = 27; });
server.on("/13", []() { server.send(200, "text/html", webPage);  setting = 27; });
server.on("/12", []() { server.send(200, "text/html", webPage);  setting = 18; });
server.on("/11", []() { server.send(200, "text/html", webPage);  setting = 18; });
server.on("/10", []() { server.send(200, "text/html", webPage);  setting = 18; });  

server.on("/9", []() { server.send(200, "text/html", webPage);   setting = 18; });
server.on("/8", []() { server.send(200, "text/html", webPage);   setting = 18; });
server.on("/7", []() { server.send(200, "text/html", webPage);   setting = 9; });
server.on("/6", []() { server.send(200, "text/html", webPage);   setting = 9; });
server.on("/5", []() { server.send(200, "text/html", webPage);   setting = 9; });
server.on("/4", []() { server.send(200, "text/html", webPage);   setting = 9; });
server.on("/3", []() { server.send(200, "text/html", webPage);   setting = 9; });
server.on("/2", []() { server.send(200, "text/html", webPage);   setting = 0; });
server.on("/1", []() { server.send(200, "text/html", webPage);   setting = 0; });
server.on("/0", []() { server.send(200, "text/html", webPage);   setting = 0; });
  server.begin();
  Serial.println("HTTP server started");
  }

//******************************************************************************************
//Arduino Loop() routine
//******************************************************************************************
void loop()
{
 buttonState = digitalRead(buttonPin);

if(buttonState == LOW && setting < 180 && direction == 0){
  setting++;
  delay(0);
  if(setting == 180){
    delay(1000);
    direction = 1;
    }
  }
  
if(buttonState == LOW && setting > 0 && direction == 1){
  setting--;
  delay(0);
  if(setting == 0){
    direction = 0;
    delay(1000);
    }
  }
  
if(buttonState == LOW && setting == 180 && direction == 0){
  setting--;
  delay(1000);
  }

if(buttonState == LOW && setting == 0 && direction == 1){
  setting++;
  delay(1000);
  }
  if(buttonState == LOW && setting > 180){
  setting--;
  direction = 1;
  delay(0);
  }
if(position < setting){
   servoA.attach(servoPin);
   servoA.write(position++);
   delay(5);
     Serial.print("Setting: ");  
     Serial.println(setting);
     Serial.print("Position: ");
     Serial.println(position);
   }
   
if(position > setting){
   servoA.attach(servoPin);
   servoA.write(position--);
   delay(5);
     Serial.print("Setting: ");  
     Serial.println(setting);
     Serial.print("Position: ");
     Serial.println(position);
   }

if(position == setting){
  servoA.detach();
  }
server.handleClient();
 //*****************************************************************************
  //Execute the Everything run method which takes care of "Everything"
  //*****************************************************************************
  st::Everything::run();
}
