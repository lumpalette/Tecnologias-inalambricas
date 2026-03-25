#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <SPI.h>
#include "RF24.h"

// --- CONFIGURACIÓN PANTALLA OLED SPI (7 PINES) ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_MOSI  23
#define OLED_CLK   18
#define OLED_DC    2
#define OLED_CS    14
#define OLED_RESET 15

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

// --- CONFIGURACIÓN BOTONES ---
#define UP_BUTTON_PIN     32
#define DOWN_BUTTON_PIN   33
#define SELECT_BUTTON_PIN 25
#define LED_PIN           2

// --- CONFIGURACIÓN NRF24 ---
SPIClass spiBus(VSPI);
RF24 radio(4, 5);
RF24 radio2(16, 17);

// --- ESTADOS ---
enum AppState { STATE_MENU, STATE_JAMMING };
AppState currentState = STATE_MENU;
int canalWifiActual = 1; // 1, 6 o 11

enum MenuItem { BT_JAM, WIFI_JAM, DRONE_JAM, TEST_RADIOS, NUM_MENU_ITEMS };
MenuItem selectedMenuItem = BT_JAM;


// --- BITMAPS ---
// Logo personalizado (128x64)
static const unsigned char PROGMEM mi_nuevo_logo_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x07, 0xc7, 0x00, 0x1f, 0xf8, 0x00, 0xe3, 0xe0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x0f, 0x03, 0x80, 0xff, 0xff, 0x01, 0xc0, 0xf0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x0e, 0x01, 0xc3, 0xff, 0xff, 0xc3, 0x80, 0x70, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0xe7, 0xff, 0xff, 0xe7, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x38, 0x01, 0xff, 0xff, 0xff, 0xff, 0x80, 0x1c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x0c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x07, 0xf1, 0xff, 0xff, 0x8f, 0xe0, 0x0c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x0f, 0xc0, 0x7f, 0xfe, 0x03, 0xf0, 0x0c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x0f, 0x80, 0x1f, 0xf8, 0x01, 0xf0, 0x0c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x1f, 0x00, 0x07, 0xe0, 0x00, 0xf8, 0x0c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x1e, 0x00, 0x03, 0xc0, 0x00, 0x78, 0x0c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x1e, 0x00, 0x03, 0xc0, 0x00, 0x78, 0x0c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x1f, 0x00, 0x03, 0xc0, 0x00, 0xf8, 0x0c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x0f, 0x80, 0x07, 0xe0, 0x01, 0xf0, 0x0c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x0f, 0xc0, 0x3f, 0xfc, 0x03, 0xf0, 0x0c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x07, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x0c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x0c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x18, 0x01, 0xff, 0xff, 0xff, 0xff, 0x80, 0x18, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x3f, 0x00, 0x00, 0xfc, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x1f, 0x80, 0x01, 0xf8, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x07, 0xf0, 0x0f, 0xe0, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x01, 0xff, 0xff, 0x80, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x3f, 0xfc, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


// ----------- NUEVO: SETUP RADIO -----------
void setupRadio(RF24 &r) {
  if (r.begin(&spiBus)) {
    r.setAutoAck(false);
    r.stopListening();
    r.setPALevel(RF24_PA_MAX);
    r.setDataRate(RF24_2MBPS);
    r.startConstCarrier(RF24_PA_MAX, 45);
  }
}

// ----------- NUEVO: ATAQUE -----------
void iniciarAtaque() {

  liberarBus(); // Limpiamos el estado de los pines
  
  digitalWrite(4, LOW); // Turno del Radio 1
  setupRadio(radio);
  digitalWrite(4, HIGH); // Descansa Radio 1

  digitalWrite(16, LOW); // Turno del Radio 2
  setupRadio(radio2);
  digitalWrite(16, HIGH); // Descansa Radio 2

  // Ahora, para actualizar la pantalla:
  digitalWrite(OLED_CS, LOW);
  display.clearDisplay();
  display.print("ATAQUE ACTIVO");
  display.display();
  digitalWrite(OLED_CS, HIGH); // Soltamos la pantalla

  if (selectedMenuItem == TEST_RADIOS) {
    display.clearDisplay();
    display.setCursor(10, 30);
    display.print("R1: "); display.print(radio.isChipConnected() ? "OK" : "ERR");
    display.setCursor(10, 45);
    display.print("R2: "); display.print(radio2.isChipConnected() ? "OK" : "ERR");
    display.display();
    delay(2000);
    drawMenu();
    return;
  }

  display.clearDisplay();
  // 1. Dibuja el logo (ajusta el -6 si quieres subirlo o bajarlo)
  display.drawBitmap(0, -6, mi_nuevo_logo_bits, 128, 64, SSD1306_WHITE);
  
  // 2. Crea una pequeña franja negra abajo para el texto de estado
  display.fillRect(0, 52, 128, 12, SSD1306_BLACK); 
  display.setCursor(10, 54);
  display.setTextColor(SSD1306_WHITE);
  display.print(">> ACTIVO - [SEL]");
  
  display.display();

  spiBus.begin(18, 19, 23);
  setupRadio(radio);
  setupRadio(radio2);

  digitalWrite(LED_PIN, HIGH);
  delay(800);

  currentState = STATE_JAMMING;

  display.clearDisplay();
  display.drawBitmap(0, -6, mi_nuevo_logo_bits, 128, 64, SSD1306_WHITE);
  display.fillRect(0, 50, 128, 14, SSD1306_BLACK);
  display.drawRect(0, 50, 128, 14, SSD1306_WHITE);
  display.setCursor(25, 53);
  display.print("ATAQUE ACTIVO");
  display.display();
  
  canalWifiActual = 1; // Resetear al entrar
}

