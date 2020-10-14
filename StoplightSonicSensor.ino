#include <ESP8266WiFi.h>
#include <WiFiClient.h>

String wifiSSID = ":)";
String wifiPSK = ":)";

int    HTTP_PORT   = 80;
String HTTP_METHOD = "GET"; // or "POST"
char   HOST_NAME[] = "10.0.0.109"; // hostname of web server:
String PATH_NAME   = "/led=green";

String ledGREEN = "/led=green";
String ledRED = "/led=red";
String ledYELLOW = "/led=yellow";
String ledBLINK = "/led=blink";

WiFiClient client;

const int trigPin = D6;
const int echoPin = D5;

float duration, distance;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);

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

  

  // connect to web server on port 80:
  if(client.connect(HOST_NAME, HTTP_PORT)) {
    // if connected:
    Serial.println("Connected to server");
    // make a HTTP request:
    // send HTTP header
    client.println("GET " + PATH_NAME + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println(); // end HTTP header

    while(client.connected()) {
      if(client.available()){
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        Serial.print(c);
      }
    }

    // the server's disconnected, stop the client:
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {// if not connected:
    Serial.println("connection failed");
  }
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration*.0343)/2;
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(100);

  if(client.connect(HOST_NAME, HTTP_PORT)) {
    // if connected:
    Serial.println("Connected to server");
    // make a HTTP request:
    // send HTTP header
    if (distance < 20 and distance > 10) {
      client.println("GET " + ledRED + " HTTP/1.1");
    }
    else if (distance >= 20 and distance < 60) {
      client.println("GET " + ledYELLOW + " HTTP/1.1");
    }
    else if (distance >= 60) {
      client.println("GET " + ledGREEN + " HTTP/1.1");
    }
    else {
      client.println("GET " + ledBLINK + " HTTP/1.1");
    }
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println(); // end HTTP header

    // the server's disconnected, stop the client:
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {// if not connected:
    Serial.println("connection failed");
  }

}
