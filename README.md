
# 💧 SafeRain — Sistema Inteligente de Monitoramento e Prevenção de Enchentes

---

## 📝 Descrição do Problema

  As enchentes são eventos frequentes em muitas regiões e causam grandes prejuízos, como perdas materiais, impactos na infraestrutura e, em casos mais graves, risco à vida humana. 
A falta de monitoramento em tempo real e de sistemas preventivos agrava a situação, dificultando a resposta rápida e a tomada de decisões. Além disso, a ausência de dados históricos dificulta o planejamento e a prevenção de futuras ocorrências.

---

## 🎯 Visão Geral da Solução

  Esta solução ajuda na prevenção de enchentes por meio de um monitoramento simples, acessível e automatizado. Com alertas em tempo real e acionamento automático de medidas de segurança, o sistema protege vidas e reduz danos materiais.
  Além disso, a coleta contínua de dados ambientais permite identificar padrões e prever futuras ocorrências, contribuindo para um planejamento urbano mais seguro e eficiente.
O projeto monitora o nível da água em um reservatório, detecta a intensidade da chuva, mede temperatura e umidade do ar, e exibe essas informações em um display LCD. Utiliza LEDs e buzzer para alertar sobre o risco de enchentes e aciona automaticamente uma comporta com servo motor quando o nível da água está alto.

### Principais Funcionalidades

- 📏 Medição do nível da água com sensor ultrassônico (HC-SR04)
- 🌧️ Medição da intensidade da chuva com sensor analógico
- 🌡️ Monitoramento de temperatura e umidade com DHT22
- ⏰ Registro de data e hora com módulo RTC (DS1307)
- 📺 Exibição de dados em display LCD 16x2 com I2C
- 🚦 Alerta visual com LEDs (verde, amarelo, vermelho)
- 🔊 Alerta sonoro com buzzer em caso de risco
- 🚪 Abertura automática de comporta com servo motor
- 💧 Animação personalizada de inicialização no LCD

---

## 🧰 Componentes Utilizados

| Componente                 | Função                                   |
|----------------------------|------------------------------------------|
| Arduino Uno / Nano         | Unidade de controle                      |
| Sensor Ultrassônico HC-SR04| Medir nível da água                      |
| Sensor de Chuva            | Detecção de intensidade de chuva         |
| Sensor DHT22               | Leitura de temperatura e umidade         |
| RTC DS1307                 | Registro de tempo real                   |
| LCD 16x2 com I2C           | Interface de usuário                     |
| LEDs                       | Alerta visual (segurança / atenção / risco) |
| Buzzer                     | Alerta sonoro                            |
| Servo Motor (SG90)         | Abertura da comporta                     |
| Protoboard + Jumpers       | Conexões                                 |

---

## 📷 Ilustração do Projeto

![WhatsApp Image 2025-06-05 at 10 34 36](https://github.com/user-attachments/assets/c013fa23-57b3-4352-8aac-4967b64228c4)


---

## 🧪 Como Simular no Wokwi

### ✅ Wokwi 

1. Acesse o link: 👉 [Simulação no Wokwi](https://wokwi.com/projects/432792167470635009)
2. Clique em "Start Simulation"
3. Observe os dados no LCD, LEDs e funcionamento do servo
4. Utilize o controle do sensor ultrassônico para simular o aumento do nível de água
5. Verifique a atuação do buzzer e abertura da comporta


## ▶️ Demonstração em Vídeo

Assista à demonstração completa do projeto funcionando em tempo real:

🎥 [Clique aqui para assistir no YouTube](https://youtu.be/7SdYCLk2rVk)

---

## 🚀 Como Usar

### 1. Instale as bibliotecas necessárias na IDE Arduino:

```cpp
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <DHT.h>
#include <Wire.h>
#include <RTClib.h>
````

### 2. Configure o RTC (DS1307)

Ajusta a data/hora com o tempo da compilação:

```cpp
if (!rtc.isrunning()) {
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}
```

### 3. Conecte os pinos corretamente:

| Componente      | Pino do Arduino |
| --------------- | --------------- |
| Trigger HC-SR04 | 7               |
| Echo HC-SR04    | 6               |
| LED Verde       | 8               |
| LED Amarelo     | 10              |
| LED Vermelho    | 13              |
| Buzzer          | 12              |
| Servo Motor     | 2               |
| Sensor de Chuva | A0              |
| DHT22           | A1              |
| LCD I2C         | SDA/SCL (A4/A5) |

---

## 📊 Lógica de Níveis

| Nível da Água (%) | LED      | Ação                           |
| ----------------- | -------- | ------------------------------ |
| 0% a 30%          | Verde    | Seguro                         |
| 31% a 69%         | Amarelo  | Atenção                        |
| 70% a 100%        | Vermelho | Risco: buzzer + abrir comporta |

---

## 👨‍💻 Autoras

**Giovana Gaspar Larocca e Rayssa luzia Portela Aquino**
📍 São Paulo - Brasil

---

