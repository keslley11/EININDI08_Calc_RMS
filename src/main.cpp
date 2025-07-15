#include <Arduino.h>
#include "IIKitmini.h"   // Biblioteca base do framework Arduino, necessária para funções básicas como Serial e delays.

void setup()
{
    IIKit.setup();
}

void loop()
{
    IIKit.loop();
}