void setup() {
  Serial.begin(115200);

  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SELECT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

    // 1. Inicializa el bus una vez
  //spiBus.begin(18, 19, 23); 

  // 2. IMPORTANTE: Pon los CS de los radios en HIGH para que no interrumpan a la pantalla
  pinMode(5, OUTPUT); digitalWrite(5, HIGH); 
  pinMode(17, OUTPUT); digitalWrite(17, HIGH);

  // 3. Inicializa pantalla
  if(!display.begin(SSD1306_SWITCHCAPVCC)) { 
     // error 
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("Error en OLED"));
    for(;;);
  }

  u8g2_for_adafruit_gfx.begin(display);

  showSplash();
  delay(3000);

  drawMenu();
}

void loop() {
  if (currentState == STATE_MENU) {
    handleMenuNavigation();
  } 
  else if (currentState == STATE_JAMMING) {

    if (selectedMenuItem == BT_JAM) {
      radio.setChannel(random(81));
      radio2.setChannel(random(81));
    } 
    else if (selectedMenuItem == WIFI_JAM) {
            int nrf_channel = 2; // Por defecto Canal 1 WiFi

      // Mapeo: WiFi 1 -> NRF 2 | WiFi 6 -> NRF 37 | WiFi 11 -> NRF 62
      if (canalWifiActual == 1)  nrf_channel = 2;
      else if (canalWifiActual == 6)  nrf_channel = 37;
      else if (canalWifiActual == 11) nrf_channel = 62;

      // --- ATAQUE ACTIVO ---
      radio.setChannel(nrf_channel);
      radio.startConstCarrier(RF24_PA_MAX, nrf_channel);

      radio2.setChannel(nrf_channel + 4); // Desfase para cubrir más banda
      radio2.startConstCarrier(RF24_PA_MAX, nrf_channel + 4);

      // --- CAMBIO DE CANAL EN VIVO ---
      // Usamos el botón DOWN para rotar entre 1, 6 y 11
      if (digitalRead(DOWN_BUTTON_PIN) == LOW) {
        if (canalWifiActual == 1) canalWifiActual = 6;
        else if (canalWifiActual == 6) canalWifiActual = 11;
        else canalWifiActual = 1;
        
        // Actualizamos pantalla inmediatamente al cambiar
        display.clearDisplay();
        display.drawBitmap(0, -6, mi_nuevo_logo_bits, 128, 64, SSD1306_WHITE);
        display.fillRect(0, 50, 128, 14, SSD1306_BLACK);
        display.drawRect(0, 50, 128, 14, SSD1306_WHITE);
        display.setCursor(15, 53);
        display.print("ATACANDO CH: "); display.print(canalWifiActual);
        display.display();
        
        delay(300); // Anti-rebote para no saltar canales de golpe
      }

      // Salida con RESTART (como ya lo tenías)
      if (digitalRead(SELECT_BUTTON_PIN) == LOW) {
          Serial.println("Reiniciando...");
          ESP.restart();
      }
    } 
    else if (selectedMenuItem == DRONE_JAM) {
      radio.setChannel(random(125));
      radio2.setChannel(random(125));
    }

    if (digitalRead(SELECT_BUTTON_PIN) == LOW) {
      radio.stopConstCarrier();
      radio2.stopConstCarrier();
      radio.powerDown();
      radio2.powerDown();
            // Feedback visual rápido
      display.clearDisplay();
      display.setCursor(20, 30);
      display.print("REINICIANDO...");
      display.display();
      
      delay(500); // Dar tiempo a que el usuario suelte el botón
      ESP.restart(); // <--- Esta línea fuerza el reinicio completo de la placa

      digitalWrite(LED_PIN, LOW);
      delay(300);

      currentState = STATE_MENU;
      drawMenu();
    }
  }
}

// --- MENÚ ---
void handleMenuNavigation() {
  if (digitalRead(UP_BUTTON_PIN) == LOW) {
    selectedMenuItem = (MenuItem)((selectedMenuItem == 0) ? (NUM_MENU_ITEMS - 1) : (selectedMenuItem - 1));
    drawMenu();
    delay(250);
  }

  if (digitalRead(DOWN_BUTTON_PIN) == LOW) {
    selectedMenuItem = (MenuItem)((selectedMenuItem + 1) % NUM_MENU_ITEMS);
    drawMenu();
    delay(250);
  }

  if (digitalRead(SELECT_BUTTON_PIN) == LOW) {
    delay(250);
    iniciarAtaque();
  }
}

void drawMenu() {
  display.clearDisplay();

  display.fillRect(0, 0, 128, 16, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(35, 4);
  display.print("HOME MENU");

  const char* labels[] = {"1. BT Jammer", "2. WiFi Jammer", "3. Drone Jammer", "4. Test Radios"};

  for (int i = 0; i < NUM_MENU_ITEMS; i++) {
    int yPos = 24 + (i * 12);

    if (i == selectedMenuItem) {
      display.fillRect(0, yPos - 2, 128, 12, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(10, yPos);
    display.print(labels[i]);
  }

  display.display();
}

void showSplash() {
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_adventurer_tr);
  u8g2_for_adafruit_gfx.setCursor(15, 35);
  u8g2_for_adafruit_gfx.print("Dokkaebi");
  display.display();
}

void liberarBus() {
  digitalWrite(OLED_CS, HIGH);  // Desactiva Pantalla
  digitalWrite(4, HIGH);        // Desactiva Radio 1 (CS pin)
  digitalWrite(16, HIGH);       // Desactiva Radio 2 (CS pin)
  delay(1); 
}