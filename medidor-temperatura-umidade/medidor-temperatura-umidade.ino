// --- WIFI ---
#include <WiFi.h>
const char* ssid = "Nome da sua rede";        // troque pelo nome da sua rede
const char* password = "Senha da sua rede";   // troque pela senha da sua rede
WiFiClient espClient;

// --- MQTT ---
#include <PubSubClient.h>
const char* mqtt_Broker = "iot.eclipse.org";
const char* mqtt_ClientID = "esp32-thermo-gabriel";
PubSubClient client(espClient);
const char* topicoTemperatura = "gabriel/temperatura";
const char* topicoUmidade = "gabriel/umidade";

// --- DHT22 ---
#include <DHT.h>
#define DHTPIN 9
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float temperatura = 0;
float umidade = 0;

// --- DISPLAY OLED ---
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_SDA 18
#define OLED_SCL 46
TwoWire I2COLED = TwoWire(0);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2COLED, OLED_RESET);

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando sistema...");

  I2COLED.begin(OLED_SDA, OLED_SCL, 400000);
  configurarDisplay();
  dht.begin();

  conectarWifi();
  client.setServer(mqtt_Broker, 1883);
}

void loop() {
  if (!client.connected()) {
    reconectarMQTT();
  }

  client.loop();  // importante para manter a conexão ativa

  medirTemperaturaUmidade();
  mostrarTemperaturaUmidade();
  publicarTemperaturaUmidade();
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

void reconectarMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect(mqtt_ClientID)) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Erro: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
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

void mostrarMensagemNoDisplay(const char* texto1, float valor, const char* unidade) {
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

void publicarTemperaturaUmidade() {
  client.publish(topicoTemperatura, String(temperatura).c_str(), true);
  client.publish(topicoUmidade, String(umidade).c_str(), true);
}
