#include <WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- CONFIG WIFI ---
const char* ssid = "Nome da sua rede";
const char* password = "Senha da sua rede";

// --- SENSOR DHT22 ---
#define DHTPIN 9
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// --- DISPLAY OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_SDA 18
#define OLED_SCL 46
TwoWire I2COLED = TwoWire(0);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2COLED, OLED_RESET);

// --- VARIÁVEIS ---
float temperatura = 0;
float umidade = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando sistema...");

  I2COLED.begin(OLED_SDA, OLED_SCL, 400000);
  configurarDisplay();

  dht.begin();
  conectarWifi();
}

void loop() {
  medirTemperaturaUmidade();
  mostrarTemperaturaUmidade();
  delay(5000);
}

void conectarWifi() {
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Conectando ");
  display.display();

  WiFi.begin(ssid, password);
  Serial.print("Conectando");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("WiFi OK");
  display.display();
  delay(1000);
}

void configurarDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Erro ao iniciar display");
    while (true);
  }
  display.setTextColor(WHITE);
  display.clearDisplay();
}

void medirTemperaturaUmidade() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("⚠️ Erro ao ler DHT22!");
    return;
  }

  umidade = h;
  temperatura = t;

  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");

  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.println(" %");
}

void mostrarTemperaturaUmidade() {
  mostrarMensagemNoDisplay("Temperatura", temperatura, " C");
  mostrarMensagemNoDisplay("Umidade", umidade, " %");
}

void mostrarMensagemNoDisplay(const char* texto1, int valor, const char* unidade) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(texto1);

  display.setTextSize(5);
  display.setCursor(20, 20);
  display.print(valor);

  display.setTextSize(2);
  display.print(unidade);

  display.display();
  delay(2000);
}
