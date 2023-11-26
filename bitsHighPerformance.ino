#include <math.h>
#include <util/delay.h>

float F, T, stepTimeus, currentPI, DC, SYM, v255;

double sineFX;

int base256, binary[8], WAVE;

unsigned long lastInterruptTime = 0, interruptTime;

bool clockwise, changeFrequency = true, changeDutyCycle, changeSymmetry;

void intTo8Bit(int num)
{
    int i, j;
    
    for (i = 0; i < 7; i++) binary[i] = 0;
      
    char temp[8];

    itoa(num, temp, 2);

    for (i = 0; temp[i] != '\0'; i++);

    for (i--, j = 7; i >= 0; i--, j--) binary[j] = temp[i] - '0';
}

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

    digitalRead(2) == LOW ? clockwise = true : clockwise = false;

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

    intTo8Bit(base256);

    Serial.println(base256);
    
    for (int x = 0; x < 8; x++)
    {        
        if (x < 4)
        {
            if (binary[x] == 0) bitClear(PORTD, x + 4);
            else bitSet(PORTD, x + 4);
        }

        else
        {
            if (binary[x] == 0) bitClear(PORTB, x - 4);
            else bitSet(PORTB, x - 4);
        }
    }

//    Serial.println(' ');

    currentPI += PI / 512.0;
    if (currentPI >= 2 * PI) currentPI = 0.0;
  
//    delayMicroseconds(244.14 / 4);

//    stepTimeus = round(stepTimeus);

    for (int i = 0; i < stepTimeus; i++) _delay_us(1);

//delay(10);
}
