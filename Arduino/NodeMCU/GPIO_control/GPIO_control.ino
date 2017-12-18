#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include "FS.h"

#define LED D0

char* ssid = "Hardwired-To-Self-Destruct";
char* pass = "metallica";
const char* apssid = "ESP8266";
const char* appass = "test464646";

String dataserverIP = "192.168.0.101";

ESP8266WebServer server(80);
HTTPClient http;

void handleRoot() {
  if(server.hasArg("ip")) dataserverIP = server.arg("ip");
  Serial.println("Root request, Dataserver IP: " + dataserverIP);
  http.begin("http://" + dataserverIP + "/index.html");
  int httpCode = http.GET();
  
  if (httpCode > 0) {
    Serial.println("GET request ok\n");
    String html = http.getString();

    String pins = "";
    for (int i = 0; i < 17; i++) {
      if (digitalRead(i)) pins += String(i) + ",";
    }
    html.replace("%dummystring%", pins);
    html.replace("%ipaddress%", dataserverIP);
    
    server.send(200, "text/html", html);
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    server.sendHeader("Location", "/redirect", true);
    server.send(307, "text/plain", "");
    Serial.println("Redirected\n");
  }
}

void handleRedirect() {
  File f = SPIFFS.open("/redirect.html", "r");
  if (!f) Serial.println("File open failed!");
  String html = f.readString();
  server.send(200, "text/html", html);
  f.close();
}

void handleOffline() {
  Serial.println("Offline mode started");
  File f = SPIFFS.open("/offline.html", "r");
  if (!f) Serial.println("File open failed!");
  String html = f.readString();
  server.send(200, "text/html", html);
  f.close();
}

void handleSubmit() {
  for (uint8_t i = 0; i < server.args(); i++) {
    int pin = server.argName(i).toInt();
    int state = server.arg(i).toInt();
    digitalWrite(pin, state);
    Serial.println("Set pin " + String(pin) + " to " + String(state));
  }
  server.send(200, "text/plain", "Ako vidis ovo svaka cast");
}

void testSubmit() {
  Serial.print(server.method());
  Serial.print(" request. Data:\n");
  for (uint8_t i = 0; i < server.args(); i++) {
    Serial.println(server.argName(i) + " = " + server.arg(i));
  }
}

void handleConnect() {
  /*if (server.hasArg("ssid") && server.hasArg("pass")) {
    ssid = server.arg("ssid");
    pass = server.arg("pass");
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
    }
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Invalid arguments!\n");
  }*/
  server.sendHeader("Location:", "/redirect", true);
  server.send(307, "text/plain", "");
}

void handleNotFound() {
  String message = "File Not Found 404\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );

}

void setup() {
  Serial.begin(115200);
  Serial.println("");
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 1);
  if(!SPIFFS.begin()) Serial.println("Error: SPIFFS not started!");;
  delay(500);

  Serial.println("Connecting to ");
  Serial.println(ssid);
  //WiFi.mode(WIFI_AP_STA);
  //WiFi.softAP(ssid);
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  //Serial.println("AP online");
  delay(500);
  Serial.print("IP: ");
  //Serial.println(WiFi.softAPIP());
  Serial.println(WiFi.localIP());
  Serial.println("Configuring server");
  delay(500);
  
  server.on("/", handleRoot);
  server.on("/test", testSubmit);
  server.on("/submit", HTTP_POST, handleSubmit);
  server.on("/redirect", handleRedirect);
  server.on("/offline", handleOffline);
  server.on("/connect", handleConnect);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Server started!\n");
}

void loop() {
  server.handleClient();
}
