/**************************************************************************/
/*!
    @file     Adafruit_MPL3115A2.cpp
    @author   K.Townsend (Adafruit Industries)
    @license  BSD (see license.txt)

    Example for the MPL3115A2 barometric pressure sensor

    This is a library for the Adafruit MPL3115A2 breakout
    ----> https://www.adafruit.com/products/1893

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section  HISTORY

    v1.0 - First release
*/
/**************************************************************************/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_MPL3115A2.h>
#include "env.h"

// Builtin LED
const int LED = 2;

Adafruit_MPL3115A2 baro;

// WLAN client
WiFiClient wifiClient;

String url; 

// HTTP client
HTTPClient httpClient;

static void setupWlan() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }

  Serial.println("Connected to WLAN");
}


static void setupSerial() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  for (int i = 0; i < 10; i++) 
    Serial.println();

  Serial.println("Adafruit_MPL3115A2 test!");  
}

void setup() {
  setupSerial();
  setupWlan();
  pinMode(LED, OUTPUT);

  if (!baro.begin()) {
    Serial.println("Could not find sensor. Check wiring.");
    while(1);
  }

  // use to set sea level pressure for current location
  // this is needed for accurate altitude measurement
  // STD SLP = 1013.26 hPa
  baro.setSeaPressure(1013.26);
}

void loop() {
  float pressure = baro.getPressure();
  float altitude = baro.getAltitude();
  float temperature = baro.getTemperature();

  url = baseUrl;
  url += "&pressure=";
  url += pressure;
  url += "&temperature=";
  url += temperature;

  Serial.println("-----------------");
  Serial.print("pressure = "); Serial.print(pressure); Serial.println(" hPa");
  Serial.print("altitude = "); Serial.print(altitude); Serial.println(" m");
  Serial.print("temperature = "); Serial.print(temperature); Serial.println(" C");
  
  Serial.print("URL: ");
  Serial.println(url);
  digitalWrite(LED, 0);
  if (httpClient.begin(wifiClient, url)) {

    httpClient.addHeader("api-key", apiKey);
    // HTTP-Code der Response speichern
    int httpCode = httpClient.GET();
   

    if (httpCode > 0) {
      
      // Anfrage wurde gesendet und Server hat geantwortet
      // Info: Der HTTP-Code für 'OK' ist 200
      if (httpCode == HTTP_CODE_OK) {

        // Hier wurden die Daten vom Server empfangen

        // String vom Webseiteninhalt speichern
        String payload = httpClient.getString();

        // Hier kann mit dem Wert weitergearbeitet werden
       // ist aber nicht unbedingt notwendig
        Serial.println(payload);                        
      }
      
    } else {
      // Falls HTTP-Error
      Serial.print("HTTP-Error: ");
      Serial.print(httpClient.errorToString(httpCode).c_str());
      Serial.print(", ");
      Serial.println(httpCode);
      for (int i = 0; i < 3; i++) {
        digitalWrite(LED, 1);
        delay(100);
        digitalWrite(LED, 0);
        delay(100);
      }
    }

    // Wenn alles abgeschlossen ist, wird die Verbindung wieder beendet
    httpClient.end();
    
  } else {
    Serial.printf("HTTP connection could not be established");
  }
  digitalWrite(LED, 1);
  delay(60000);
}
