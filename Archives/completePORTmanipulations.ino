#include <math.h>
#include <util/delay.h>

float F, T, stepTimeus, currentPI, DC, SYM, v255;

double sineFX;

int base256, WAVE;
// WAVE | 0 - SQR | 1 - TRI | 2 - SINE |

unsigned long lastInterruptTime = 0, interruptTime;

bool clockwise, changeFrequency = true, changeDutyCycle, changeSymmetry;

byte binaryB, binaryD;

void setParameters(float frequency, float dutyCycle, float symmetry, int wave)
{
    F = frequency;
    T = 1/F;
    stepTimeus = (T / 1024.0) * 1000000.0;
    currentPI = 0.0;
    DC = dutyCycle;
    SYM = symmetry;
    WAVE = wave;
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
        if (changeDutyCycle) DC++;
        if (changeSymmetry) SYM++;
    }

    else
    {
        if (changeFrequency) F--;
        if (changeDutyCycle) DC--;
        if (changeSymmetry) SYM--;
    }

    setParameters(F, DC, SYM, WAVE);

    lastInterruptTime = interruptTime;
}

void setup()
{
    for (int i = 2; i < 12; i++)
    {
        if (i > 3) pinMode(i, OUTPUT);

        else
        {
            pinMode(i, INPUT_PULLUP);
            attachInterrupt(digitalPinToInterrupt(i), rotation, LOW);
        }
    }
    
    setParameters(100.0, 50.0, 50.0, 2);

    Serial.begin(115200);
}

void loop()
{
    sineFX = (sin(currentPI) + 1) / 2.0;
    v255 = 255.0 * sineFX;

    base256 = round(v255);

    binaryB = byte(base256);
    binaryD = byte(base256);

    binaryB >>= 4;
    binaryD <<= 4;

    PORTB = binaryB;
    PORTD = binaryD;

    Serial.println(base256);

    currentPI += PI / 512.0;
    if (currentPI >= 2 * PI) currentPI = 0.0;

    for (int i = 0; i < stepTimeus; i++) _delay_us(1);
}
