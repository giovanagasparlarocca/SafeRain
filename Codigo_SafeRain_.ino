#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <DHT.h>
#include <Wire.h>
#include <RTClib.h>

// Definições de pinos
#define trig 7
#define echo 6
#define red 13
#define yell 10
#define green 8
#define buzz 12
#define servoPin 2
#define chuvaPin A0
#define dhtPin A1
#define DHTTYPE DHT22

// Objetos
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo gate;
DHT dht(dhtPin, DHTTYPE);
RTC_DS1307 rtc;

// Variáveis
const int alturaMaxima = 400; // 400 cm = reservatório cheio
unsigned long tempoAnterior = 0;
int tela = 0;

// Cria caractere de gota preenchida
void criaChars() {
  byte gotaPreenchida[8] = {
    0b00100,
    0b01110,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000
  };
  lcd.createChar(0, gotaPreenchida);
}

// Animação inicial com gotas alinhadas ao texto
void animacaoSafeRain() {
  criaChars();
  const char* texto = "SafeRain";
  int len = 8;
  int startCol = 4;  // centralizado em display 16x2

  lcd.clear();

  // Anima letra a letra com gotas alinhadas
  for (int i = 0; i <= len; i++) {
    lcd.setCursor(startCol, 0);
    for (int j = 0; j < i; j++) {
      lcd.print(texto[j]);
    }
    for (int k = i; k < len; k++) {
      lcd.print(" ");
    }

    // Linha inferior: gotas alinhadas com texto
    lcd.setCursor(0, 1);
    lcd.print("               "); // limpa linha

    for (int g = 0; g < i; g++) {
      lcd.setCursor(startCol + g, 1);
      lcd.write(byte(0)); // gota cheia
    }

    delay(250);
  }

  // Piscar a palavra com as gotas por 3 vezes
  for (int n = 0; n < 3; n++) {
    lcd.clear();
    delay(300);

    lcd.setCursor(startCol, 0);
    lcd.print("SafeRain");

    for (int g = 0; g < len; g++) {
      lcd.setCursor(startCol + g, 1);
      lcd.write(byte(0));
    }

    delay(400);
  }

  // Apagar letra por letra
  for (int i = len - 1; i >= 0; i--) {
    lcd.setCursor(startCol + i, 0);
    lcd.print(" ");
    lcd.setCursor(startCol + i, 1);
    lcd.print(" ");
    delay(150);
  }

  lcd.clear();
  delay(300);
}

void gL() {
  digitalWrite(red, LOW);
  digitalWrite(yell, LOW);
  digitalWrite(green, HIGH);
}

void yL() {
  digitalWrite(red, LOW);
  digitalWrite(yell, HIGH);
  digitalWrite(green, LOW);
}

void rL() {
  digitalWrite(red, HIGH);
  digitalWrite(yell, LOW);
  digitalWrite(green, LOW);
}

void beep() {
  for (int i = 0; i < 3; i++) {
    tone(buzz, 440);
    delay(100);
    noTone(buzz);
    delay(100);
  }
}

void noTurn() {
  gate.write(0);
}

void openD() {
  gate.write(180);
}

void setup() {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(red, OUTPUT);
  pinMode(yell, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(buzz, OUTPUT);
  pinMode(chuvaPin, INPUT);

  gate.attach(servoPin);
  dht.begin();
  rtc.begin();

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  animacaoSafeRain();
}

void loop() {
  // Medir distância
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duracao = pulseIn(echo, HIGH);
  int distancia = duracao * 0.034 / 2;

  // 0cm = 0%, 400cm = 100%
  int nivelAgua = map(distancia, 0, alturaMaxima, 0, 100);
  nivelAgua = constrain(nivelAgua, 0, 100);

  // Sensor de chuva
  int leituraChuva = analogRead(chuvaPin);
  int intensidadeChuva = map(leituraChuva, 0, 900, 9, 1);
  intensidadeChuva = constrain(intensidadeChuva, 1, 9);

  // Sensor DHT22
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  // Data e hora
  DateTime agora = rtc.now();

  // Debug serial - tudo em uma linha
  Serial.print("Data: ");
  if (agora.day() < 10) Serial.print("0");
  Serial.print(agora.day());
  Serial.print("/");
  if (agora.month() < 10) Serial.print("0");
  Serial.print(agora.month());
  Serial.print("/");
  Serial.print(agora.year());
  Serial.print(" | Hora: ");
  if (agora.hour() < 10) Serial.print("0");
  Serial.print(agora.hour());
  Serial.print(":");
  if (agora.minute() < 10) Serial.print("0");
  Serial.print(agora.minute());
  Serial.print(":");
  if (agora.second() < 10) Serial.print("0");
  Serial.print(agora.second());

  Serial.print(" | Agua: ");
  Serial.print(nivelAgua);
  Serial.print("%");

  Serial.print(" | Chuva: ");
  Serial.print(intensidadeChuva);

  Serial.print(" | Temp: ");
  Serial.print(temperatura);
  Serial.print("C");

  Serial.print(" | Umid: ");
  Serial.print(umidade);
  Serial.println("%");

  // Controle de LEDs e buzzer
  if (nivelAgua <= 30) {  // baixo - tudo ok
    noTone(buzz);
    noTurn();
    gL();
  }
  else if (nivelAgua > 30 && nivelAgua < 70) { // médio - atenção
    noTone(buzz);
    noTurn();
    yL();
  }
  else { // alto - perigo, abrir comportas
    beep();
    rL();
    openD();
  }

  // Alternar informações no display a cada 3 segundos
  if (millis() - tempoAnterior >= 3000) {
    lcd.clear();
    tempoAnterior = millis();

    if (tela == 0) {
      lcd.setCursor(0, 0);
      lcd.print("Hora:");
      lcd.print(agora.hour() < 10 ? "0" : "");
      lcd.print(agora.hour());
      lcd.print(":");
      lcd.print(agora.minute() < 10 ? "0" : "");
      lcd.print(agora.minute());

      lcd.setCursor(0, 1);
      lcd.print("Data:");
      lcd.print(agora.day() < 10 ? "0" : "");
      lcd.print(agora.day());
      lcd.print("/");
      lcd.print(agora.month() < 10 ? "0" : "");
      lcd.print(agora.month());
      lcd.print("/");
      lcd.print(agora.year());
    }

    else if (tela == 1) {
      lcd.setCursor(0, 0);
      lcd.print("Nivel de Agua:");
      lcd.setCursor(0, 1);
      lcd.print(nivelAgua);
      lcd.print("%");
    }

    else if (tela == 2) {
      lcd.setCursor(0, 0);
      lcd.print("Chuva Nivel:");
      lcd.setCursor(0, 1);
      lcd.print(intensidadeChuva);
    }

    else if (tela == 3) {
      lcd.setCursor(0, 0);
      lcd.print("Temp:");
      lcd.print(temperatura);
      lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("Umid:");
      lcd.print(umidade);
      lcd.print("%");
    }

    tela = (tela + 1) % 4;
  }

  delay(200);
}
