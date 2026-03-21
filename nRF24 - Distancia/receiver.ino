#include <SPI.h>
#include <RF24.h>

#define CE_PIN 4
#define CSN_PIN 5
#define LED 2

RF24 radio(CE_PIN, CSN_PIN);
const byte direccion[6] = "00001";

bool estadoLED = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  if (!radio.begin()) {
    Serial.println("NRF24 NO detectado");
    while (1);
  }

  radio.setChannel(76);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);

  radio.openReadingPipe(0, direccion);
  radio.startListening();

  Serial.println("Receptor listo");
}

void loop() {
  if (radio.available()) {
    char texto[32] = "";
    radio.read(&texto, sizeof(texto));

    if (strcmp(texto, "TOGGLE") == 0) {
      estadoLED = !estadoLED;
      digitalWrite(LED, estadoLED);

      Serial.println("LED cambiado de estado");
    }
  }
}