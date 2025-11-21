#include <DHT.h>

#define DHTPIN 21
#define DHTTYPE DHT22

#define RELAY_PIN 19
#define BUZZER_PIN 18

DHT dht(DHTPIN, DHTTYPE);

void beepAlarme() {
  for (int i = 0; i < 5; i++) {
    tone(BUZZER_PIN, 1000);
    delay(150);
    noTone(BUZZER_PIN);
    delay(100);
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  pinMode(BUZZER_PIN, OUTPUT);

  Serial.println("Sistema de monitoramento inicializado.");
}

void loop() {
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Erro: falha ao ler DHT22. Tentando novamente...");
    delay(1000);
    return;
  }

  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print(" °C | Umidade: ");
  Serial.print(umidade);
  Serial.println(" %");

  if (umidade < 50) {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Umidificador LIGADO (relé HIGH)");
  } else {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Umidificador DESLIGADO");
  }

  if (temperatura > 23) {
    Serial.println("ALARME: Temperatura fora da faixa segura!");
    beepAlarme();
  }

  Serial.println("-----------------------------");
  delay(2000);
}
