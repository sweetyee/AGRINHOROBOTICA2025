#include "esp_camera.h"
#include <WiFi.h>

#define CAMERA_MODEL_AI_THINKER

// ===== CONFIGURAÇÃO DO WIFI =====
const char* ssid = "Minha Mãe 2.4 g";   
const char* password = "Pipe2308";       

// === DEFINIÇÕES DE PINOS PARA MÓDULO AI THINKER ===
#if defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27

  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22
#else
  #error "Camera model not selected"
#endif

// === CONTROLE DE MOTORES E LED ===
int gpLb = 2;   // Esquerda - trás
int gpLf = 14;  // Esquerda - frente
int gpRb = 15;  // Direita - trás
int gpRf = 13;  // Direita - frente
int gpLed = 4;  // LED
String WiFiAddr = "";

// === PROTÓTIPO DO SERVIDOR ===
void startCameraServer();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("\nInicializando...");

  // === CONFIGURAÇÃO DOS PINOS DE CONTROLE ===
  pinMode(gpLb, OUTPUT);
  pinMode(gpLf, OUTPUT);
  pinMode(gpRb, OUTPUT);
  pinMode(gpRf, OUTPUT);
  pinMode(gpLed, OUTPUT);

  digitalWrite(gpLb, LOW);
  digitalWrite(gpLf, LOW);
  digitalWrite(gpRb, LOW);
  digitalWrite(gpRf, LOW);
  digitalWrite(gpLed, LOW);

  // === CONFIGURAÇÃO DA CÂMERA ===
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Falha ao iniciar a câmera: erro 0x%x", err);
    return;
  }

  // Reduz a resolução para inicialização mais rápida
  sensor_t *s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_CIF);

  // === CONECTANDO AO WI-FI COM TIMEOUT ===
  Serial.printf("Conectando ao Wi-Fi \"%s\"", ssid);
  WiFi.begin(ssid, password);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Wi-Fi conectado!");
    Serial.println("Iniciando servidor da câmera...");
    startCameraServer();

    Serial.print("📷 Acesse: http://");
    Serial.println(WiFi.localIP());
    WiFiAddr = WiFi.localIP().toString();
  } else {
    Serial.println("\n❌ Falha ao conectar no Wi-Fi!");
  }
}

void loop() {
  // Código principal
}
