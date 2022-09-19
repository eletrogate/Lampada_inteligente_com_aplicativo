#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "*****"
#define STAPSK  "*****"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// Cria uma instância do servidor
// Especifica a porta
WiFiServer server(80);

void setup() {
  Serial.begin(9600);

  // prepara a porta da lampada
  pinMode(13, OUTPUT);
  digitalWrite(13, 0);

  // Connecta na rede do wifi
  Serial.println();
  Serial.println();
  Serial.print(F("Conectando com "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connectado"));

  // Inicia o servidor
  server.begin();
  Serial.println(F("Servidor iniciado"));

  // Escreve o endereço do IP
  Serial.println(WiFi.localIP());
}

void loop() {
  // Vê se um cliente conectou 
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println(F("novo cliente"));

  client.setTimeout(5000); 

  // Lê a primeira linha do pedido
  String req = client.readStringUntil('\r');
  Serial.println(F("pedido: "));
  Serial.println(req);
  client.flush();
  
  // Responde o pedido
  int val;
  if (req.indexOf(F("/gpio/0")) != -1) {
    val = 0;
  } else if (req.indexOf(F("/gpio/1")) != -1) {
    val = 1;
  } else {
    Serial.println(F("Pedido invalido"));
    val = digitalRead(13);
  }

  // Define a lampada de acordo com o pedido
  digitalWrite(13, val);

  // Lê o resto do pedido
  
  while (client.available()) {
    
    client.read();
  }

  // Manda a resposta para o cliente
  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nA lampada esta agora "));
  client.print((val) ? F("ligada") : F("desligada"));
  client.print(F("<br><br>Clique <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio/1'>Aqui</a> para ligar a lampada, ou <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio/0'>Aqui</a> para desligar a lampada.</html>"));

  
  Serial.println(F("Desconectando do cliente"));
}