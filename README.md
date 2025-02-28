# Cálculo de RMS com Janela Fixa e Janela Deslizante

## Sumário
- [Introdução](#introdução)
- [Cálculo do RMS: Derivação Matemática](#cálculo-do-rms-derivação-matemática)
- [Transição para a Forma Discreta](#transição-para-a-forma-discreta)
- [Implementação em C](#implementação-em-c)
- [Uso com Janela Deslizante ou Janela Fixa](#uso-com-janela-deslizante-ou-janela-fixa)
- [Exemplo de Código Arduino](#exemplo-de-código-arduino)
- [Exemplo de Código Arduino: Janela Deslizante](#exemplo-de-código-arduino-janela-deslizante)
- [Conclusão](#conclusão)
- [Referências](#referências)

## Introdução
O cálculo do valor RMS (Root Mean Square) é amplamente utilizado para determinar a magnitude efetiva de um sinal, especialmente quando se deseja eliminar o offset (componente DC). Este documento apresenta a derivação matemática do cálculo do RMS, desde a forma integral contínua até sua implementação discreta em C, e discute sua aplicação prática utilizando abordagens com janela fixa e janela deslizante para leitura instantânea ou acumulada.

## Cálculo do RMS: Derivação Matemática
Para um sinal contínuo $x(t)$ definido no intervalo $[0, T]$, o valor RMS é dado por:

$$
x_{\text{RMS}} = \sqrt{\frac{1}{T} \int_{0}^{T} x(t)^2 \, dt}
$$

Caso o sinal possua um offset, calcula-se primeiramente a média:

$$
\mu = \frac{1}{T} \int_{0}^{T} x(t) \, dt
$$

E o RMS do sinal sem o offset é definido como:

$$
x_{\text{RMS}} = \sqrt{\frac{1}{T} \int_{0}^{T} \bigl(x(t) - \mu\bigr)^2 \, dt}
$$

## Transição para a Forma Discreta
Para sinais discretos com $N$ amostras $x[n]$ (com $n = 0, 1, \dots, N-1$), as integrais são substituídas por somatórios, resultando em:

$$
\mu \approx \frac{1}{N} \sum_{n=0}^{N-1} x[n]
$$

$$
x_{\text{RMS}} \approx \sqrt{\frac{1}{N} \sum_{n=0}^{N-1} \bigl(x[n] - \mu\bigr)^2 }
$$


/* Representação das equações discretas:
   μ = (1/N) * Σₙ x[n]  
   x_RMS = sqrt((1/N) * Σₙ (x[n] - μ)²)
*/


## Implementação em C
A função a seguir implementa o cálculo do RMS removendo o offset do sinal, seguindo estes passos:
1. Calcular a média dos valores do sinal (offset/DC).
2. Subtrair a média de cada amostra e acumular os quadrados das diferenças.
3. Calcular a média dos quadrados.
4. Retornar a raiz quadrada dessa média, resultando no valor RMS.

```c
#include <math.h>
#include <stdint.h>

/**
 * @brief Calcula o valor RMS (Root Mean Square) removendo o offset do sinal.
 *
 * Essa função realiza os seguintes passos:
 * 1. Calcula a média dos valores (offset/DC) do sinal.
 * 2. Subtrai a média de cada amostra e eleva ao quadrado, acumulando os resultados.
 * 3. Calcula a média desses quadrados.
 * 4. Retorna a raiz quadrada dessa média, que é o valor RMS.
 *
 * @param data Ponteiro para o vetor de dados do sinal.
 * @param numSamples Número de amostras presentes no vetor.
 * @return double Valor RMS do sinal.
 */
double calculateRMS(const int *data, const uint16_t numSamples) {
    double sum = 0.0;
    for (uint16_t i = 0; i < numSamples; i++) {
        sum += data[i];
    }
    double mean = sum / numSamples;

    double sumSquaredDifferences = 0.0;
    for (uint16_t i = 0; i < numSamples; i++) {
        double difference = data[i] - mean;
        sumSquaredDifferences += difference * difference;
    }

    double rmsValue = sqrt(sumSquaredDifferences / numSamples);
    return rmsValue;
}
```

## Uso com Janela Deslizante ou Janela Fixa
Na prática, o cálculo do RMS pode ser aplicado de duas formas:

- **Janela Fixa:**  
  O cálculo é realizado sobre um conjunto fixo de amostras. Por exemplo, ao acumular 200 amostras, o valor RMS é calculado e a janela é reiniciada. Essa abordagem é simples e ideal para sistemas que não exigem atualização contínua do valor medido.

- **Janela Deslizante:**  
  Nessa abordagem, a janela de amostras é atualizada a cada nova leitura; a cada nova amostra, a amostra mais antiga é descartada e o novo valor é incorporado. Assim, o RMS é recalculado de forma contínua, permitindo monitorar o sinal de forma quase instantânea. Essa técnica é útil em aplicações que exigem resposta imediata a mudanças no sinal, embora demande maior capacidade de processamento.

## Exemplo de Código Arduino
O código a seguir exemplifica a leitura de um sinal analógico em um Arduino, o cálculo do RMS utilizando uma janela fixa (ou adaptável para janela deslizante) e a transmissão do valor RMS via comunicação serial. Nele, as leituras são armazenadas em um vetor até atingir o tamanho definido (`WINSIZE`). Quando a janela está completa, o RMS é calculado e o valor é enviado pela porta serial.

```c
#include <Arduino.h>
#include "rms.h"

#define pinANALOG A5                        // Configura o pino de leitura do LUXÍMETRO
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
  Serial.print(A * rmsFinal + B);
  Serial.println("|g");
}

void analogReadFunc() {                      // Faz a leitura do sinal Analógico
  arrayValue[index++] = analogRead(pinANALOG);
  if (index == WINSIZE) {
    const float aux = calculateRMS(arrayValue, index);
    rmsFinal = abs(rmsFinal - aux) < 0.9 ? rmsFinal : aux;
    index = 0;
  }
}

void setup() {                              // Código de configuração
  Serial.begin(19200);
  pinMode(pinANALOG, INPUT);
}

#define TIME_DELAY_ANALOG_MS 1
uint32_t previousTimeAnalogMS = 0;

#define TIME_DELAY_PRINT_MS 100
uint32_t previousTimePrintMS = 0;

void loop() {
  uint32_t currentMilis = millis();
  if ((currentMilis - previousTimeAnalogMS) >= TIME_DELAY_ANALOG_MS) {
    previousTimeAnalogMS = currentMilis;
    analogReadFunc();
  }
  if ((currentMilis - previousTimePrintMS) >= TIME_DELAY_PRINT_MS) {
    previousTimePrintMS = currentMilis;
    printSerial(currentMilis);
  }  
}
```

## Exemplo de Código Arduino: Janela Deslizante
A seguir, um exemplo de código para implementar o cálculo do RMS com janela deslizante, utilizando a mesma função de cálculo do RMS da biblioteca. Neste exemplo, a cada nova amostra, a janela é atualizada removendo a amostra mais antiga e adicionando a nova. Para simplificar, este exemplo assume um buffer circular para manter as amostras.

```c
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
```

## Conclusão
Este documento apresentou uma descrição detalhada do cálculo do valor RMS, desde a derivação matemática utilizando integrais até sua implementação prática em C e Arduino. Foram discutidas as abordagens de janela fixa e janela deslizante para o cálculo do RMS, possibilitando a obtenção de uma leitura efetiva do sinal, seja de forma instantânea ou acumulada, conforme as necessidades da aplicação.

## Referências
- Datasheet do AD5700/AD5700-1 – Analog Devices  
- *A Basic Guide to the HART Protocol* – Texas Instruments  
- Discussões e exemplos em fóruns de Arduino e comunidades de instrumentação
