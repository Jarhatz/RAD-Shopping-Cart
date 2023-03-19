#include <ESP8266WiFi.h>

String ssid;
String pwd;

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  WiFi.mode(WIFI_STA);
  ssid, pwd = "";

  while (ssid == "" && pwd == "")
    getWiFiLogin();
}

void getWiFiLogin() {
  if (Serial.available()) {
    // Receives WiFi login details from Arduino Mega
    ssid = Serial.readStringUntil('\n');
    pwd = Serial.readStringUntil('\n');
    ssid.trim();
    pwd.trim();

    if (ssid == "RESET" && pwd == "RESET") {
      // Handles condition of resetting the chip
      ESP.reset();
    } else if (ssid.startsWith("LOGIN ")) {
      // Handles condition of connecting to WiFi
      WiFi.begin(ssid.substring(6), pwd);
      int timeout = 0; // MAX 30 sec
      while (WiFi.status() != WL_CONNECTED) {
        // Blink internal LED when attempting to connect
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);

        // Timeout and fail after 30 seconds
        timeout++;
        if (timeout > 30)
          return;
      }
    }
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Set internal LED to ON
    digitalWrite(LED_BUILTIN, LOW);

    // Transmit RSSI value when connected
    String rssi = "RSSI " + String(WiFi.RSSI() * -1);
    Serial.println(rssi);

    if (Serial.available()) {
      String input1 = Serial.readStringUntil('\n');
      String input2 = Serial.readStringUntil('\n');
      input1.trim();
      input2.trim();
      // Handles condition of resetting the WiFi chip
      if (input1 == "RESET" && input2 == "RESET")
        ESP.reset();
    }
  } else {
    // Set internal LED to OFF
    digitalWrite(LED_BUILTIN, HIGH);
    
    // Transmit DC value when disconnected
    Serial.println("DC");
    ESP.reset();
  }
  delay(1000);
}
