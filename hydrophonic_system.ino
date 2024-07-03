#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#define DHTTYPE DHT22
#define motor1 D0
#define motor2 D1
#define DHTPIN D2
#define sensorPower D3
#define sensorPin A0


DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "YOUR_SSID"; // Replace with your Wi-Fi network SSID
const char* password = "YOUR_PASSWORD"; // Replace with your Wi-Fi network password

const String baseURL = "http://api.thingspeak.com/update?api_key=APIKEY&field1="; // Replace APIKEY with your ThingSpeak API key

void setup() {
  Serial.begin(115200);
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(sensorPower, OUTPUT);
  dht.begin();
  digitalWrite(sensorPower, LOW);
  WiFi.disconnect();
  delay(2000);
  Serial.print("Start connection");
  WiFi.begin(ssid, password);
  while (!(WiFi.status() == WL_CONNECTED)) {
    delay(500);
    Serial.print("..");
  }
  Serial.println("Connected");
}

void loop() {
  float temperature = dht.readTemperature(); // Read temperature from the sensor
  float humidity = dht.readHumidity(); // Read humidity from the sensor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C  Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  delay(2000);
  Serial.print("Analog output: ");
  int val = readSensor();
  Serial.println(val);
  delay(1000);
  sendData(temperature, humidity, val);

  if (val > 20) {
    digitalWrite(motor1, HIGH);
    digitalWrite(motor2, HIGH);
  }
}

int readSensor() {
  digitalWrite(sensorPower, HIGH); // Turn the sensor ON
  delay(10); // Allow power to settle
  int val = analogRead(sensorPin); // Read the analog value from the sensor
  digitalWrite(sensorPower, LOW); // Turn the sensor OFF
  return val;
}

void sendData(float t, float h, int val) {
  WiFiClient client;
  HTTPClient http;
  String url = baseURL + String(t) + "&field2=" + String(h) + "&field3=" + String(val);
  http.begin(client, url);
  int responseCode = http.GET();
  String data = http.getString();
  Serial.println(data);
  http.end();
  delay(5000);
}
