
#include "ArduinoJson.h"
#include "EspMQTTClient.h"

// MQTT Configuracoes
EspMQTTClient client{
  "Wokwi-GUEST", //SSID do WiFi
  "",     // Senha do wifi
  "mqtt.tago.io",  // Endereço do servidor
  "Default",       // Usuario
  "31d7009c-2b6c-4a0e-a4e0-3a6bf822cb14",         // Token do device
  "SmartTrashCan",           // Nome do device
  1883             // Porta de comunicação
};

const int maxSequenceLength = 14;
char sequence[maxSequenceLength];
int sequenceIndex = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Conectando WiFi");
  while (!client.isWifiConnected()) {
    Serial.print('.'); client.loop(); delay(1000);
  }
  Serial.println("WiFi Conectado");
  Serial.println("Conectando com Servidor MQTT");
  while (!client.isMqttConnected()) {
    Serial.print('.'); client.loop(); delay(1000);
  }
  Serial.println("MQTT Conectado");
}

void onConnectionEstablished()
{}

void loop() {
  send_code();
}

void send_code(void) {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();
    
    // Check if the received character is a digit (0-9)
    if (isdigit(incomingByte)) {
      // Add the digit to the sequence if there's space
      if (sequenceIndex < maxSequenceLength - 1) {
        sequence[sequenceIndex] = incomingByte;
        sequenceIndex++;
      }
    } else if (sequenceIndex == maxSequenceLength - 1) {
      // If 13 digits are received, send the data
      sequence[sequenceIndex] = '\0'; // Null-terminate the sequence
      String sequencestr = String(sequence); // Convert char array to String
      
      printReceivedSequence(sequencestr);
      sendDataToTago(sequencestr);

      sequenceIndex = 0;
    }
  }
}

void printReceivedSequence(String sequencestr) {
  Serial.println("Received Sequence: " + sequencestr);
}



void sendDataToTago(String data) {

  char bufferJson[100];
  int temperatura = 0;
  
  temperatura= random(0,100);
  StaticJsonDocument<300> documentoJson;
  documentoJson["variable"] = "codbarras";
  documentoJson["value"] = data;
  documentoJson["unit"] = "code";
  serializeJson(documentoJson, bufferJson);
  Serial.println(bufferJson);
  client.publish("topicoTDSPI", bufferJson);
  delay(5000);
  //client.loop();



}
