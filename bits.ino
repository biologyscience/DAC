#include <math.h>

int binary[8];

void intTo8Bit(int num)
{
    int i, j;
    
    for (i = 0; i < 7; i++) binary[i] = 0;
      
    char temp[8];

    itoa(num, temp, 2);

    for (i = 0; temp[i] != '\0'; i++);

    for (i--, j = 7; i >= 0; i--, j--) binary[j] = temp[i] - '0';
}

void setup()
{
    for (int i = 3; i < 11; i++) pinMode(i, OUTPUT);

    Serial.begin(115200);
}

float F = 500;
float T = 1/F;
float t = 0.0;
float stepTimeus = T / 1024.0 * 1000000.0;
float currentPI = 0.0;
float v255;
double sineFX;

int base256, x;

void loop()
{
    currentPI += PI / 512.0;
    sineFX = (sin(currentPI) + 1) / 2.0;
    v255 = 255.0 * sineFX;

    base256 = round(v255);

    intTo8Bit(base256);
  
//    for (int x = 0; x < 7; x++) Serial.print(binary[x]);
//    Serial.println(' ');

    Serial.println(base256);

    for (int x = 0; x < 8; x++) {digitalWrite(x + 3, binary[x]); Serial.print(binary[x]);}

    Serial.println(' ');

    if (currentPI >= 2 * PI) currentPI = 0.0;
  
//    delayMicroseconds(stepTimeus);

    delay(5);
}
