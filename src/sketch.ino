#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 21
#define DHTTYPE DHT22
#define RELAY_PIN 19
#define BUZZER_PIN 18

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void connectWiFi() {
  Serial.print("Conectando ao WiFi");
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

void connectMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  while (!client.connected()) {
    Serial.println("Conectando ao MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("MQTT conectado!");
    } else {
      Serial.print("Falha rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 1s");
      delay(1000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  connectWiFi();
  connectMQTT();
  Serial.println("Sistema inicializado.");
}

void loop() {
  if (!client.connected()) connectMQTT();
  client.loop();

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) return;

  Serial.printf("Temp: %.1f°C | Umid: %.1f%%\n", temp, hum);

  static bool humidOn = false;
  if (!humidOn && hum < 45.0) {
    humidOn = true; digitalWrite(RELAY_PIN, HIGH);
  } else if (humidOn && hum >= 55.0) {
    humidOn = false; digitalWrite(RELAY_PIN, LOW);
  }

  if (temp < 36.0 || temp > 37.5) {
    for (int i = 0; i < 5; i++) {
      tone(BUZZER_PIN, 1000); delay(150);
      noTone(BUZZER_PIN); delay(100);
    }
  }

  char payload[100];
  snprintf(payload, sizeof(payload), "{\"temp\": %.1f, \"hum\": %.1f}", temp, hum);
  client.publish("bebes/monitor", payload);

  delay(2000);
}
