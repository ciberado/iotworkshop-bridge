#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>


#define Vib A0

WiFiClient wifiClient;
int val;
HTTPClient http;

void setup() {  
  Serial.begin(115200);
  delay(10);

  Serial.print("Connecting to the wifi");
  WiFi.begin("startrek", "11111111");
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
      http.begin("http://52.136.215.36:3000/");
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
