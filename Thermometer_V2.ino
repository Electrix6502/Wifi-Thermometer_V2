#include <SPI.h>                  // Bibliothek für SPI-Kommunikation
#include <WiFi.h>                 // Bibliothek für WiFi-Funktionen
#include <WiFiClient.h>           // Bibliothek für WiFi-Client
#include <WiFiAP.h>               // Bibliothek für WiFi-Access-Point (ESP32 als Access-Point)
#include <WebServer.h>            // Bibliothek für die Webserver-Funktionalität
#include <Update.h>               // Bibliothek für OTA-Firmware-Updates
#include <Preferences.h>          // Bibliothek für nicht-flüchtige Speicherung (NVS)
#include <nvs_flash.h>            // Low-Level-Bibliothek für NVS-Flash-Operationen
#include <FS.h>                   // Bibliothek für Dateisystem
#include <SPIFFS.h>               // Bibliothek für SPIFFS (Dateisystem auf Flash)

#include <OneWire.h>              // Bibliothek für OneWire-Protokoll (für DS18B20)
#include <DallasTemperature.h>    // Bibliothek für DS18B20-Temperatursensor
#include <TFT_eSPI.h>             // Bibliothek für TFT-LCD-Display

#include "Button2.h"              // Bibliothek für Button mit Debouncing
#include "esp_adc_cal.h"          // Bibliothek für ADC-Kalibrierung
#include "LargeFont.h"            // Eigene Schriftart

#include "time.h"                 // Bibliothek für Zeitfunktionen

// ESP32 RTC (Real-Time-Clock) für Schlafmodi
#include "esp32/rom/rtc.h"

#include <PubSubClient.h>         // Bibliothek für MQTT-Kommunikation

// Definieren von Konstanten und Pin-Nummern
#define AA_FONT_LARGE NotoSansBold36 // Schriftart für große Schrift

#define TFT_BLACK 0x0000        // Farbe für Schwarz (TFT)
#define ONE_WIRE_BUS 15         // Pin für OneWire-Bus

#define ADC_EN              14  // Pin für ADC (Analog-Digital-Wandler) aktivieren
#define ADC_PIN             34  // Pin für den Batteriemess-ADC
#define BUTTON_1            35  // Pin für den ersten Button
#define BUTTON_2            0   // Pin für den zweiten Button

#define uS_TO_S_FACTOR 1000000   /* Umrechnungsfaktor von Mikrosekunden zu Sekunden */
#define TIME_TO_SLEEP  360       /* Zeit in Sekunden, die der ESP32 im Schlafmodus verbringt */

OneWire oneWire(ONE_WIRE_BUS); // OneWire-Objekt erstellen, für Kommunikation mit DS18B20
DallasTemperature sensors(&oneWire); // DS18B20-Objekt für Temperaturmessung

TFT_eSPI tft = TFT_eSPI();      // TFT-Objekt erstellen, Pins in User_Setup.h definiert

WebServer server(80);           // Webserver auf Port 80 erstellen

Preferences pref;               // Preferences-Objekt für nicht-flüchtigen Speicher

WiFiClient espClient;           // WiFi-Client erstellen
PubSubClient client(espClient); // MQTT-Client erstellen

// Variablen für WiFi, MQTT und Konfiguration
String apssid = ""; // Access Point SSID
String appassword = ""; // Access Point Passwort

String ssid = ""; // WiFi SSID
String password = ""; // WiFi Passwort

String hostname = "WiFi-Thermo"; // Standard-Hostname

String sensor1 = "Sensor1"; // Name des ersten Sensors
String sensor2 = "Sensor2"; // Name des zweiten Sensors
String sensor3 = "ADC CH1"; // Name des ADC-Kanals

// RTC-gespeicherte Variablen (über den Sleep hinaus erhalten)
RTC_DATA_ATTR int ringbuffer[225];  // Ringpuffer für Sensordaten
RTC_DATA_ATTR int ringbuffer1[225]; // Ringpuffer für zweiten Sensor

RTC_DATA_ATTR byte pos = 0;         // Position im Ringpuffer
RTC_DATA_ATTR byte werte = 0;       // Anzahl der gespeicherten Werte

RTC_DATA_ATTR int tmax = 0;         // Maximalwert der gemessenen Temperatur
RTC_DATA_ATTR int tmin = 0;         // Minimalwert der gemessenen Temperatur

