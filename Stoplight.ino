#include <ESP8266WiFi.h>              // This includes the libraries to connect to wifi and start a server

const char* wifiSSID = ":)";     // In order for this to work, you MUST specify the SSID for your wifi
const char* wifiPSK  = ":)";   // And the preshared key (wifi password)

WiFiServer server(80);                // This sets which port our server will listen on

int ledR = D3;                        // Sets which GPIO pin we will use for the Red LED
int ledY = D2;                        // Sets which GPIO pin we will use for the Yellow LED
int ledG = D1;                        // Sets which GPIO pin we will use for the Green LED
String ledState = "off";              // Initialize a variable for tracking the state of the LED
                                      // LED States: off, red, yellow, green, cycle, blink

int cycleFreq = 1000;                 // Time between lights turning on - 1000ms = 1s
unsigned long cycleTimer = 0;         // The timer to keep track of how long it's been since the last LED change

int cycleLed = 1;                     // Integer to keep track of which color LED we're on

// Turn off all LEDs and Reset the Cycle Timer and Cycle LED
void allOff() {
    cycleTimer = 0;
    cycleLed = 1;
    digitalWrite(ledR,LOW);
    digitalWrite(ledY,LOW);
    digitalWrite(ledG,LOW);
}

void setup() {                        // Perform this part once when first powered on
  Serial.begin(9600);                 // Open a serial connection (for debugging)
  delay(10);                          // Wait 10 milliseconds (1/100th of a second)
  Serial.println("Initializing ..."); // DEBUG CODE

  pinMode(ledR, OUTPUT);            // Set the Red LED pin to Output
  digitalWrite(ledR, LOW);          // Set the pin to LOW (Off)
  pinMode(ledG, OUTPUT);            // Set the Green LED pin to Output
  digitalWrite(ledG, LOW);          // Set the pin to LOW (Off)
  pinMode(ledY, OUTPUT);            // Set the Yellow LED pin to Output
  digitalWrite(ledY, LOW);          // Set the pin to LOW (Off)

  // ** Connect to WiFi network - Adapted from http://www.esp8266learning.com/wemos-webserver-example.php
  Serial.print("Connecting to ");           // Display debugging connection info
  Serial.println(wifiSSID);                 // Print the configured SSID to the Serial Monitor

  WiFi.begin(wifiSSID, wifiPSK);            // Use the provided SSID and PSK to connect

  while (WiFi.status() != WL_CONNECTED) {   // If not connected to wifi
    delay(500);                             // Pause
    Serial.print(".");                      // Print a dot each loop while trying to connect
  }
  Serial.println("");
  Serial.println("WiFi connected");         // Print "connected" message to the Serial Monitor

  server.begin();                           // Start the web server
  Serial.println("Server started");

  Serial.print("Control the LED Flashlight at: ");          // Print the connected IP address to the Serial Monitor
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
} // ** End Adapted Code - This is the end of the code that was adapted from www.esp8266learning.com

