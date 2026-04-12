#include <Wire.h>
#include <Adafruit_PN532.h>

#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

void setup(void) {
  Serial.begin(115200);
  nfc.begin();
  nfc.SAMConfig(); // Configura el módulo para lectura de tarjetas
  Serial.println("Esperando tarjeta...");
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; 
  uint8_t uidLength;

  // Escanea buscando una tarjeta
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    for (uint8_t i = 0; i < uidLength; i++) {
      uint8_t data = uid[i];

      // --- EXPLICACIÓN DE SALIDAS ---
      
      // Base 16 (Hexadecimal): Formato estándar de sistemas
      Serial.print(" HEX: "); Serial.print(data, HEX);
      
      // Base 10 (Decimal): Formato numérico tradicional
      Serial.print(" | DEC: "); Serial.print(data, DEC);
      
      // Base 2 (Binario): Representación de bits puros (0 y 1)
      Serial.print(" | BIN: "); Serial.println(data, BIN);
    }
    delay(1000);
  }
}