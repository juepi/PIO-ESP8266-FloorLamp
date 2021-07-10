/*
*   ESP8266 FloorLamp
*   FastLED specific settings
*/
#ifndef FASTLED_CONFIG_H
#define FASTLED_CONFIG_H

#include <Arduino.h>
#include <FastLED.h>

#define FASTLED_USING_NAMESPACE

// Setup LED Strip
#define LED_PIN D8
#define COLOR_ORDER GRB
#define CHIPSET WS2812B
#define NUM_LEDS 155

extern CRGBArray<NUM_LEDS> leds;

// Required for use with ESP8266 to stop flickering LEDs
// see https://github.com/FastLED/FastLED/issues/306
// also reduce jitter by WiFi.setSleepMode(WIFI_NONE_SLEEP) in setup
#define FASTLED_INTERRUPT_RETRY_COUNT 0

// ============== Simulation specific defines =========================
// BeatWave
extern void beatwave();
extern CRGBPalette16 currentPalette;
extern CRGBPalette16 targetPalette;
extern TBlendType currentBlending;

// Fire
extern void Fire2012WithPalette(int FireType);
extern CRGBPalette16 FirePal;
extern CRGBPalette16 BlueFirePal;
extern bool fireReverseDirection;
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100
#define COOLING 60

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 80

// Sinelon
extern void sinelon();
extern uint8_t gHue;

#endif //FASTLED_CONFIG_H