void loop() { // ** Create a web server - Adapted from http://www.esp8266learning.com/wemos-webserver-example.php
  WiFiClient client = server.available(); // Create a new client object for available connections
  if (client) {                           // If a client is connected, wait until it sends some data
    while (!client.available()) {         // If the client hasn't sent info, wait for it
      delay(10);
    }

    String request = client.readStringUntil('\r');  // read the first line of the request
    Serial.println(request);              // DEBUG CODE - Echo the request to the Serial Monitor for debug
    client.flush();                       // Wait until the buffers are clear

    if (request.indexOf("/led=red") != -1) {   // If the request is for the page "/led=red"
      allOff();                                //   Turn off the lights and reset the cycle timer
      digitalWrite(ledR, HIGH);                //   then set the Red led Pin to high (turn on)
      ledState = "red";                        //   and track the state as "red"
    }
    if (request.indexOf("/led=off") != -1) {  // If the request is for the page "/led=off"
      allOff();                               //   then set all the led Pins to low (turn off)
      ledState = "off";                       //   and track the state as "off"
    }
    if (request.indexOf("/led=green") != -1) {  // If the request is for the page "/led=green"
      allOff();                                 //   Turn off the lights and reset the cycle timer
      digitalWrite(ledG, HIGH);                 //   then set the green led Pin to high (turn on)
      ledState = "green";                       //   and track the state as "green"
    }
    if (request.indexOf("/led=yellow") != -1) {   // If the request is for the page "/led=yellow"
      allOff();                                   // Turn off the lights and reset the cycle timer
      digitalWrite(ledY, HIGH);                   //   then set the yellow led Pin to high (turn on)
      ledState = "yellow";                        //   and track the state as "yellow"
    }
    if (request.indexOf("/led=blink") != -1) {   // If the request is for the page "/led=yellow"
      cycleTimer = 0;
      cycleLed = 1;
      digitalWrite(ledY,LOW);
      digitalWrite(ledG,LOW);                     // Turn off the lights and reset the cycle timer
      digitalWrite(ledR, HIGH);                   //   then set the yellow led Pin to high (turn on)
      ledState = "blink";                        //   and track the state as "blink"
    }
    if (request.indexOf("/led=cycle") != -1) {    //  If the request is for the page "/led=cycle"
        if (cycleTimer == 0) {                    //  If this is the initial check (Cycle timer ==0)
            cycleTimer = millis() + cycleFreq;    //  Set the timer
            digitalWrite(ledR, LOW);              //  Turn off all other LEDs and turn on Green
            digitalWrite(ledY, LOW);
            digitalWrite(ledG, HIGH);
        }
      ledState = "cycle";                         //  and track the state as "cycle"
    }

    // ** End Adapted Code - This is the end of the code that was adapted from www.esp8266learning.com

    // Return the response
    client.println("<!DOCTYPE html>");
    client.println("<html>");
    client.println("<body>");
    client.println("");
    client.println("<h1>Stoplight over Wifi</h1>");
    client.println("");
    client.println("<p>Light Status:</p>");
    client.println("");
    client.print("<input type=\"radio\" id=\"off\" name=\"led\" value=\"off\"");
        if (ledState == "off") {
          client.print(" checked ");
        }
        client.print("onchange=\"window.location.href='/led=off'");
    client.println("\">");
    client.println("<label for=\"off\">Off</label><br>");
    client.println("<input type=\"radio\" id=\"red\" name=\"led\" value=\"red\"");
        if (ledState == "red") {
          client.print(" checked ");
        }
        client.print("onchange=\"window.location.href='/led=red'");
    client.println("\">");
    client.println("<label for=\"red\">Red</label><br>");
    client.println("<input type=\"radio\" id=\"yellow\" name=\"led\" value=\"yellow\"");
        if (ledState == "yellow") {
          client.print(" checked ");
        }
        client.print("onchange=\"window.location.href='/led=yellow'");
    client.println("\">");
    client.println("<label for=\"yellow\">Yellow</label><br>");
    client.println("<input type=\"radio\" id=\"green\" name=\"led\" value=\"green\"");
        if (ledState == "green") {
          client.print(" checked ");
        }
        client.print("onchange=\"window.location.href='/led=green'");
    client.println("\">");
    client.println("<label for=\"green\">Green</label><br>");
    client.println("<input type=\"radio\" id=\"cycle\" name=\"led\" value=\"cycle\"");
        if (ledState == "cycle") {
          client.print(" checked ");
        }
        client.print("onchange=\"window.location.href='/led=cycle'");
    client.println("\">");
    client.println("<label for=\"cycle\">Cycle</label><br>");
    client.println("<input type=\"radio\" id=\"blink\" name=\"led\" value=\"blink\"");
        if (ledState == "blink") {
          client.print(" checked ");
        }
        client.print("onchange=\"window.location.href='/led=blink'");
    client.println("\">");
    client.println("<label for=\"blink\">Blink</label><br>");
    client.println("</body>");
    client.println("</html>");
  } 
//  Code to cycle the LEDs
  if (millis() > cycleTimer && ledState == "cycle") {   //  If the mode is "cycle" and it's time to change LEDs
          cycleTimer = millis() + cycleFreq;            //  Reset the cycleTimer
          switch(cycleLed) {                            //  Depending on the cycleLed turn on an LED and the rest off
              case 0 : // Red
                  digitalWrite(ledG, LOW);
                  digitalWrite(ledY, LOW);
                  digitalWrite(ledR, HIGH);
                  cycleLed = 1;                         //  and set the cycleLed to the next color in order.
                  break;
              case 1 : // Green
                  digitalWrite(ledR, LOW);
                  digitalWrite(ledY, LOW);
                  digitalWrite(ledG, HIGH);
                  cycleLed = 2;                         //  and set the cycleLed to the next color in order.
                  break;
              case 2 : // Yellow
                  digitalWrite(ledG, LOW);
                  digitalWrite(ledR, LOW);
                  digitalWrite(ledY, HIGH);
                  cycleLed = 0;                         //  and set the cycleLed to the next color in order.
                  break;
          }
  } 
  else if (millis() > cycleTimer && ledState == "blink") {
    int blnk = digitalRead(ledR);
    digitalWrite(ledR, !blnk);
  }
  delay(100); // This introduces a little pause in each cycle. Probably helps save some power.
}
