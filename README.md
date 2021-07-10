# Introduction 
This sketch allows you to run various animations on a single WS2812B LED strip. Animations along with other settings (brightness, automatic animation switching, on/off) can be configured via MQTT.  
I have installed the LED-strip (144 LEDs/m) vertically in a cheap cylindric shaped floor lamp covered with white paper.  
  
The animation routines have been mostly copied from [FastLED library examples](https://github.com/FastLED/FastLED/tree/master/examples).  

Code based on [PIO-ESP8266-Template](https://github.com/juepi/PIO-ESP8266-Template).

## Local Requirements
A (local) MQTT broker is mandatory.  

## Generic Requirements
Please see [PIO-ESP8266-Template](https://github.com/juepi/PIO-ESP8266-Template) for general requirements and usage of this sketch.  

# Version History

## Initial Release v1.0.0
- Firmware converted from [original Arduino sketch](https://github.com/juepi/ESP8266-FloorLamp) to PIO ESP8266 template based version