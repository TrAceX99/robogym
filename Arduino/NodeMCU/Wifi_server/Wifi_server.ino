#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define LED D0

const char* ssid = "NodeMCU";

ESP8266WebServer server(80);

void handleRoot() {
  digitalWrite(LED, server.arg("led").toInt());
  
  int ledState = digitalRead(LED);
  int ledTarget = (ledState + 1) % 2;
  
  char html[1000];

  sprintf(html, "<html>\
  <head>\
    <title>NodeMCU control</title>\
    <style>\
    body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; font-size: 1.5em; Color: #000000; }\
    h1 { Color: #AA0000; }\
    </style>\
  </head>\
  <body>\
    <\div align=center>\
    <h1>NodeMCU control page</h1>\
    <p>LED state: %d</p>\
    <br>\
    <a href=\"/?led=%d\"><button style=\"height:200px; width:400px\">Switch</button></a>\
    </\div>\
  </body>\
</html>", ledState, ledTarget);

  server.send(200, "text/html", html);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
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
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);
  delay(500);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("NodeMCU");

  server.on("/", handleRoot);
  server.on("/led=1", handleRoot);
  server.on("/led=2", handleRoot);
  server.on("/test", [](){
    server.send(200, "text/plain", "moze i ovako..");
  } );
  server.onNotFound(handleNotFound);

  server.begin();
}

void loop() {
  server.handleClient();
}