RTC_DATA_ATTR int sensCnt = 0;      // Anzahl der gefundenen DS18B20-Sensoren

RTC_DATA_ATTR byte device0[8] = {0}; // Adresse des ersten DS18B20-Sensors
RTC_DATA_ATTR byte device1[8] = {0}; // Adresse des zweiten DS18B20-Sensors

RTC_DATA_ATTR byte lines = 0; // Anzahl der Datenzeilen
RTC_DATA_ATTR byte files = 0; // Anzahl der Dateien im Dateisystem
RTC_DATA_ATTR byte oldfile = 0; // Zeigt auf die alte Datei

uint16_t v = 0; // Spannung (ADC-Wert)
int vref = 1100; // Referenzspannung für den ADC
String voltage = ""; // Spannung als String

int sec = 0; // Sekundenzähler
int store = 0; // Speicherkounter
int page = 0; // Aktuelle Display-Seite (welche Daten angezeigt werden)

unsigned long currmillis = 0; // Zeitstempel in Millisekunden

int color = 0; // Aktuelle Farbe
int color2 = 0; // Zweite Farbe

int colors[3] = {TFT_GREEN, TFT_RED, TFT_BLUE}; // Farbauswahl

int storeinterval = 0; // Speichervorgangsintervall

int config_done = 0; // Konfigurationsstatus

String fwversion = "Wifi-Thermometer_V2.0.0"; // Firmwareversion
String tempdata = ""; // Temporäre Daten

String header = ""; // HTTP-Header für Webserver

String filename = ""; // Dateiname für Speicherung

String mqttServer = ""; // MQTT-Server-Adresse
String mqttUser = ""; // MQTT-Benutzername
String mqttPass = ""; // MQTT-Passwort

byte mqttEnable = 0; // MQTT aktivieren (1 = Ja, 0 = Nein)

// Setup Funktion (einmaliger Aufruf beim Start)
void setup(void) 
{   
  // Pins für Buttons als Eingänge konfigurieren
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  
  // TFT initialisieren
  tft.init();
  tft.setRotation(1);  // Bildschirmausrichtung festlegen
  tft.fillScreen(TFT_BLACK); // Bildschirm schwarz füllen
  digitalWrite(TFT_BL, LOW); // TFT-Hintergrundbeleuchtung deaktivieren
  
  // DS18B20-Temperatursensor initialisieren
  sensors.begin();
  sensCnt = sensors.getDeviceCount(); // Anzahl der angeschlossenen Sensoren ermitteln
  
  // Prüfen, ob Sensoren angeschlossen sind und deren Adressen speichern
  if(sensCnt){
    if(device0[0] == 0){ // Falls die Adresse des ersten Sensors noch nicht gespeichert ist
      sensors.getAddress(device0, 0); // Adresse speichern
    }
    if(sensCnt > 1){ // Falls ein zweiter Sensor vorhanden ist
      if(device1[0] == 0){ // Falls die Adresse des zweiten Sensors noch nicht gespeichert ist
        sensors.getAddress(device1, 1); // Adresse speichern
      }
    }
    sensors.requestTemperatures(); // Temperatursensoren abfragen
  }

  // Wenn BUTTON_1 gedrückt ist, wird ein Factory-Reset durchgeführt
  if(!digitalRead(BUTTON_1)){
    digitalWrite(TFT_BL, HIGH);               // TFT Hintergrundbeleuchtung einschalten
    tft.fillScreen(TFT_BLACK);                // TFT-Bildschirm leeren
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);  // Textfarbe festlegen
    tft.setTextSize(2);                       // Textgröße festlegen
    tft.setCursor(0, 0, 2);                   // Cursor positionieren
    tft.println("Factory\r\nReset");          // Text anzeigen
    
    // Preferences (EEPROM-Ersatz) löschen
    pref.begin("config", false);  // Zugriff auf gespeicherte Konfigurationsdaten
    pref.clear();                 // Konfiguration zurücksetzen
    pref.end();                   // Beenden des NVS-Speichers

    // NVS-Flash-Speicher für WiFi-Daten und andere Konfigurationen löschen
    nvs_flash_erase();                        // NVS-Partition löschen
    nvs_flash_init();                         // NVS-Partition initialisieren
    delay(1000);                              // Eine Sekunde warten
    ESP.restart();                            // ESP32 neu starten
  }
  
  digitalWrite(TFT_BL, HIGH); // TFT-Hintergrundbeleuchtung einschalten
  
  Serial.begin(115200); // Serielle Kommunikation initialisieren
  
  prefRead(); yield(); // Konfiguration aus dem Speicher laden
  
  // WiFi-Konfiguration je nach Zustand laden oder neuen Access Point erstellen
  if(config_done){
    wifisetup(); yield();
  } else {
    apsetup(); yield();
  }
  
  serversetup(); // Webserver einrichten
  
  currmillis = millis(); // Aktuelle Zeit erfassen
  ringbufferAdd(); // Daten im Ringpuffer aktualisieren
  displayWifi(); // WiFi-Daten auf dem Display anzeigen

  // MQTT-Client konfigurieren, falls ein MQTT-Server angegeben wurde
  if(mqttServer.length() > 3){
    client.setServer(mqttServer.c_str(), 1883); // MQTT-Server und Port konfigurieren
    client.setCallback(callback); // Callback-Funktion für MQTT-Nachrichten
    mqttEnable = 1; // MQTT aktivieren
  }
}
 
