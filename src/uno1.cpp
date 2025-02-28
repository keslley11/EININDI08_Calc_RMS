#include <Arduino.h>
#include "rms.h"

#define pinANALOG A5                        // Pino de leitura do sinal
#define WINSIZE 200                        // Tamanho da janela deslizante

int buffer[WINSIZE];
float rmsValue = 0;
int index = 0;
bool bufferFull = false;

void updateBuffer(int newSample) {
  buffer[index] = newSample;
  index = (index + 1) % WINSIZE;
  if (index == 0) {
    bufferFull = true;
  }
}

float computeSlidingRMS() {
  // Se o buffer ainda não estiver cheio, calcula o RMS apenas das amostras disponíveis
  int samples = bufferFull ? WINSIZE : index;
  return calculateRMS(buffer, samples);
}

void setup() {
  Serial.begin(19200);
  pinMode(pinANALOG, INPUT);
}

#define TIME_DELAY_MS 1
uint32_t previousTime = 0;
#define PRINT_DELAY_MS 100
uint32_t previousPrint = 0;

void loop() {
  uint32_t currentMillis = millis();
  if (currentMillis - previousTime >= TIME_DELAY_MS) {
    previousTime = currentMillis;
    // Leitura do sinal, com ajuste de offset (exemplo: 512)
    int sample = analogRead(pinANALOG);
    updateBuffer(sample);
    // Atualiza o RMS com base na janela deslizante
    rmsValue = computeSlidingRMS();
  }
  if (currentMillis - previousPrint >= PRINT_DELAY_MS) {
    previousPrint = currentMillis;
    Serial.print(">graf:");
    Serial.print(currentMillis);
    Serial.print(":");
    Serial.print(rmsValue);
    Serial.println("|g");
  }
}