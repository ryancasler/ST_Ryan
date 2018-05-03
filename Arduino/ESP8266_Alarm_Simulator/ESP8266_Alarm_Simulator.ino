/*
 *  Contains code from: 
 *  http://randomnerdtutorials.com/esp8266-remote-controlled-sockets/
 *  http://www.bruhautomation.com/p/cheapest-wifi-automated-blinds.html
 *  https://www.arduino.cc/en/Tutorial/Sweep
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 *  in compliance with the License. You may obtain a copy of the License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 *  on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License
 *  for the specific language governing permissions and limitations under the License.
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Servo.h>

#define WLAN_SSID       "YOUR WIFI SSID" //change to your Wifi SSID
#define WLAN_PASS       "YOUR WIFI PASSWORD" //change to your Wifi Password

MDNSResponder mdns;
ESP8266WebServer server(80);

int servoPin = D1; //Servo on GPIO0 or NODEMCU pin D3
const int buttonPin = D2; //manual button on GPIO12 or NODEMCU pin D6
int buttonState = 0;
int direction = 0;
int setting = 0; //Startup with blinds closed
int position = 45;
Servo servoA;

String webPage = "";

void setup(void) {
WiFi.mode(WIFI_STA);
  delay(1000);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  delay(500);
  Serial.println("Blind Startup Sequence");
  delay(500);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  // Set a static IP (optional)
  //IPAddress ip(10,0,1,50);
  //IPAddress gateway(10,0,1,1);
  //IPAddress subnet(255, 255, 255, 0);
  //WiFi.config(ip, gateway, subnet);
  // End of set a static IP (optional)
  delay(500);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    ESP.wdtFeed();
    if (i > 40)                                        // Try 40 times to connect to Wifi
      Serial.print("Restarting Wifi"); ESP.reset();    // Reset Wifi stack if more than 40 trys
    i++;
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    // Set a static IP retry (optional)
    //IPAddress ip(10,0,1,50);
    //IPAddress gateway(10,0,1,1);
    //IPAddress subnet(255, 255, 255, 0);
    //WiFi.config(ip, gateway, subnet);
    // End of set a static IP retry (optional)
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

server.on("/", []() { server.send(200, "text/html", webPage); });
server.on("/ON", []() { server.send(200, "text/html", webPage); setting = 90; });
server.on("/OFF", []() { server.send(200, "text/html", webPage);  setting = 0; });
  server.begin();
  Serial.println("HTTP server started");
}


void loop() {

buttonState = digitalRead(buttonPin);

if(buttonState == LOW && setting < 90 && direction == 0){
  setting++;
  delay(0);
  if(setting == 90){
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
  
if(buttonState == LOW && setting == 90 && direction == 0){
  setting--;
  delay(1000);
  }

if(buttonState == LOW && setting == 0 && direction == 1){
  setting++;
  delay(1000);
  }
  if(buttonState == LOW && setting > 90){
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
}
