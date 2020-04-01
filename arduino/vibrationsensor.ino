#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>


#define Vib A0

#define EDGE_DEVICE_ENDPOINT  "http://52.136.215.36:3000/"
#define WIFI_SSID  "startrek"
#define WIFI_PASS  "11111111"

WiFiClient wifiClient;
int val;
HTTPClient http;

void setup() {  
  Serial.begin(115200);
  delay(10);

  Serial.print("Connecting to the wifi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.setAutoReconnect (true);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());  
  pinMode(13, OUTPUT);  
}

void loop() {
  val = analogRead(Vib);
  if (val > 50) {
    Serial.print(val);

    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Sending http post. ");
      http.begin(EDGE_DEVICE_ENDPOINT);
      http.addHeader("Content-Type", "application/json");
      String message = "";
      message += "{\"sensor\" : \"esp8266\", \"value\": ";
      message += val;
      message += "}";
      int httpCode = http.POST(message);
      Serial.print(httpCode);
      http.end();    
    }
    
    Serial.print("\n");
    delay(200);  
  }
}
