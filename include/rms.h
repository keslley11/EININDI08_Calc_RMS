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
    // Passo 1: Calcular a média dos dados (offset/DC)
    double sum = 0.0;
    for (uint16_t i = 0; i < numSamples; i++) {
        sum += data[i];
    }
    double mean = sum / numSamples;

    // Passo 2: Calcular a soma dos quadrados das diferenças de cada amostra em relação à média
    double sumSquaredDifferences = 0.0;
    for (uint16_t i = 0; i < numSamples; i++) {
        double difference = data[i] - mean;
        sumSquaredDifferences += difference * difference;
    }

    // Passo 3: Calcular o valor RMS: raiz quadrada da média dos quadrados
    double rmsValue = sqrt(sumSquaredDifferences / numSamples);
    return rmsValue;
}
