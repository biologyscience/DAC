#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <util/delay.h>
#include <Adafruit_MCP4725.h>
#include <math.h>

#define STEP 120

Adafruit_SSD1306 display(128, 32, &Wire, -1);
Adafruit_MCP4725 dac;

float F, T, Tus, rise, fall, v4095, factor = 0.0, deno;

double sineFX;

int t = 0, base4096, SYM, WAVE, mode = 2;

char waveType[10][3] = {"Square", "Triangle", "Sine"};

unsigned long lastInterruptTime = 0, interruptTime;

bool clockwise, UP = true;

void setParameters(float frequency, int symmetry, int wave)
{
    F = frequency;
    SYM = symmetry;
    WAVE = wave;

    if (WAVE == 2) SYM = 50;

    T = 1 / F;
    Tus = T * 1000000.0;
    rise = (SYM / 100.0) * (Tus / 2.0);
    fall = (Tus / 2.0) - rise;
}

void displayNEW()
{
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(1, 1);
    display.print(F("Frequency : "));
    display.setCursor(80, 1);
    display.print(F);
    display.print(F(" Hz"));

    display.setCursor(1, 10);
    display.print(F("Symmetry : "));
    display.setCursor(80, 10);
    display.print(SYM);
    display.print(F(" %"));

    display.setCursor(1, 20);
    display.print(F("Wave : "));
    display.setCursor(80, 20);
    display.print(waveType[WAVE]);

    display.display();
}

void rotation()
{
    interruptTime = millis();

    if (interruptTime - lastInterruptTime < 1) return;

    int READ2 = (PIND >> 2) & (B00000100 >> 2);

    READ2 == 0 ? clockwise = true : clockwise = false;

    if (clockwise)
    {

        if (mode == 0)
        {

            if (F >= 0 && F <= 1000)
                F += 10;
        };
        if (mode == 1)
            SYM++;
        if (mode == 2)
            WAVE++;

        if (WAVE == 3)
            WAVE = 0;
    }

    else
    {
        if (mode == 0)
        {
            if (F >= 0 && F <= 1000)
                F -= 10;
        }
        if (mode == 1)
            SYM--;
        if (mode == 2)
            WAVE--;

        if (WAVE == -1)
            WAVE = 2;
    }

    setParameters(F, SYM, WAVE);

    lastInterruptTime = interruptTime;

    displayNEW();
}

void setup()
{
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
    pinMode(7, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(2), rotation, LOW);
    attachInterrupt(digitalPinToInterrupt(3), rotation, LOW);

    dac.begin(0x60);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    setParameters(10.0, 50, 0);
    displayNEW();
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

        if (!UP)
            v4095 = 0.0;
    }

    // TRIANGLE
    if (WAVE == 1)
    {
        v4095 = 4095.0 * factor;

        if (!UP)
            v4095 = 4095.0 - v4095;
    }

    // SINE
    if (WAVE == 2)
    {
        sineFX = (sin(2 * PI * factor) + 1) / 2.0;
        v4095 = 4095.0 * sineFX;
    }

    base4096 = round(v4095);

    dac.setVoltage(base4096, false);
}
