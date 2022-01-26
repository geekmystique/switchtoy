#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <ArduinoOTA.h>
#include <AceButton.h>//https://github.com/bxparks/AceButton
#include <jled.h>//https://github.com/jandelgado/jled
#include <WS2812FX.h>//https://github.com/kitesurfer1404/WS2812FX

using namespace ace_button;

#define GREENLEDPIN 14
#define REDLEDPIN 12
#define GREENSWITCHPIN 5
#define REDSWITCHPIN 4
#define NEOXPIXELPIN 13
#define NEOPIXELNUM 2


auto greenled = JLed(GREENLEDPIN);
auto redled = JLed(REDLEDPIN);

ButtonConfig config1;
ButtonConfig config2;

AceButton buttongreen(&config1);
AceButton buttonred(&config2);

void handleEventGreen(AceButton*, uint8_t, uint8_t);
void handleEventRed(AceButton*, uint8_t, uint8_t);

WS2812FX pixels(NEOPIXELNUM, NEOXPIXELPIN, NEO_GRB + NEO_KHZ400);

AsyncWebServer server(80);
DNSServer dns;


void setup()
{
  WiFi.persistent(true); //ensure wifi credentials are saved

  Serial.begin(76800);
  Serial.println("Starting setup...");

  //turn on red led until setup is finished
  redled.On();
  redled.Update();

  //acebutton setup
  pinMode(GREENSWITCHPIN, INPUT_PULLUP); 
  pinMode(REDSWITCHPIN, INPUT_PULLUP);
  buttongreen.init(GREENSWITCHPIN);
  buttonred.init(REDSWITCHPIN);

  pixels.init();
  pixels.start();
  pixels.setSegment(0,0,0,FX_MODE_STATIC,RED);
  pixels.setSegment(1,1,1,FX_MODE_STATIC,RED);
  pixels.service();

  config1.setEventHandler(handleEventGreen);
  config1.setFeature(ButtonConfig::kFeatureClick);

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

  //wifimanager to handle initial SoftAP wifi setup
  AsyncWiFiManager wifiManager(&server, &dns);
  wifiManager.setDebugOutput(true);
  wifiManager.autoConnect("switchtoy");
  WiFi.hostname("switchtoy");
  
  Serial.println("Wifi finished...");
  redled.Off();
  greenled.On();
  pixels.setSegment(0,0,0,FX_MODE_BREATH,GREEN);
  pixels.setSegment(1,1,1,FX_MODE_STATIC,BLACK);

  

  //ota init
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

void loop(){
  ArduinoOTA.handle();

  //AceButton
  buttongreen.check();
  buttonred.check();
  
  //JLed
  greenled.Update();
  redled.Update();

  //Neo pixel effects
  pixels.service();
}

void handleEventGreen(AceButton* button, uint8_t eventType, uint8_t buttonState) {

  //Simple on off setup (with fade)

  switch (eventType) {
    case AceButton::kEventClicked:
    if(greenled.IsRunning() || greenled.Get() > 0)
    {
      pixels.setSegment(0,0,0,FX_MODE_STATIC,BLACK);
      greenled.FadeOff(200);
    }else{
      pixels.stop();
      pixels.resetSegments();
      pixels.setSegment(0,0,0,FX_MODE_BREATH,GREEN);
      greenled.FadeOn(200);
    }
    break;
  }
}
void handleEventRed(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  switch (eventType) {

    //Example with 1 press, long press and double press logic
    case AceButton::kEventClicked:
    if(redled.IsRunning() || redled.Get() > 0)
    {
      redled.Stop();
      pixels.setSegment(1,1,1,FX_MODE_STATIC,BLACK);
    }else{
      redled.Blink(500,500).Forever();
      pixels.setSegment(1,1,1,FX_MODE_COLOR_SWEEP_RANDOM);
    }
    break;
    case AceButton::kEventLongPressed://
    redled.Breathe(3000);
    pixels.stop();
    pixels.resetSegments();
    pixels.setSegment(0,0,1,FX_MODE_STATIC,WHITE);
    pixels.setBrightness(255);
    pixels.start();
    break;
    case AceButton::kEventDoubleClicked://fire
    redled.Candle(5, 100).Forever();
    pixels.stop();
    pixels.resetSegments();
    pixels.setSegment(0,0,1,FX_MODE_FIRE_FLICKER_SOFT,RED,200);
    pixels.start();
    break;
  }
}
