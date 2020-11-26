
diveraLED
===========

Pulls the API, deserializes the json and mirrors the vehicle state to an addressable led strip.

Increases the visibility and awareness of the vehicle statuses. <br>

:red_circle: :fire_engine:Status 6

:green_circle: :minibus: Status 2

:green_circle: :red_car: Status 2


##### Features
- Updates over the air (OTA)
- using multiple Access-Points
- outputs RSSI _(WiFi strength)_ at startup :red_circle:weak :orange_circle:okay :green_circle:good
- serial debugging

## Data source
### DIVERA 24/7 RESTful Webservice
[https://api.divera247.com](https://api.divera247.com/?urls.primaryName=api%2Fv2%2Fpull#/Daten%2F%20Informationen/get_api_v2_pull_vehicle_status)

![DIVERA247 Logo](https://www.divera247.com/images/divera247.svg)

---

#### Used libraries
- [ESP8266WiFi](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi)
- [ESP8266WiFiMulti](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFiMulti.h)
- [ESP8266HTTPClient](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h)
- [WiFiClientSecureBearSSL](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClientSecureBearSSL.h)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [FastLED](https://github.com/FastLED/FastLED)
- [ArduinoOTA](https://github.com/jandrassy/ArduinoOTA)


#### Used hardware
- NodeMCU - ESP8266 development board
- WS2812b - addressable LED strip [FastLED reference](https://github.com/FastLED/FastLED/wiki/Chipset-reference)
- _USB_ Power Supply - [Wiring-leds#a-word-on-power](https://github.com/FastLED/FastLED/wiki/Wiring-leds#a-word-on-power)
- 500 Ohm resistor
- capacitor //todo!

---

## Things **you** need to change
##### FastLED settings
See [FastLED/Basic-Usage](https://github.com/FastLED/FastLED/wiki/Basic-usage)
```
#define NUM_LEDS 21
#define DATA_PIN 5
#define LED_TYPE WS2812B
#define RGB_ORDER GRB
#define BRIGHTNESS 100
```
##### DIVERA247 settings
[DIVERA247 Access-Key](https://www.divera247.com/localmanagement/index-settings-api.html)
```
accesskey = "0123456789-abcdefghijklmnopqrstuvwxyz";
```

##### ArduinoJSON settings
See [JSON sizing](https://arduinojson.org/v6/assistant/). Use your [Response body](https://api.divera247.com/?urls.primaryName=api%2Fv2%2Fpull#/Daten%2F%20Informationen/get_api_v2_pull_vehicle_status)
```
const int capacity = 21*JSON_ARRAY_SIZE(0) + JSON_ARRAY_SIZE(21) + JSON_OBJECT_SIZE(2) + 21*JSON_OBJECT_SIZE(11) + 1010;
```
##### WiFiMulti settings
Add your Access-Point(s)
```
wifiMulti.addAP("freifunk", "");
wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1");
wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");
```

---

#### Tasks
- [ ] custom vehicle order (array sorting)
- [ ] Frizing
- [ ] Wiki
- [ ] german translation
- [ ] improve error handling
