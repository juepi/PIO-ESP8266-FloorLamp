/*
 *  ESP8266 FloorLamp
 *  FastLED specific setup
 */
#include <Arduino.h>
#include <FastLED.h>
#include "fastled-config.h"

// LED Strip setup
CRGBArray<NUM_LEDS> leds;

// ============== Simulation specific variables =========================
// BeatWave Palette definitions
CRGBPalette16 currentPalette;
CRGBPalette16 targetPalette;
TBlendType currentBlending;

// Fire
CRGBPalette16 FirePal;
CRGBPalette16 BlueFirePal;
bool fireReverseDirection = false;

// Sinelon
uint8_t gHue = 0; // rotating "base color"


/*
 *        FastLED Simulations
 * ========================================================================
 */
void beatwave()
{

  uint8_t wave1 = beatsin8(9, 0, 255); // That's the same as beatsin8(9);
  uint8_t wave2 = beatsin8(8, 0, 255);
  uint8_t wave3 = beatsin8(7, 0, 255);
  uint8_t wave4 = beatsin8(6, 0, 255);

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, i + wave1 + wave2 + wave3 + wave4, 255, currentBlending);
  }
}

void Fire2012WithPalette(int FireType)
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];
  CRGB color;

  // Step 1.  Cool down every cell a little
  for (int i = 0; i < NUM_LEDS; i++)
  {
    heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k = NUM_LEDS - 1; k >= 2; k--)
  {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if (random8() < SPARKING)
  {
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }

  // Step 4.  Map from heat cells to LED colors
  for (int j = 0; j < NUM_LEDS; j++)
  {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    byte colorindex = scale8(heat[j], 240);
    switch (FireType)
    {
    case 0: //Normal Fire
      color = ColorFromPalette(FirePal, colorindex);
      break;
    case 1: //blue fire
      color = ColorFromPalette(BlueFirePal, colorindex);
      break;
    }
    int pixelnumber;
    if (fireReverseDirection)
    {
      pixelnumber = (NUM_LEDS - 1) - j;
    }
    else
    {
      pixelnumber = j;
    }
    leds[pixelnumber] = color;
  }
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(gHue, 255, 192);
}