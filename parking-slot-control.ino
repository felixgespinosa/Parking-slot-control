#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

// Pines definition
const int TriggerA = D5;
const int EchoA = D6;
const int TriggerB = D7;
const int EchoB = D8;

// Distance Constants
const int Max = 120;
const int Min = 60;

// Variables de estado
bool lastState = false;
bool currentState = false;

// Conexion to wifi variables
const char* ssid = "xxxxxxxx";
const char* password = "xxxxxxxxxxx";
WiFiClient client;

// Token y ID Telegram chat
const char* BOTtoken = "xxxxxxxx";
const char* CHAT_ID = "xxxxxxx";

void sendMessage(const char* message) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;

  String url = "https://api.telegram.org/";
  url += BOTtoken;
  url += "/sendMessage?chat_id=";
  url += CHAT_ID;
  url += "&text=";
  url += message;

  if (https.begin(client, url)) {
    int httpCode = https.GET();
    if (httpCode > 0) {
      Serial.println(https.getString());
    }
    https.end();
  } else {
    Serial.println("[HTTPS] Unable to connect");
  }
}

// Función para calcular la distancia
int getDistance(int trigger, int echo) {
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  return pulseIn(echo, HIGH) * 0.034 / 2;
}

// Función para verificar el estado del estacionamiento
bool checkParkingStatus() {
  int d1A = getDistance(TriggerA, EchoA);
  delay(500);
  int d2A = getDistance(TriggerA, EchoA);
  delay(500);
  int d1B = getDistance(TriggerB, EchoB);
  delay(500);
  int d2B = getDistance(TriggerB, EchoB);

  bool sensorRight = (d1A >= Min && d1A <= Max) && (d2A >= Min && d2A <= Max);
  bool sensorLeft = (d1B >= Min && d1B <= Max) && (d2B >= Min && d2B <= Max);
  return sensorRight && sensorLeft;
}

void setup() {
  pinMode(TriggerA, OUTPUT);
  pinMode(EchoA, INPUT);
  pinMode(TriggerB, OUTPUT);
  pinMode(EchoB, INPUT);

  Serial.begin(9600);
  delay(10);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a:\t");
  Serial.println(ssid); 

  // Esperar a que nos conectemos
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200);
  Serial.print(".");
  }

  // Mostrar mensaje de exito y dirección IP asignada
  Serial.println();
  Serial.print("Conectado a:\t");
  Serial.println(WiFi.SSID()); 
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  sendMessage("Conectado a la red");
}

void loop() {
  currentState = checkParkingStatus();

  if (currentState != lastState) {
    lastState = currentState;
    if (currentState) {
      sendMessage("------SE PUEDE APARCAR------");
    } else {
      sendMessage("------NO SE PUEDE APARCAR------");
    }
  }

  delay(10000);
}
