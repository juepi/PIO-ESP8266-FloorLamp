/*
 * ESP8266 FloorLamp
 * ==================
 * See README.md for Details!
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <FastLED.h>
#include "hardware-config.h"
#include "generic-config.h"
#include "wifi-config.h"
#include "mqtt-ota-config.h"
#include "common-functions.h"
#include "setup-functions.h"
#include "fastled-config.h"

// Setup WiFi instance
WiFiClient WIFI_CLTNAME;

// Setup PubSub Client instance
PubSubClient mqttClt(MQTT_BROKER, 1883, MqttCallback, WIFI_CLTNAME);

/*
/ Functions
*/

// Function to subscribe to MQTT topics
bool MqttSubscribe(const char *Topic)
{
  if (mqttClt.subscribe(Topic))
  {
    DEBUG_PRINTLN("Subscribed to " + String(Topic));
    SubscribedTopics++;
    mqttClt.loop();
    return true;
  }
  else
  {
    DEBUG_PRINTLN("Failed to subscribe to " + String(Topic));
    delay(100);
    return false;
  }
}

// Function to connect to MQTT Broker and subscribe to Topics
bool ConnectToBroker()
{
  // Reset subscribed/received Topics counters
  SubscribedTopics = 0;
  ReceivedTopics = 0;
  bool RetVal = false;
  int ConnAttempt = 0;
  // Try to connect x times, then return error
  while (ConnAttempt < MAXCONNATTEMPTS)
  {
    DEBUG_PRINT("Connecting to MQTT broker..");
    // Attempt to connect
    if (mqttClt.connect(MQTT_CLTNAME))
    {
      DEBUG_PRINTLN("connected");
      RetVal = true;

// Subscribe to Topics
#ifdef OTA_UPDATE
      MqttSubscribe(ota_topic);
      MqttSubscribe(otaInProgress_topic);
#endif //OTA_UPDATE
      MqttSubscribe(sim_topic);
      MqttSubscribe(brightness_topic);
      MqttSubscribe(enable_topic);
      MqttSubscribe(AutoSimSwitch_topic);
      MqttSubscribe(fps_topic);
      delay(200);
      break;
    }
    else
    {
      DEBUG_PRINTLN("failed, rc=" + String(mqttClt.state()));
      DEBUG_PRINTLN("Sleeping 2 seconds..");
      delay(2000);
      ConnAttempt++;
    }
  }
  return RetVal;
}

/*
 * Setup
 * ========================================================================
 */
void setup()
{
// start serial port and digital Outputs
#ifdef SERIAL_OUT
  Serial.begin(BAUD_RATE);
#endif
  DEBUG_PRINTLN();
  DEBUG_PRINTLN(FIRMWARE_NAME);
  DEBUG_PRINTLN(FIRMWARE_VERSION);
#ifdef ONBOARD_LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LEDOFF);
#endif

  // Startup WiFi
  wifi_setup();
  // Setup MQTT Connection to broker and subscribe to topic
  if (ConnectToBroker())
  {
    // New connection to broker, fetch topics
    // ATTN: will run endlessly if subscribed topics
    // does not have retained messages and no one posts a message
    DEBUG_PRINTLN("Waiting for topics..");
    while (ReceivedTopics < SubscribedTopics)
    {
      DEBUG_PRINTLN("SETUP: Subscribed / Received Topics: " + String(SubscribedTopics) + "/" + String(ReceivedTopics));
      mqttClt.loop();
#ifdef ONBOARD_LED
      ToggleLed(LED, 100, 2);
#else
      delay(100);
#endif
    }
    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("All topics received.");
  }
  else
  {
    DEBUG_PRINTLN("Unable to connect to MQTT broker.");
#ifdef ONBOARD_LED
    ToggleLed(LED, 100, 40);
#endif
#ifdef DEEP_SLEEP
    ESP.deepSleep(DS_DURATION_MIN * 60000000);
    delay(3000);
#else
    ESP.reset();
#endif
    delay(1000);
  }

  // Setup OTA
#ifdef OTA_UPDATE
  ota_setup();
#endif

  // Setup LED Strip
  DEBUG_PRINTLN("Setup LED strip..");
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  // BeatWave Setup
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;

  // Fire Setup
  FirePal = HeatColors_p;
  BlueFirePal = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);

  // Init random number generator
  randomSeed(analogRead(0));

#ifdef ONBOARD_LED
  // Signal setup finished
  ToggleLed(LED, 200, 6);
#endif
}

/*
 * Main Loop
 * ========================================================================
 */
