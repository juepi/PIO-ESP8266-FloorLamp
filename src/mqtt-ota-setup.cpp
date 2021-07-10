/*
 * ESP8266 Template
 * MQTT / OTA specific setup
 */
#include <Arduino.h>
#include "mqtt-ota-config.h"

// Define Variables
char message_buff[20];
#ifdef OTA_UPDATE
bool OTAupdate = false;
bool SentUpdateRequested = false;
bool OtaInProgress = false;
bool OtaIPsetBySketch = false;
bool SentOtaIPtrue = false;
#endif
float VCC = 3.333;
unsigned int SubscribedTopics = 0;
unsigned int ReceivedTopics = 0;

// Floor lamp specific vars
int ActiveSim = 2;
bool SimSwitched = false;
char Simulations[SIMCOUNT][10]=
{
    "White",
    "BlueFire",
    "BeatWave",
    "RedFire",
    "Rainbow",
    "Sinelon"
};
int BRIGHTNESS = 80;
bool BrightSwitched = false;
int FRAMES_PER_SECOND = 30;
bool ENABLE = false;
bool AutoSSEnabled = false;

// Automatic Simulation switching and minumum sim runtime
unsigned long AutoSimSwitch = 300;       // Simulations automatically rotate after X seconds
unsigned long NextAutoSimSwitch = 0;
