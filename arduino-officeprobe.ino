/*
    This sketch sends data via HTTPS POST to op5 Monitors API.

*/

#include <ESP8266WiFi.h>
#include "DHT.h"

// WiFi Settings
const char* ssid     = "<oid>";
const char* password = "<pass>";

// Monitor Settings
const char* mon_host = "<monitor host>";
int httpsPort = 82;
  //encode op5 Monitor <user>:<pass> to base64 
const char* mon_token = "<base64>";

//Host and Service Settings
String host = "<host>";
String service = "<service>";

//Temp & Humidity Settings
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

String data;

void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

 
void loop() {
  delay(5000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();

  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.println(" *C ");



  delay(1000);

// Send to monitor
  Serial.println("");
  Serial.print("connecting to ");
  Serial.println(mon_host);
  Serial.println("");
  
  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;

  if (!client.connect(mon_host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  // URI to the op5 Monitor API command
  String url = "/api/command/PROCESS_SERVICE_CHECK_RESULT";

  Serial.print("Requesting URL: ");
  Serial.println(mon_host + httpsPort + url);

  // This will send the request to the server

  data = "{\"host_name\":\"" + host + "\",\"service_description\":\"" + service + "\",\"status_code\":\"0\",\"plugin_output\":\"from arduino\"}";
  Serial.print("Sending data: ");
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + mon_host + "\r\n" +
               "Authorization: Basic YXBpOmFwaQ==\r\n" +
               "User-Agent: Arduino/1.0\r\n" +
               "Content-Type: application/json;charset=utf-8\r\n" +
               "Content-Length: " + data.length() + "\r\n" +
               "\r\n" +
               data +
               "\r\n"
               "Connection: close\r\n\r\n");

  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");

    
  delay(300000);
}