// Hauptschleife
void loop(void) 
{ 
  server.handleClient(); // Webserver-Client-Verbindungen handhaben
  yield(); // Zeit für Hintergrundprozesse
  
  mqttLoop(); // MQTT-Nachrichten abarbeiten

  // Überprüfen, ob eine Sekunde vergangen ist
  if(millis() >= currmillis + 1000){
    currmillis = millis(); // Zeitstempel aktualisieren
    sec++; // Sekundenzähler erhöhen

    // Alle 3 Sekunden die Sensoren abfragen
    if(sec == 3){
      if(sensCnt){
        sensors.requestTemperatures(); // Temperatursensoren abfragen
      }
    }
    
    // Alle 5 Sekunden Daten speichern und die Anzeige aktualisieren
    if(sec >= 5){
      sec = 0; // Sekundenzähler zurücksetzen
      store++; // Speicherzähler erhöhen
      
      // Daten im Ringpuffer speichern, falls der Intervall erreicht ist
      if(store == storeinterval){
        store = 0; // Speicherzähler zurücksetzen
        ringbufferAdd(); // Daten hinzufügen
        mqttPub(); // Daten über MQTT veröffentlichen
      }

      // Daten auf dem Display anzeigen, je nach aktueller Seite
      if(page == 0){
        if(sensCnt > 1){
          page = 1; // Zur nächsten Seite wechseln
        } else {
          page = 2; // Zur Graph-Anzeige wechseln
        }
        displayData();  
      } else if(page == 1) {
        page = 2;
        displayData2(); 
      } else {
        page = 0;
        displayGraph(); // Temperatur-Graph anzeigen
      }   
    }
  }
}

// Funktion zum Hinzufügen von Sensordaten in den Ringpuffer
void ringbufferAdd(){
  if(sensCnt){
    ringbuffer[pos] = (int)(sensors.getTempC(device0) * 100.0); // Erste Sensordaten speichern
    if(sensCnt > 1){
      ringbuffer1[pos] = (int)(sensors.getTempC(device1) * 100.0); // Zweite Sensordaten speichern
    }
  }
  
  pos++; // Position im Ringpuffer erhöhen
  
  if(pos >= 224){ // Wenn die Position den Puffer überschreitet, auf 0 setzen
    pos = 0;
  }

  if(werte < 224){
    werte++; // Anzahl der gespeicherten Werte erhöhen
  }
    
  // Maximal- und Minimalwerte aus den gespeicherten Daten berechnen
  tmax = ringbuffer[0];
  tmin = ringbuffer[0];
  
  for(int i = 0; i < werte; i++){
    if(ringbuffer[i] > tmax){
      tmax = ringbuffer[i]; // Neuer Maximalwert
    }
    if(ringbuffer[i] < tmin){
      tmin = ringbuffer[i]; // Neuer Minimalwert
    }    
    if(ringbuffer1[i] > tmax){
      tmax = ringbuffer[i];
    }
    if(ringbuffer1[i] < tmin){
      tmin = ringbuffer[i];
    }   
  }
}
