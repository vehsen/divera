
diveraLED
===========

Bildet den Fahrzeugstatus aus [**DIVERA 24/7**](https://www.divera247.com) mittels Mikrocontroller auf adressierbaren LED-Streifen ab.

Erhöht die Sichtbarkeit und das Bewusstsein für den Fahrzeugstatus. 

Ruft die API ab, verarbeitet das JSON und spiegelt den Fahrzeugstatus als farbige LED wider.

Die Sortierung ist analog der Einstellungen in DIVERA 24/7 [Setup>Sortierung](https://www.divera247.com/localmanagement/sorting.html).

<br>

:red_circle: :fire_engine: LF

:green_circle: :minibus: MTW

:green_circle: :red_car: PKW

##### weitere Funktionen
- mehrere Access-Points konfigurierbar
- Anzeige der WLAN-Signalstärke beim Start
- Web-Interface zur Diagnose

## Datenquelle
### DIVERA 24/7 RESTful Webservice
[https://api.divera247.com](https://api.divera247.com/?urls.primaryName=api%2Fv2%2Fpull#/Daten%2F%20Informationen/get_api_v2_pull_vehicle_status)

![DIVERA247 Logo](https://www.divera247.com/images/divera247.svg)

---

#### Genutzte Hardware
- NodeMCU - ESP8266 Entwicklungsboard
- WS2812b - adressierbarer LED Streifen [FastLED reference](https://github.com/FastLED/FastLED/wiki/Chipset-reference)
- _USB_ Netzteil - [Wiring-leds#a-word-on-power](https://github.com/FastLED/FastLED/wiki/Wiring-leds#a-word-on-power)
- 500 Ohm Widerstand


#### Genutzte Libraries
- [ESP8266WiFi](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi)
- [ESP8266WiFiMulti](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFiMulti.h)
- [ESP8266HTTPClient](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h)
- [WiFiClientSecureBearSSL](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClientSecureBearSSL.h)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [FastLED](https://github.com/FastLED/FastLED)


---

## Dinge, die **du** anpassen musst:
##### FastLED Einstellungen
Siehe [FastLED/Basic-Usage](https://github.com/FastLED/FastLED/wiki/Basic-usage)
```
#define NUM_LEDS 28
#define DATA_PIN 5
#define LED_TYPE WS2812B
#define RGB_ORDER GRB
#define BRIGHTNESS 100
```

##### DIVERA247 Einstellungen
[DIVERA247 Access-Key](https://www.divera247.com/localmanagement/index-settings-api.html)
```
const char statusURI[] = "https://www.divera247.com/api/v2/pull/vehicle-status?accesskey=YOUR-ACCESS-KEY-HERE";
```

##### ArduinoJSON Einstellungen
Benutze den [ArduinoJson Assistant](https://arduinojson.org/v6/assistant/).
Benutze im zweiten Schritt "JSON" als InPut den [Response body](https://api.divera247.com/?urls.primaryName=api%2Fv2%2Fpull#/Daten%2F%20Informationen/get_api_v2_pull_vehicle_status) aus einem Test im Swagger.
Passe den Quellcode analog der empfohlenen Größe _Total (recommended)_ im dritten Schritt "Size" an.
```
DynamicJsonDocument doc(1234);
```
##### Abfrageinterval
Abfrageinterval in Millisekunden: 60000ms = 60sek = 1min
```
const long interval = 60000;
```


##### WiFiMulti Einstellungen
Füge deine WLAN-Access-Point(s) hinzu:
```
wifiMulti.addAP("freifunk", "");
wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1");
wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");
```

---
#### Programmablauf
**Setup**
1. Initialisierung der seriellen Schnittstelle (für Debugging)
2. LED-Test (alle blau)
3. Verbindungsherstellung zum WLAN-Access-Point
4. Anzeige der WLAN-Signalstärke auf dem LED-Streifen :red_circle:schwack :orange_circle:okay :green_circle:gut
5. Starten des Web-Server

**Loop**
1. Handling der Webseite
2. Abfrage der DIVERA-API (nach eingestelltem Intervall) 
2. Ausgabe der Fahrzeugstatus


---
#### Debugging
**Seriell (Beispielausgabe)**
```
[RUN] 4...
[RUN] 3...
[RUN] 2...
[RUN] 1...
[LED] testing all leds with blue...
[WLAN] connecting  Done.
[WLAN] connected to:  Access-Point-Name
[WLAN] IP address:  192.168.179.2
[WLAN] RSSI:  -47
[WEB] Server started
[HTTPS] GET... Code: 200
```
**Webseite**
http://ip.adresse.des.mikrokontrollers

[Vorschau](diveraLED-web-preview.png)

---

#### To Do Liste: 
- [ ] Anpassbare Ausgabereihenfolge
- [ ] Frizing Schema
- [ ] Wiki anlegen
- [ ] Error-Handling verbessern
- [ ] OTA Updates - [ArduinoOTA](https://github.com/jandrassy/ArduinoOTA)
- [ ] Beispielbilder einbinden
