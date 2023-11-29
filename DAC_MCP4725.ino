#include <math.h>
#include <util/delay.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;

float F, T, stepTimeus, currentPI, rise, fall, v4095;

double sineFX;

int base4096, SYM, WAVE;

unsigned long lastInterruptTime = 0, interruptTime;

bool clockwise, changeFrequency, changeSymmetry = true, changeWave;

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

    int READ2 = (PIND >> 2) & (B00000100 >> 2);

    READ2 == 0 ? clockwise = true : clockwise = false;

    if (clockwise)
    {
        if (changeFrequency) F++;
        if (changeSymmetry) SYM++;
        if (changeWave) WAVE++;

        if (WAVE == 3) WAVE = 0;
    }

    else
    {
        if (changeFrequency) F--;
        if (changeSymmetry) SYM--;
        if (changeWave) WAVE--;

        if (WAVE == -1) WAVE = 2;
    }

    setParameters(F, SYM, WAVE);

    lastInterruptTime = interruptTime;
}

void setup()
{
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(2), rotation, LOW);
    attachInterrupt(digitalPinToInterrupt(3), rotation, LOW);
    
    setParameters(100.0, 50, 1);

    dac.begin(0x60);
}

void loop()
{    
    if (WAVE == 0)
    {
        if (currentPI >= rise) v4095 = 0.0;

        else v4095 = 4095.0;
    }

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

    if (WAVE == 2)
    {
        sineFX = (sin(currentPI) + 1) / 2.0;
        v4095 = 4095.0 * sineFX;
    }

    base4096 = round(v4095);

    dac.setVoltage(base4096, false);

    currentPI += PI / 512.0;
    if (currentPI >= 2 * PI) currentPI = 0.0;

    for (int i = 0; i < stepTimeus; i++) _delay_us(1);
}
