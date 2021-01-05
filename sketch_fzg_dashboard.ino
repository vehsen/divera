// Projekt Fahrzeug-DashBoard 2021
// Spiegelt den Status der Fahrzeuge aus DIVERA247 auf einen adressierbaren LED-Strip.
// DIVERA 24/7 RESTful Webservice: https://api.divera247.com/?urls.primaryName=api%2Fv2%2Fpull#/
//
// Anzahl LEDS = Anzahl Fahrzeuge = Größe des JSON Arrays!!
// Die Reihenfolge der Fahrzeuge ist wie in DIVERA eingestellt!!


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>
#include <FastLED.h>

// ###########################################################################

// FASTLED
#define NUM_LEDS 28 // = Anzahl LEDs & FAHRZEUGE
#define DATA_PIN 5
#define LED_TYPE WS2812B
#define RGB_ORDER GRB
#define BRIGHTNESS 100
CRGB leds[NUM_LEDS];

// fingerprint divera247.com (valid until 16.12.2021)
const uint8_t fingerprint[20] = {0xd1, 0x78, 0x49, 0xbe, 0x35, 0x3d, 0xd6, 0xba, 0x56, 0x59, 0x1a, 0x0e, 0x1c, 0xd5, 0x9f, 0xda, 0x74, 0x71, 0x38, 0x3b};

// DIVERA247 System-API-Accesskey  (apiKey)
const char statusURI[] = "https://www.divera247.com/api/v2/pull/vehicle-status?accesskey=YOUR-ACCESS-KEY-HERE";


// JSON sizing: https://arduinojson.org/v6/assistant/
DynamicJsonDocument doc(8192);

// WLAN
ESP8266WiFiMulti wifiMulti;

// WEBSERVER
ESP8266WebServer webserver(80);

// FUNCTION PROTOTYPES
void rssi_to_stip();
void handleRoot();
void handleDebug();
void handleNotFound();

// GLOBAL VARIABLES
unsigned long previousMillis = 0;
const long interval = 60000; // Abfrageinterval/Aktualisierungen in Millisekunden: 60000ms = 60sek = 1min
int requests = 0;
int httpCode = 0;
//int dataSize = 0;
String previewstring = "-";
String errorstring = "none";
boolean controllerOnTop = false;

//###########################################################################
void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // port as output

  // SERIAL
  Serial.begin(115200);
  delay(10);
  Serial.println();


  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[RUN] %d...\n", t);
    Serial.flush();
    digitalWrite(LED_BUILTIN, HIGH); //onboard LED an
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW); //onboard LED aus
  }

  // FASTLED
  FastLED.addLeds<LED_TYPE, DATA_PIN, RGB_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  // LED TEST
  Serial.println(F("[LED] testing all leds with blue..."));
  FastLED.clear();
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    leds[dot] = CRGB::DarkBlue;
    FastLED.show();
    delay(33);
  }
  delay(2470);  //time to check the leds
  FastLED.clear();

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP("freifunk", "");
  wifiMulti.addAP("feierwehr", "loerres94");
