#include <Wire.h>
#include <Adafruit_PN532.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define PIN_MAQUINARIA 4 // LED interno o salida a relé

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

// --- AQUÍ SE DEFINE EL CÓDIGO DE LA TARJETA AUTORIZADA ---
// Sustituye estos valores con los que obtuviste en la práctica anterior
uint8_t llaveMaestra[] = { 0x4A, 0xF2, 0x08, 0x1B }; 

void setup(void) {
  Serial.begin(115200);
  pinMode(PIN_MAQUINARIA, OUTPUT);
  digitalWrite(PIN_MAQUINARIA, LOW); // Maquinaria apagada al inicio

  nfc.begin();
  nfc.SAMConfig();
  Serial.println("SISTEMA DE CONTROL LISTO. Acerque credencial...");
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    bool accesoGarantizado = true;

    Serial.println("\n--- VALIDANDO CREDENCIAL ---");
    
    for (uint8_t i = 0; i < uidLength; i++) {
      // Impresión en las 3 bases requeridas
      Serial.print("BYTE "); Serial.print(i);
      Serial.print(" -> HEX: "); Serial.print(uid[i], HEX);
      Serial.print(" | DEC: "); Serial.print(uid[i], DEC);
      Serial.print(" | BIN: "); Serial.println(uid[i], BIN);

      // Comparación lógica con la llave maestra
      if (uid[i] != llaveMaestra[i]) {
        accesoGarantizado = false;
      }
    }

    if (accesoGarantizado) {
      Serial.println(">> ACCESO CONCEDIDO: Iniciando operaciones...");
      digitalWrite(PIN_MAQUINARIA, HIGH); // "Enciende" la maquinaria
      delay(3000); // Mantiene encendido por 3 segundos
      digitalWrite(PIN_MAQUINARIA, LOW);
      Serial.println(">> Operación finalizada. Sistema bloqueado.");
    } else {
      Serial.println(">> ERROR: Credencial no válida. Acceso denegado.");
    }
    
    Serial.println("-------------------------------------------");
    delay(1000);
  }
}