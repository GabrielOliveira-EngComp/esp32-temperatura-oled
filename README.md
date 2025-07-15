# Monitor de Temperatura com ESP32, Display OLED e Conexão MQTT

Projeto desenvolvido com ESP32, sensor DHT11 e display OLED I2C, com envio dos dados de temperatura e umidade via protocolo MQTT para integração com outras aplicações (ex: dashboards, servidores, automações).

## 🔧 Componentes Utilizados

- ESP32
- Sensor DHT11 (Temperatura e Umidade)
- Display OLED 0.96" I2C (128x64)
- Conexão Wi-Fi
- Broker MQTT (local ou na nuvem)
- Protoboard e jumpers

## 🧠 Tecnologias e Ferramentas

- Linguagem C/C++
- IDE Arduino
- Bibliotecas: `Adafruit_SSD1306`, `DHT`, `PubSubClient`
- MQTT Broker: Mosquitto ou HiveMQ
- Plataforma: Windows + Arduino IDE

## 🎯 Objetivo do Projeto

Criar um sistema embarcado com ESP32 que:
- Realiza leitura da temperatura e umidade com o sensor DHT11
- Exibe as informações em tempo real no display OLED
- Envia os dados periodicamente via MQTT para um broker
- Permite integração com sistemas externos (ex: Node-RED, Home Assistant)

## 📡 Comunicação MQTT

O ESP32 publica os dados nos seguintes tópicos:

## 👨‍💻 Autor
- Gabriel Barbosa Fernandes de Oliveira
- Estudante de Engenharia de Computação – PUCPR