//  wifiMulti.addAP("name", "password");

  // Wait for connection
  Serial.print("[WLAN] connecting ");
  int dot = 0;
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(F("."));
    leds[dot] = CRGB::Blue;
    FastLED.show();
    delay(247);
    leds[dot] = CRGB::Black;
    if(dot==NUM_LEDS){
      dot = 0;
    }else {
      dot++;
    }
    
  }
  Serial.println(F(" Done."));
  
  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.print(F("[WLAN] connected to:  "));
    Serial.println(WiFi.SSID());
    Serial.print(F("[WLAN] IP address:  "));
    Serial.println(WiFi.localIP());
    Serial.print(F("[WLAN] RSSI:  ")); //Received Signal Strength Indicator in dBm  
    Serial.println(WiFi.RSSI());

    // output the rssi on the led-strip
    for(int i=0; i<4;i++){
      rssi_to_stip();
      delay(1000);
    }
    webserver.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
    webserver.on("/debug", handleDebug);
    webserver.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
  
    webserver.begin();                           // Actually start the server
    Serial.println(F("[WEB] Server started"));
    Serial.println(ESP.getFreeHeap());
  }


}
// ###########################################################################
void loop() {
  unsigned long currentMillis = millis();
  
  webserver.handleClient(); // Listen for HTTP requests from clients
  
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(F("[WLAN] NOT connected!"));
    delay(1000);
  }

  // INTERVAL LOOP
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    previewstring = "";
    
    // SSL certificate shit
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setFingerprint(fingerprint);
    
    HTTPClient https;
    
    // send Request
    https.useHTTP10(true);
    https.begin(*client, statusURI);
    httpCode = https.GET();
    
    // httpCode negative on error
    if (httpCode > 0) {
     Serial.printf("[HTTPS] GET... Code: %d\n", httpCode);
  
     if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      DeserializationError err = deserializeJson(doc, https.getStream());
      if (err) {
       Serial.print(F("deserializeJson() failed with code "));
       Serial.println(err.c_str());
       errorstring += "\n deserializeJson() failed with code: ";
       errorstring += err.c_str();
       leds[0] = CRGB::DarkOrange;
          FastLED.show();
      }
     }
        } else {
          Serial.print(httpCode);
          Serial.print(F("[HTTPS] GET... failed, error:")); 
          Serial.println(https.errorToString(httpCode).c_str());
          errorstring += "\n http error: ";
          errorstring += https.errorToString(httpCode).c_str();
          leds[0] = CRGB::Crimson; //red
          FastLED.show();
        }
    
    // Disconnect
    https.end();
    
    // Read values
    JsonArray data = doc["data"];
    
    if(data.size() != NUM_LEDS){
      Serial.println(F("different number of values"));
      //dataSize = data.size();
      Serial.println(data.size()); //Size = NUM_Vehicels = NUM_LEDS
      Serial.println(NUM_LEDS);
    }
  
    // O.U.T.P.U.T - START
    if (doc["success"] == true)
    {
      int j = NUM_LEDS -1;
      
      if(controllerOnTop == true){
        j = 0;
      }

      requests++;
    
      // Output 
      FastLED.clear();

      for(int i=0; i < NUM_LEDS; i++){
        int status = data[i]["fmsstatus"].as<int>();

        // Output Status to LED with DIVERA247 status color code
        switch (status) {
          case 1:
            leds[j].setRGB(85, 179, 0);
			       previewstring += "<span class=\"dot\" style=\"background-color:rgb(85,179,0);\"></span>";
            break;
          case 2:
            leds[j].setRGB(25, 130, 21);
			       previewstring += "<span class=\"dot\" style=\"background-color:rgb(25,130,21);\"></span>";
            break;
          case 3:
            leds[j].setRGB(255, 70, 12);
			       previewstring += "<span class=\"dot\" style=\"background-color:rgb(255,70,12);\"></span>";
            break;
          case 4:
            leds[j].setRGB(214, 0, 0);
			       previewstring += "<span class=\"dot\" style=\"background-color:rgb(214,0,0);\"></span>";
            break;
          case 5:
            leds[j].setRGB(239, 178, 0);
			       previewstring += "<span class=\"dot\" style=\"background-color:rgb(239,178,0);\"></span>";
            break;
          case 6:
            //leds[j].setRGB(62, 62, 62);
            leds[j].setRGB(255, 0, 0);
			       previewstring += "<span class=\"dot\" style=\"background-color:rgb(255,0,0);\"></span>";
            break;
          case 7:
            leds[j].setRGB(0, 135, 230);
			       previewstring += "<span class=\"dot\" style=\"background-color:rgb(0,135,230);\"></span>";
            break;
          case 8:
            leds[j].setRGB(0, 56, 169);
			       previewstring += "<span class=\"dot\" style=\"background-color:rgb(0,56,169);\"></span>";
            break;
          case 9:
            leds[j].setRGB(0, 26, 122);
			       previewstring += "<span class=\"dot\" style=\"background-color:rgb(0,26,122);\"></span>";
            break;
          case 0:
            leds[j].setRGB(228, 0, 255);
			       previewstring += "<span class=\"dot\" style=\"background-color:rgb(228,0,255);\"></span>";
            break;
          default:
            leds[j].setRGB(0, 0, 0);
			       previewstring += "<span class=\"dot\" style=\"background-color:rgb(0,0,0);\"></span>";
            break;
          }

         previewstring += "[";
         previewstring += data[i]["fmsstatus"].as<int>();
         previewstring += "]  ";
         previewstring += data[i]["name"].as<String>();
         previewstring += "  =  LED[";
         previewstring += j;
         previewstring += "]<br>";
        
		    if(controllerOnTop){
          j++;
		    }else{
          j--;
		    }
      }
      FastLED.show();

    }
    // O.U.T.P.U.T - END
  }  
 }
 
