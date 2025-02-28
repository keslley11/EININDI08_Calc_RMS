#include <Arduino.h>
#include "rms.h"

#define pinANALOG A5                        //Configura o pino de leitura do LUXÍMETRO
#define WINSIZE 200

#define A 1
#define B 0

int arrayValue[WINSIZE];
float rmsFinal = 0;
int index = 0;

void printSerial(const uint32_t &currentMilis) {
  Serial.print(">graf:");
  Serial.print(currentMilis);
  Serial.print(":");
  Serial.print(A*rmsFinal+B);
  Serial.println("|g");
}

void analogReadFunc() {                      // Faz a leitura do sinal Analógico
  arrayValue[index++] = analogRead(pinANALOG);
  if(index==WINSIZE){
    // rmsFinal = calculateRMS(arrayValue, index);
    const float aux = calculateRMS(arrayValue, index);
    rmsFinal = abs(rmsFinal - aux) < 0.9 ? rmsFinal: aux;
    index = 0;
  }
}

void setup() {                              // Codigo de configuração
  Serial.begin(19200);
  pinMode(pinANALOG, INPUT);
}

#define TIME_DELAY_ANALOG_MS 1
uint32_t previousTimeAnalogMS = 0;

#define TIME_DELAY_PRINT_MS 100
uint32_t previousTimePrintMS = 0;

void loop()
{
  uint32_t currentMilis = millis();
  if ((currentMilis - previousTimeAnalogMS) >= TIME_DELAY_ANALOG_MS)
  {
    previousTimeAnalogMS = currentMilis;
    analogReadFunc();
  }
  if ((currentMilis - previousTimePrintMS) >= TIME_DELAY_PRINT_MS)
  {
    previousTimePrintMS = currentMilis;
    printSerial(currentMilis);
  }  
}