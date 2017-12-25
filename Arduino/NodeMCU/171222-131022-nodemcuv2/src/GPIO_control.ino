#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include "FS.h"
#include <ArduinoJson.h>

#define LED D0

String ssid = "ZTE_942B17";
String pass = "test464646";
const char* apssid = "ESP8266";
const char* appass = "";

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
			server.sendHeader("Location", "/menu", true);
			server.send(307, "text/plain", "");
			Serial.println("Redirected to menu\n");
		}
}

void handleMenu() {	
	http.begin("http://" + dataserverIP + "/menu.html");
	int httpCode = http.GET();
	
	if (httpCode > 0) {
		Serial.println("GET request to menu.html ok\n");
		String html = http.getString();
		server.send(200, "text/html", html);
	} else {
		Serial.printf("[REDIRECT] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
		Serial.println("Opening file from flash");
		//Change next line pls!!!!!!!!!!!!!!!!!!!!!!!!!!
		File f = SPIFFS.open("/menu.html", "r");
		if (!f) Serial.println("File open failed!");
		String html = f.readString();
		server.send(200, "text/html", html);
		f.close();
	}
	Serial.println();
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
	server.send(200, "test/plain", "test content");
}

void handleConnect() {	
	if (WiFi.localIP().toString() == "0.0.0.0") {
		if (server.hasArg("ssid") && server.hasArg("pass") && server.arg("ssid") != "") {
			ssid = server.arg("ssid");
			pass = server.arg("pass");
			Serial.printf("WiFi ssid:%s, pass:%s; attempting to connect...", ssid.c_str(), pass.c_str());
			if (connectWiFi()) server.send(200, "text/plain", "Connected");
			else server.send(200, "text/plain", "Failed");
			return;
		} else {
			Serial.println("Invalid arguments!\n");
			server.send(200, "text/plain", "Invalid");
		}
	} else {
		Serial.printf("Already connected\nSSID:%s\npassword:%s\nIP:%s\n\n",
		ssid.c_str(), pass.c_str(), WiFi.localIP().toString().c_str());
	}

	//server.sendHeader("Location", "/", true);
	server.send(200, "text/plain", "Already connected");
}

void handleDisconnect() {
	server.sendHeader("Location", "/", true);
	server.send(307, "text/plain", "");
	WiFi.disconnect();
	Serial.printf("Disconnected from %s\n\n", ssid.c_str());
}

void handleGet() {
	StaticJsonBuffer<400> buffer;
	JsonObject &json = buffer.createObject();

	if (server.arg("request") == "wifiscan") {
		int8_t networks = WiFi.scanNetworks();

		json["networks"] = networks;
		WiFi.localIP().toString() != "0.0.0.0" ? json["connected"] = ssid : json["connected"] = (char*)NULL;
		JsonArray& j_ssid = json.createNestedArray("ssid");
		JsonArray& j_channel = json.createNestedArray("channel");
		JsonArray& j_rssi = json.createNestedArray("rssi");
		JsonArray& j_enc = json.createNestedArray("enc");
		for (int8_t i = 0; i < networks; i++) {
			j_ssid.add(WiFi.SSID(i));
			j_channel.add(WiFi.channel(i));
			j_rssi.add(WiFi.RSSI(i));
			WiFi.encryptionType(i) == ENC_TYPE_NONE ? j_enc.add("open") : j_enc.add("");
		}
	}
	if (server.arg("request") == "wifistatus") {
		String ip = WiFi.localIP().toString();
		ip == "0.0.0.0" ? json["status"] = false : json["status"] = true;
		json["ssid"] = ssid;
		json["ip"] = ip;
	}

	String stringBuffer;
	json.printTo(stringBuffer);
	server.send(200, "application/json; charset=utf-8", stringBuffer);
	Serial.printf("JSON: %s\n\n", stringBuffer.c_str());
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

bool connectWiFi() {	
	WiFi.begin(ssid.c_str(), pass.c_str());
	if (WiFi.waitForConnectResult() == WL_CONNECTED) {
		Serial.printf("Connected to %s, IP: ", ssid.c_str());
		Serial.println(WiFi.localIP());
		Serial.println();
	return true;
	} else {
		Serial.println("Connection failed\n");
		return false;
	}
}

void setup() {
	Serial.begin(115200);
	Serial.println("");
		
	pinMode(LED, OUTPUT);
	digitalWrite(LED, 1);
	if(!SPIFFS.begin()) Serial.println("Error: SPIFFS not started!");;
	delay(500);

	WiFi.mode(WIFI_AP_STA);
	WiFi.softAP(apssid, appass);

	Serial.println("AP online");
	delay(500);
	Serial.print("IP: ");
	Serial.println(WiFi.softAPIP());
	Serial.println("Configuring server");
	delay(500);
		
	server.on("/", handleRoot);
	server.on("/test", testSubmit);
	server.on("/submit", HTTP_POST, handleSubmit);
	server.on("/menu", handleMenu);
	server.on("/offline", handleOffline);
	server.on("/connect", HTTP_POST, handleConnect);
	server.on("/disconnect", handleDisconnect);
	server.on("/get", HTTP_POST, handleGet);
	server.onNotFound(handleNotFound);

	server.begin();
	Serial.println("Server started!\nAttempting to connect to default AP...\n");
	connectWiFi();
}

void loop() {
	server.handleClient();
}
