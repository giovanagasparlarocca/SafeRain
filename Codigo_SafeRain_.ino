#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <DHT.h>
#include <Wire.h>
#include <RTClib.h>
#include <EEPROM.h>

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
const int alturaMaxima = 400;
unsigned long tempoAnterior = 0;
int tela = 0;

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

void animacaoSafeRain() {
  criaChars();
  const char* texto = "SafeRain";
  int len = 8;
  int startCol = 4;

  lcd.clear();
  for (int i = 0; i <= len; i++) {
    lcd.setCursor(startCol, 0);
    for (int j = 0; j < i; j++) lcd.print(texto[j]);
    for (int k = i; k < len; k++) lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print("               ");
    for (int g = 0; g < i; g++) {
      lcd.setCursor(startCol + g, 1);
      lcd.write(byte(0));
    }
    delay(250);
  }

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

void salvarDadosNaEEPROM(int agua, int chuva, float temp, float umid, DateTime agora) {
  int endereco = 0;
  EEPROM.put(endereco, agua);        endereco += sizeof(agua);
  EEPROM.put(endereco, chuva);       endereco += sizeof(chuva);
  EEPROM.put(endereco, temp);        endereco += sizeof(temp);
  EEPROM.put(endereco, umid);        endereco += sizeof(umid);
  EEPROM.put(endereco, agora.year());endereco += sizeof(int);
  EEPROM.put(endereco, agora.month());endereco += sizeof(int);
  EEPROM.put(endereco, agora.day()); endereco += sizeof(int);
  EEPROM.put(endereco, agora.hour());endereco += sizeof(int);
  EEPROM.put(endereco, agora.minute());endereco += sizeof(int);
  EEPROM.put(endereco, agora.second());
}

void mostrarDadosSalvosSerial() {
  int endereco = 0;
  int agua, chuva;
  float temp, umid;
  int ano, mes, dia, hora, minuto, segundo;

  EEPROM.get(endereco, agua); endereco += sizeof(agua);
  EEPROM.get(endereco, chuva); endereco += sizeof(chuva);
  EEPROM.get(endereco, temp);  endereco += sizeof(temp);
  EEPROM.get(endereco, umid);  endereco += sizeof(umid);
  EEPROM.get(endereco, ano);   endereco += sizeof(int);
  EEPROM.get(endereco, mes);   endereco += sizeof(int);
  EEPROM.get(endereco, dia);   endereco += sizeof(int);
  EEPROM.get(endereco, hora);  endereco += sizeof(int);
  EEPROM.get(endereco, minuto);endereco += sizeof(int);
  EEPROM.get(endereco, segundo);

  Serial.println("==== DADOS SALVOS NA EEPROM ====");
  Serial.print("Data: ");
  if (dia < 10) Serial.print("0"); Serial.print(dia); Serial.print("/");
  if (mes < 10) Serial.print("0"); Serial.print(mes); Serial.print("/");
  Serial.println(ano);

  Serial.print("Hora: ");
  if (hora < 10) Serial.print("0"); Serial.print(hora); Serial.print(":");
  if (minuto < 10) Serial.print("0"); Serial.print(minuto); Serial.print(":");
  if (segundo < 10) Serial.print("0"); Serial.print(segundo); Serial.println();

  Serial.print("Nivel de Agua: "); Serial.print(agua); Serial.println("%");
  Serial.print("Intensidade da Chuva: "); Serial.println(chuva);
  Serial.print("Temperatura: "); Serial.print(temp); Serial.println(" °C");
  Serial.print("Umidade: "); Serial.print(umid); Serial.println(" %");
  Serial.println("================================");
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
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duracao = pulseIn(echo, HIGH);
  int distancia = duracao * 0.034 / 2;
  int nivelAgua = map(distancia, 0, alturaMaxima, 0, 100);
  nivelAgua = constrain(nivelAgua, 0, 100);

  int leituraChuva = analogRead(chuvaPin);
  int intensidadeChuva = map(leituraChuva, 0, 900, 9, 1);
  intensidadeChuva = constrain(intensidadeChuva, 1, 9);

  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();
  DateTime agora = rtc.now();

  Serial.print("Data: ");
  if (agora.day() < 10) Serial.print("0"); Serial.print(agora.day()); Serial.print("/");
  if (agora.month() < 10) Serial.print("0"); Serial.print(agora.month()); Serial.print("/");
  Serial.print(agora.year());
  Serial.print(" | Hora: ");
  if (agora.hour() < 10) Serial.print("0"); Serial.print(agora.hour()); Serial.print(":");
  if (agora.minute() < 10) Serial.print("0"); Serial.print(agora.minute()); Serial.print(":");
  if (agora.second() < 10) Serial.print("0"); Serial.print(agora.second());
  Serial.print(" | Agua: "); Serial.print(nivelAgua); Serial.print("%");
  Serial.print(" | Chuva: "); Serial.print(intensidadeChuva);
  Serial.print(" | Temp: "); Serial.print(temperatura); Serial.print("C");
  Serial.print(" | Umid: "); Serial.print(umidade); Serial.println("%");

  if (nivelAgua <= 30) {
    noTone(buzz);
    noTurn();
    gL();
  } else if (nivelAgua < 70) {
    noTone(buzz);
    noTurn();
    yL();
  } else {
    beep();
    rL();
    openD();
    salvarDadosNaEEPROM(nivelAgua, intensidadeChuva, temperatura, umidade, agora);
  }

  if (millis() - tempoAnterior >= 3000) {
    lcd.clear();
    tempoAnterior = millis();

    if (tela == 0) {
      lcd.setCursor(0, 0);
      lcd.print("Hora:");
      lcd.print(agora.hour() < 10 ? "0" : ""); lcd.print(agora.hour()); lcd.print(":");
      lcd.print(agora.minute() < 10 ? "0" : ""); lcd.print(agora.minute());
      lcd.setCursor(0, 1);
      lcd.print("Data:");
      lcd.print(agora.day() < 10 ? "0" : ""); lcd.print(agora.day()); lcd.print("/");
      lcd.print(agora.month() < 10 ? "0" : ""); lcd.print(agora.month()); lcd.print("/");
      lcd.print(agora.year());
    } else if (tela == 1) {
      lcd.setCursor(0, 0);
      lcd.print("Nivel de Agua:");
      lcd.setCursor(0, 1);
      lcd.print(nivelAgua); lcd.print("%");
    } else if (tela == 2) {
      lcd.setCursor(0, 0);
      lcd.print("Chuva Nivel:");
      lcd.setCursor(0, 1);
      lcd.print(intensidadeChuva);
    } else if (tela == 3) {
      lcd.setCursor(0, 0);
      lcd.print("Temp:"); lcd.print(temperatura); lcd.print("C");
      lcd.setCursor(0, 1);
      lcd.print("Umid:"); lcd.print(umidade); lcd.print("%");
    }

    tela = (tela + 1) % 4;
  }

  // COMANDO SERIAL PARA "GET"
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();

    if (comando.equalsIgnoreCase("get")) {
      mostrarDadosSalvosSerial();
    }
  }

  delay(200);
}
