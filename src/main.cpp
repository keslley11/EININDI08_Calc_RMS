#include "IIKitmini.h"
#include "util/asyncDelay.h"

// Neste código é trabalhado: Leds, botões, display e serial

AsyncDelay_c blinkLED1(500); // time mili second
void blinkLEDFunc(uint8_t pin)
{
    if (blinkLED1.isExpired())
    {
        digitalWrite(pin, !digitalRead(pin));
        // blinkLED1.restart(500);
    }
}

AsyncDelay_c blinkLED2(800); // time mili second
void blinkLEDFunc2(uint8_t pin)
{
    if (blinkLED2.isExpired())
    {
        digitalWrite(pin, !digitalRead(pin));
        // blinkLED2.restart(500);
    }
}

void push1_LED3(uint8_t LED_pin)
{
    // sem debounce

    if (digitalRead(def_pin_PUSH1))
    {
        digitalWrite(LED_pin, HIGH);
    }
    else
        digitalWrite(LED_pin, LOW);

    // com debounce (se usar interrupção)

    // bool enable_button = true;
    // if (enable_button)
    // {
    //   if (digitalRead(def_pin_PUSH1))
    //   {
    //     digitalWrite(LED_pin, HIGH);
    //     AsyncDelay_c debounce_time(50); // time mili second
    //     while (!debounce_time.isExpired())
    //     {
    //       enable_button = false;
    //     }
    //     enable_button = true;
    //   }
    //   else
    //     digitalWrite(LED_pin, LOW);
    // }
}

void push2_LED3_togle(uint8_t LED_pin)
{

    if (digitalRead(def_pin_PUSH2))
    {
        digitalWrite(LED_pin, !digitalRead(LED_pin)); // usar interrupção!
    }
}

volatile bool estadoLED = false;              // Usado dentro e fora da interrupção
volatile unsigned long ultimaInterrupcao = 0; // debounce
void IRAM_ATTR handleBotaoPressionado()
{
    unsigned long tempoAgora = millis();
    if (tempoAgora - ultimaInterrupcao > 200)
    {
        estadoLED = !estadoLED;
        ultimaInterrupcao = tempoAgora;
    }
}

AsyncDelay_c delayPOT(50); // time mili second
void managerInputFunc(void)
{
    if (delayPOT.isExpired())
    {
        const uint16_t vlPOT1 = analogRead(def_pin_ADC1);
        const uint16_t vlPOT2 = analogRead(def_pin_ADC2);
        const uint16_t led3 = digitalRead(def_pin_D3) ? 1 : 0;
        const uint16_t led4 = digitalRead(def_pin_D4) ? 1 : 0;
        IIKit.disp.setText(2, ("P1:" + String(vlPOT1) + "| P2:" + String(vlPOT2)).c_str());
        IIKit.disp.setText(3, ("Led3:" + String(led3) + "| Led4:" + String(led4)).c_str());
        IIKit.WSerial.plot("vlPOT1", vlPOT1);
        IIKit.WSerial.plot("vlPOT2", vlPOT2);
    }
}

void setup()
{
    IIKit.setup();
    // pinMode(def_pin_D1, OUTPUT);
    // pinMode(def_pin_D2, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(def_pin_PUSH2), handleBotaoPressionado, RISING);
}

void loop()
{
    IIKit.loop();
    blinkLEDFunc(def_pin_D1);
    blinkLEDFunc2(def_pin_D2);
    push1_LED3(def_pin_D3);
    // push2_LED3_togle(def_pin_D4);
    digitalWrite(def_pin_D4, estadoLED); // interrupção PUSH2 --> togle LED3
    managerInputFunc();
}