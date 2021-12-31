#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <ArduinoOTA.h>
#include <AceButton.h>
using namespace ace_button;

#define GREENLED 14
#define REDLED 12

#define GREENSWITCH 5
#define REDSWITCH 4

// Create 2 ButtonConfigs. The System ButtonConfig is not used.
ButtonConfig config1;
ButtonConfig config2;

// Two buttons, explicitly bound to their respective ButtonConfig, instead
// of the default System ButtonConfig.
AceButton buttongreen(&config1);
AceButton buttonred(&config2);

AsyncWebServer server(80);
DNSServer dns;

void handleEventGreen(AceButton*, uint8_t, uint8_t);

void handleEventRed(AceButton*, uint8_t, uint8_t);

void setup()
{
  pinMode(GREENLED, OUTPUT); 
  pinMode(REDLED, OUTPUT); 

  digitalWrite(REDLED, HIGH);

  pinMode(GREENSWITCH, INPUT_PULLUP); 
  pinMode(REDSWITCH, INPUT_PULLUP); 


  buttongreen.init(GREENSWITCH);
  buttonred.init(REDSWITCH);

  config1.setEventHandler(handleEventGreen);
  config1.setFeature(ButtonConfig::kFeatureClick);
  config1.setFeature(ButtonConfig::kFeatureDoubleClick);
  config1.setFeature(ButtonConfig::kFeatureLongPress);
  config1.setFeature(ButtonConfig::kFeatureRepeatPress);

  config2.setEventHandler(handleEventRed);
  config2.setFeature(ButtonConfig::kFeatureClick);
  config2.setFeature(ButtonConfig::kFeatureDoubleClick);
  config2.setFeature(ButtonConfig::kFeatureLongPress);
  config2.setFeature(ButtonConfig::kFeatureRepeatPress);

  
  if (buttongreen.isPressedRaw()) {
    Serial.println(F("setup(): button 1 was pressed while booting"));
  }
  if (buttonred.isPressedRaw()) {
    Serial.println(F("setup(): button 2 was pressed while booting"));
  }

  
  WiFi.persistent(true);
  Serial.begin(76800);
  Serial.println("Starting setup...");

  AsyncWiFiManager wifiManager(&server, &dns);
  wifiManager.setDebugOutput(true);
  wifiManager.autoConnect("switchtoy");
  WiFi.hostname("switchtoy");
  

  Serial.println("Wifi finished...");
  digitalWrite(REDLED, LOW);
  digitalWrite(GREENLED, HIGH);

  ArduinoOTA.setPassword("switchtoyy");
  ArduinoOTA.onStart([]()
                     {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type); });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed"); });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
   
}


void loop()
{
  ArduinoOTA.handle();
  buttongreen.check();
  buttonred.check();


}


void handleEventGreen(AceButton* button, uint8_t eventType, uint8_t buttonState) {

  // Print out a message for all events, for both buttons.
  Serial.print(F("handleEvent1(): pin: "));
  Serial.print(button->getPin());
  Serial.print(F("; eventType: "));
  Serial.print(eventType);
  Serial.print(F("; buttonState: "));
  Serial.println(buttonState);

  // Control the LED only for the Pressed and Released events of Button 1.
  // Notice that if the MCU is rebooted while the button is pressed down, no
  // event is triggered and the LED remains off.
  switch (eventType) {
    case AceButton::kEventClicked:
      digitalWrite(GREENLED, 1);
      break;
    case AceButton::kEventLongPressed:
      digitalWrite(GREENLED, 0);
      break;
  }
}



void handleEventRed(AceButton* button, uint8_t eventType, uint8_t buttonState) {

  // Print out a message for all events, for both buttons.
  Serial.print(F("handleEvent1(): pin: "));
  Serial.print(button->getPin());
  Serial.print(F("; eventType: "));
  Serial.print(eventType);
  Serial.print(F("; buttonState: "));
  Serial.println(buttonState);

  // Control the LED only for the Pressed and Released events of Button 1.
  // Notice that if the MCU is rebooted while the button is pressed down, no
  // event is triggered and the LED remains off.
  switch (eventType) {
    case AceButton::kEventClicked:
      digitalWrite(REDLED, 1);
      break;
    case AceButton::kEventLongPressed:
      digitalWrite(REDLED, 0);
      break;
  }
}
