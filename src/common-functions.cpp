/*
 * ESP8266 Template
 * Common Functions
 */
#include <Arduino.h>
#include "common-functions.h"

void ToggleLed(int PIN, int WaitTime, int Count)
{
    // Toggle digital output
    for (int i = 0; i < Count; i++)
    {
        digitalWrite(PIN, !digitalRead(PIN));
        delay(WaitTime);
    }
}

/*
 * Callback Functions
 * ========================================================================
 */
//MQTT Subscription callback function
void MqttCallback(char *topic, byte *payload, unsigned int length)
{
    unsigned int i = 0;
    // create character buffer with ending null terminator (string)
    for (i = 0; i < length; i++)
    {
        message_buff[i] = payload[i];
    }
    message_buff[i] = '\0';
    String msgString = String(message_buff);

    DEBUG_PRINTLN("MQTT: Message arrived [" + String(topic) + "]: " + String(msgString));

// run through topics
#ifdef OTA_UPDATE
    if (String(topic) == ota_topic)
    {
        if (msgString == "on")
        {
            OTAupdate = true;
            ReceivedTopics++;
        }
        else if (msgString == "off")
        {
            OTAupdate = false;
            ReceivedTopics++;
        }
        else
        {
            DEBUG_PRINTLN("MQTT: ERROR: Fetched invalid OTA-Update: " + String(msgString));
            delay(200);
        }
    }
    else if (String(topic) == otaInProgress_topic)
    {
        if (msgString == "on")
        {
            OtaInProgress = true;
            ReceivedTopics++;
        }
        else if (msgString == "off")
        {
            OtaInProgress = false;
            ReceivedTopics++;
        }
        else
        {
            DEBUG_PRINTLN("MQTT: ERROR: Fetched invalid OtaInProgress: " + String(msgString));
            delay(200);
        }
    }
    else if (String(topic) == sim_topic)
    {
        int IntPayLd = msgString.toInt();
        if ((IntPayLd >= 0) && (IntPayLd < SIMCOUNT))
        {
            // Valid value, use for new sim
            ActiveSim = IntPayLd;
            SimSwitched = true;
            DEBUG_PRINTLN("MQTT: Fetched new sim: " + String(Simulations[ActiveSim]));
            ReceivedTopics++;
        }
        else
        {
            DEBUG_PRINTLN("MQTT: ERROR: Invalid sim fetched: " + String(IntPayLd));
            delay(200);
        }
    }
    else if (String(topic) == brightness_topic)
    {
        int IntPayLd = msgString.toInt();
        if ((IntPayLd >= 0) && (IntPayLd <= 255))
        {
            // Valid value, use for new brightness value
            BRIGHTNESS = IntPayLd;
            BrightSwitched = true;
            DEBUG_PRINTLN("MQTT: Fetched new brightness: " + String(BRIGHTNESS));
            ReceivedTopics++;
        }
        else
        {
            DEBUG_PRINTLN("MQTT: ERROR: Fetched invalid brightness: " + String(IntPayLd));
            delay(200);
        }
    }
    else if (String(topic) == fps_topic)
    {
        int IntPayLd = msgString.toInt();
        if ((IntPayLd >= 10) && (IntPayLd <= 120))
        {
            // Valid value, use for new brightness value
            FRAMES_PER_SECOND = IntPayLd;
            DEBUG_PRINTLN("MQTT: Fetched new FPS: " + String(FRAMES_PER_SECOND));
            ReceivedTopics++;
        }
        else
        {
            DEBUG_PRINTLN("MQTT: ERROR: Fetched invalid FPS: " + String(IntPayLd));
            delay(200);
        }
    }
    else if (String(topic) == enable_topic)
    {
        if (msgString == "on")
        {
            ENABLE = true;
            ReceivedTopics++;
        }
        else if (msgString == "off")
        {
            ENABLE = false;
            ReceivedTopics++;
        }
        else
        {
            DEBUG_PRINTLN("MQTT: ERROR: Fetched invalid ENABLE: " + String(msgString));
            delay(200);
        }
    }
    else if (String(topic) == AutoSimSwitch_topic)
    {
        if (msgString == "on")
        {
            AutoSSEnabled = true;
            ReceivedTopics++;
        }
        else if (msgString == "off")
        {
            AutoSSEnabled = false;
            ReceivedTopics++;
        }
        else
        {
            DEBUG_PRINTLN("ERROR: Fetched invalid AutoSimSwitch: " + String(msgString));
            delay(200);
        }
    }
    else
    {
        DEBUG_PRINTLN("ERROR: Unknown topic: " + String(topic));
        DEBUG_PRINTLN("ERROR: Unknown topic value: " + String(msgString));
        delay(200);
    }
#endif //OTA_UPDATE
}
