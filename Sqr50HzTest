#include <Wire.h>
#include <Adafruit_MCP4725.h>

#define runAfter(t,Clock)    for (static unsigned long _ATimer = Clock; (unsigned long)(Clock - _ATimer) >= (t); _ATimer = Clock)

Adafruit_MCP4725 dac;

int WAIT = 5000;

void setup()
{
    dac.begin(0x60);
}

void loop()
{
    dac.setVoltage(0, false);

    runAfter(WAIT, micros());

    dac.setVoltage(0, false);

    runAfter(WAIT, micros());
}
