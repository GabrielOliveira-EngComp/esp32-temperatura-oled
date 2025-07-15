// === Bibliotecas ===
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// === Wi-Fi ===
const char* ssid     = "Nome da rede";
const char* password = "Senha da rede";
WiFiClient espClient;

// === MQTT ===
const char* mqtt_Broker   = "iot.eclipse.org";      // altere se desejar
const char* mqtt_ClientID = "esp32-thermo-gabriel";
const char* topTemp = "gabriel/temperatura";
const char* topUmid = "gabriel/umidade";
PubSubClient client(espClient);

// === DHT22 ===
#define DHTPIN 4            // GPIO seguro
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float temperatura = NAN;
float umidade     = NAN;

// === OLED ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_SDA   18
#define OLED_SCL   46
TwoWire I2COLED = TwoWire(0);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2COLED, OLED_RESET);

// === Temporização ===
unsigned long ultimaTentativaMQTT = 0;
const unsigned long intervaloMQTT = 30000;   // 30 s
unsigned long ultimoRefresh       = 0;
const unsigned long intervaloLoop = 1500;    // 1,5 s

// ---------------------------------------------------------------------------
// SETUP
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("=== Iniciando ===");

  I2COLED.begin(OLED_SDA, OLED_SCL, 400000);
  configurarDisplay();
  dht.begin();

  conectarWiFi();
  client.setServer(mqtt_Broker, 1883);
}

// ---------------------------------------------------------------------------
// LOOP
// ---------------------------------------------------------------------------
void loop() {
  // 1) Reconecta Wi-Fi se cair
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }

  // 2) Tenta MQTT de 30 em 30 s (não bloqueia)
  if (!client.connected() && millis() - ultimaTentativaMQTT > intervaloMQTT) {
    ultimaTentativaMQTT = millis();
    reconectarMQTT();
  }
  client.loop();   // mantém pilha MQTT viva

  // 3) Leitura + display + MQTT a cada 1,5 s
  if (millis() - ultimoRefresh > intervaloLoop) {
    ultimoRefresh = millis();
    medirSensor();
    mostrarDisplay();
    publicarMQTT();
  }
}

// ---------------------------------------------------------------------------
// FUNÇÕES
// ---------------------------------------------------------------------------
void conectarWiFi() {
  Serial.print("Wi-Fi: "); Serial.println(ssid);

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  byte tent = 0;
  while (WiFi.status() != WL_CONNECTED && tent < 20) { // ~10 s
    delay(500);
    Serial.print(".");
    tent++;
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Wi-Fi OK");
    display.print("WiFi OK");
  } else {
    Serial.println("\n❌ Wi-Fi Fail");
    display.print("WiFi Fail");
  }
  display.display();
  delay(800);
}

void reconectarMQTT() {
  Serial.print("MQTT… ");
  if (client.connect(mqtt_ClientID)) {
    Serial.println("conectado!");
    displayMQTT(true);      // mostra apenas OK
  } else {
    Serial.print("falhou (");
    Serial.print(client.state());
    Serial.println(")");
    // não exibe nada no display quando falha
  }
}

// Mostra “MQTT OK” somente quando conecta
void displayMQTT(bool ok) {
  if (!ok) return;                // se falhou, não mostra nada
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("MQTT OK");
  display.display();
  delay(800);
}

void configurarDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("❌ Erro OLED");
    while (true);
  }
  display.cp437(true);            // habilita ° (código 248)
  display.setTextColor(WHITE);
  display.clearDisplay();
}

void medirSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("⚠️ Erro DHT22");
    temperatura = umidade = NAN;
  } else {
    temperatura = t;
    umidade     = h;
    Serial.printf("🌡 %.1f°C | 💧 %.1f%%\n", temperatura, umidade);
  }
}

void mostrarDisplay() {
  display.clearDisplay();

  if (isnan(temperatura) || isnan(umidade)) {
    display.setTextSize(1);
    display.setCursor(10, 25);
    display.print("Erro DHT22");
    display.display();
    return;
  }

  // Títulos
  display.setTextSize(1);
  display.setCursor(8, 0);
  display.print("Temp");
  display.setCursor(80, 0);
  display.print("Umid");

  // Divisor
  display.drawLine(64, 0, 64, 64, WHITE);

  // Valores
  display.setTextSize(2);
  display.setCursor(6, 25);
  display.print((int)temperatura);
  display.setTextSize(1);
  display.write(248);  // °
  display.print("C");

  display.setTextSize(2);
  display.setCursor(78, 25);
  display.print((int)umidade);
  display.setTextSize(1);
  display.print("%");

  display.display();
}

void publicarMQTT() {
  if (client.connected() && !isnan(temperatura) && !isnan(umidade)) {
    client.publish(topTemp, String(temperatura, 1).c_str(), true);
    client.publish(topUmid, String(umidade, 1).c_str(), true);
  }
}
