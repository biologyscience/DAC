#include <math.h>
#include <util/delay.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;

float F, T, stepTimeus, currentPI, rise, fall, v4095;

double sineFX;

int base4096, SYM, WAVE, mode = 0;

unsigned long lastInterruptTime = 0, interruptTime;

bool clockwise;

void setParameters(float frequency, int symmetry, int wave)
{
    F = frequency;
    SYM = symmetry;
    WAVE = wave;
    
    T = 1/F;
    stepTimeus = (T / 1024.0) * 1000000.0;
    currentPI = 0.0;
    rise = (SYM / 100.0) * 2 * PI;
    fall = (2 * PI) - rise;
}

void rotation()
{  
    interruptTime = millis();

    if (interruptTime - lastInterruptTime < 1) return;

    clockwise == !((PIND >> 2) & (B00000100 >> 2));

    if (clockwise)
    {
        if (mode == 0) F++;
        if (mode == 1) SYM++;
        if (mode == 2) WAVE++;

        if (WAVE == 3) WAVE = 0;
    }

    else
    {
        if (mode == 0) F--;
        if (mode == 1) SYM--;
        if (mode == 2) WAVE--;

        if (WAVE == -1) WAVE = 2;
    }

    setParameters(F, SYM, WAVE);

    lastInterruptTime = interruptTime;
}

void setup()
{
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
    pinMode(7, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(2), rotation, LOW);
    attachInterrupt(digitalPinToInterrupt(3), rotation, LOW);
    
    setParameters(50.0, 50, 0);

    dac.begin(0x60);
}

void loop()
{
    // MODE CHANGE
    if (((PIND >> 7) & (B10000000 >> 7)) == 0)
    {
        mode++;

        mode == 3 ? mode = 0 : NULL;
    }

    // SQUARE
    if (WAVE == 0)
    {
        if (currentPI >= rise) v4095 = 0.0;

        else v4095 = 4095.0;
    }

    // TRIANGLE
    if (WAVE == 1)
    {        
        float factor = currentPI / rise;

        v4095 = 4095.0 * factor;

        if (factor >= 1.0)
        {
            factor = 1 - (((2 * PI) - currentPI) / fall);
            v4095 = 4095.0 - (4095.0 * factor);
        }
    }

    // SINE
    if (WAVE == 2)
    {
        sineFX = (sin(currentPI) + 1) / 2.0;
        v4095 = 4095.0 * sineFX;
    }

    base4096 = round(v4095);

    dac.setVoltage(base4096, false);

    currentPI += (PI / 512.0);
    if (currentPI >= (2 * PI)) currentPI = 0.0;

    for (int i = 0; i < stepTimeus; i++) _delay_us(1);
}