void loop()
{
  // Check connection to MQTT broker and update topics
  if (!mqttClt.connected())
  {
    if (ConnectToBroker())
    {
      // New connection to broker, fetch topics
      // ATTN: will run endlessly if subscribed topics
      // does not have retained messages and no one posts a message
      DEBUG_PRINTLN("Waiting for topics..");
      while (ReceivedTopics < SubscribedTopics)
      {
        DEBUG_PRINTLN("MAIN LOOP: Subscribed / Received Topics: " + String(SubscribedTopics) + "/" + String(ReceivedTopics));
        mqttClt.loop();
#ifdef ONBOARD_LED
        ToggleLed(LED, 100, 2);
#else
        delay(100);
#endif
      }
      DEBUG_PRINTLN("");
      DEBUG_PRINTLN("All topics received.");
    }
    else
    {
      DEBUG_PRINTLN("Unable to connect to MQTT broker.");
#ifdef ONBOARD_LED
      ToggleLed(LED, 100, 40);
#endif
#ifdef DEEP_SLEEP
      ESP.deepSleep(DS_DURATION_MIN * 60000000);
      delay(3000);
#else
      ESP.reset();
#endif
    }
  }
  else
  {
    mqttClt.loop();
  }

#ifdef OTA_UPDATE
  // If OTA Firmware Update is requested,
  // only loop through OTA function until finished (or reset by MQTT)
  if (OTAupdate)
  {
    if (OtaInProgress && !OtaIPsetBySketch)
    {
      DEBUG_PRINTLN("OTA firmware update successful, resuming normal operation..");
      mqttClt.publish(otaStatus_topic, String(UPDATEOK).c_str(), true);
      mqttClt.publish(ota_topic, String("off").c_str(), true);
      mqttClt.publish(otaInProgress_topic, String("off").c_str(), true);
      OTAupdate = false;
      OtaInProgress = false;
      OtaIPsetBySketch = true;
      SentOtaIPtrue = false;
      SentUpdateRequested = false;
      return;
    }
    if (!SentUpdateRequested)
    {
      mqttClt.publish(otaStatus_topic, String(UPDATEREQ).c_str(), true);
      SentUpdateRequested = true;
    }
    DEBUG_PRINTLN("OTA firmware update requested, waiting for upload..");
#ifdef ONBOARD_LED
    // Signal OTA update requested
    ToggleLed(LED, 100, 10);
#endif
    //set MQTT reminder that OTA update was executed
    if (!SentOtaIPtrue)
    {
      DEBUG_PRINTLN("Setting MQTT OTA-update reminder flag on broker..");
      mqttClt.publish(otaInProgress_topic, String("on").c_str(), true);
      OtaInProgress = true;
      SentOtaIPtrue = true;
      OtaIPsetBySketch = true;
      delay(100);
    }
    //call OTA function to receive upload
    ArduinoOTA.handle();
    return;
  }
  else
  {
    if (SentUpdateRequested)
    {
      DEBUG_PRINTLN("OTA firmware update cancelled by MQTT, resuming normal operation..");
      mqttClt.publish(otaStatus_topic, String(UPDATECANC).c_str(), true);
      mqttClt.publish(otaInProgress_topic, String("off").c_str(), true);
      OtaInProgress = false;
      OtaIPsetBySketch = true;
      SentOtaIPtrue = false;
      SentUpdateRequested = false;
    }
  }
#endif

// START STUFF YOU WANT TO RUN HERE!
// ============================================

  // Check if FloorLamp is enabled
  if (!ENABLE)
  {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    DEBUG_PRINTLN("FloorLamp disabled, sleeping 2 seconds..");
    delay(2000);
    return;
  }

  // Check for config changes
  // Brightness updated?
  if (BrightSwitched)
  {
    FastLED.setBrightness(BRIGHTNESS);
    BrightSwitched = false;
    DEBUG_PRINTLN("Brightness updated to: " + String(BRIGHTNESS));
  }

  // New Simulation selected?
  if (SimSwitched)
  {
    // Publish new simulation name
    mqttClt.publish(simString_topic, String(Simulations[ActiveSim]).c_str(), true);
    SimSwitched = false;
    DEBUG_PRINTLN("Simulation switched, new sim: " + String(Simulations[ActiveSim]));
  }

  // Automatic Simulation switching
  if (AutoSSEnabled && millis() > NextAutoSimSwitch)
  {
    ActiveSim++;
    if (ActiveSim >= SIMCOUNT)
    {
      ActiveSim = 1;
    }
    NextAutoSimSwitch = millis() + (AutoSimSwitch * 1000);
    // Publish new simulation name
    mqttClt.publish(simString_topic, String(Simulations[ActiveSim]).c_str(), true);
    DEBUG_PRINTLN("AutoSimSwitch triggered, new sim: " + String(Simulations[ActiveSim]));
#ifdef ONBOARD_LED
    ToggleLed(LED, 1, 1);
#endif
  }

  // Create and display a simulation frame
  switch (ActiveSim)
  {
  case 0:
    fill_solid(leds, NUM_LEDS, CRGB::White);
    break;
  case 1:
    random16_add_entropy(micros());
    Fire2012WithPalette(1); //Blue fire
    break;
  case 2:
    beatwave(); // run beat_wave simulation frame
    EVERY_N_MILLISECONDS(200)
    {
      nblendPaletteTowardPalette(currentPalette, targetPalette, 24);
    }
    // Change the target palette to a random one every 5 seconds.
    EVERY_N_SECONDS(30)
    {
      targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
    }
    break;
  case 3:
    random16_add_entropy(micros());
    Fire2012WithPalette(0); //normal fire
    break;
  case 4:
    fill_rainbow(leds, NUM_LEDS, millis() / 15); // fill strip with moving rainbow.
    break;
  case 5:
    sinelon();
    break;
  }
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);

#ifdef DEEP_SLEEP
  // disconnect WiFi and go to sleep
  DEBUG_PRINTLN("Good night for " + String(DS_DURATION_MIN) + " minutes.");
  WiFi.disconnect();
  ESP.deepSleep(DS_DURATION_MIN * 60000000);
#endif
}