// ###########################################################################
// F U N C T I O N - D E C L A R A T I O N S

void rssi_to_stip() {
  // map RSSI (dBm) to led count (NUM_LEDS)
  // assume -100 dBm poor and -40 dBm good  
  // strip divided in four color ranges representing the quality
  int rssi_val = map(WiFi.RSSI(), -100, -40, 1, NUM_LEDS);
  int rssi_good = 3*(NUM_LEDS/4);
  int rssi_okay = 2*(NUM_LEDS/4);
  int rssi_weak = 1*(NUM_LEDS/4);

  // DEBUG
  Serial.print(F("[WLAN] RSSI: "));
  Serial.println(WiFi.RSSI());
  Serial.print(F("[WLAN] RSSI mapped: "));
  Serial.print(rssi_val);
  Serial.print(F("/"));
  Serial.println(NUM_LEDS);

  FastLED.clear();

  // output scale
  for(int i=0; i < rssi_val; i++){
    if (i < rssi_weak){
      leds[i].setRGB(255, 0, 0);
    } else if (i >= rssi_weak && i < rssi_okay){
      leds[i].setRGB(255, 192, 0);
    } else if (i >= rssi_okay && i < rssi_good){
      leds[i].setRGB(0, 208, 80);
    } else if (i >= rssi_good){
      leds[i].setRGB(25, 130, 21);
    }
  }

    FastLED.show();
}


// W E B S E R V E R

void handleRoot() {
  char content[740];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(content, 740,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5' charset='UTF-8'/>\
    <title>Fahrzeug Dashboard</title>\
    <style>\
      body { background-color:#f7f0ea; font-family:Arial, Helvetica, Sans-Serif; color:#003399; }\
      h1, h2, h3, h4, h5, h6 { margin-bottom:0; }\
      .dot { height:0.9em; width:0.9em; border-radius:50%%; display:inline-block; }\
    </style>\
  </head>\
  <body>\
    <h1>Fahrzeug Dashboard</h1>\
    <div><h3>Settings:</h3>\
    Interval: %d <br>\
    LEDs: %d <br>\
    Brightness: %d <br>\
    ControllerOnTop: %d</div>\
    <div><h3>Statistics:</h3>\
    Uptime: %02d:%02d:%02d <br>\
    Free Heap: %d <br>\
    Heap Fragmentation: %d %%<br>\
    WLAN RSSI: %d <br>\
    Requests: %d <br>\
    Last httpCode: %d </div>\
    <div><h3>Preview:</h3>\
",

           interval/1000, NUM_LEDS, BRIGHTNESS, controllerOnTop, hr, min % 60, sec % 60, ESP.getFreeHeap(), ESP.getHeapFragmentation(), WiFi.RSSI(), requests, httpCode
          );
  webserver.send(200, "text/html", content + previewstring + "</div></body></html>");
}

void handleDebug(){
  webserver.send(200, "text/plain", errorstring);
}

void handleNotFound(){
  webserver.send(404, "text/plain", "404: Not found");
}