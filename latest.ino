#include <math.h>
#include <util/delay.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>

#define runAfter(t,Clock)    for (static unsigned long _ATimer = Clock; (unsigned long)(Clock - _ATimer) >= (t); _ATimer = Clock)
#define STEP 120

Adafruit_MCP4725 dac;

float F, T, Tus, rise, fall, v4095, factor = 0.0, deno;

double sineFX;

int t = 0, base4096, SYM, WAVE, mode = 0;

unsigned long lastInterruptTime = 0, interruptTime;

bool clockwise, UP = true;

void setParameters(float frequency, int symmetry, int wave)
{
    F = frequency;
    SYM = symmetry;
    WAVE = wave;

    if (WAVE == 2) SYM = 50;
    
    T = 1/F;
    Tus = T * 1000000.0;
    rise = (SYM / 100.0) * (Tus / 2.0);
    fall = (Tus / 2.0) - rise;
}

void rotation()
{  
    interruptTime = millis();

    if (interruptTime - lastInterruptTime < 1) return;

    int READ2 = (PIND >> 2) & (B00000100 >> 2);

    READ2 == 0 ? clockwise = true : clockwise = false;

    if (clockwise)
    {
       
          
        
       if (mode == 0) {
        
        if(F>=0&&F<=1000)F+=10;
       };
        if (mode == 1) SYM++;
        if (mode == 2) WAVE++;

        if (WAVE == 3) WAVE = 0;
    }

    else
    {
        if (mode == 0) {
          if(F>=0&&F<=1000)F-=10;
        }
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
    
    setParameters(10.0, 50, 2);
    dac.begin(0x60);
}

void loop()
{
    t += STEP;
    factor = t / deno;

    
    if (factor >= 1)
    {
        t = 0;
        factor = 0;
        UP = !UP;

        UP ? deno = rise : deno = fall;
    }

    // MODE CHANGE
    if (((PIND >> 7) & (B10000000 >> 7)) == 0)
    {
        mode++;

        mode == 3 ? mode = 0 : NULL;
    }

    // SQUARE
    if (WAVE == 0)
    {
        v4095 = 4095.0;

        if (!UP) v4095 = 0.0;
    }

    // TRIANGLE
    if (WAVE == 1)
    {        
        v4095 = 4095.0 * factor;

        if (!UP) v4095 = 4095.0 - v4095;
    }

    // SINE
    if (WAVE == 2)
    {
        sineFX = (sin(2 * PI * factor) + 1) / 2.0;
        v4095 = 4095.0 * sineFX;
    }

    base4096 = round(v4095);

    dac.setVoltage(base4096, false);

    // runAfter(STEP, micros());
}
