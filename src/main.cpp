#include <WiFi.h>
#include <HTTPClient.h>
#include "HX711Handler.h"
#include "WifiHandler.h"

const char* ssid = "UNIFI_IDO2";
const char* password = "99Bidules!";
const char* serverLogin = "http://192.168.20.113:8000/api/token";
const char* serverName = "http://192.168.20.113:8000/api/data";

WifiHandler wifiHandler(ssid, password, serverLogin);
HX711Handler scaleHandler(4, 0, 5, 18);  // Pins pour HX711 et capteur de la boîte

void setup() {
  Serial.begin(115200);
  wifiHandler.setupWifi();
  scaleHandler.setup();
  wifiHandler.getAccessToken();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    wifiHandler.updateDeviceAddress();
  } else {
    Serial.println("Erreur de connexion au WiFi");
  }

  scaleHandler.readWeight();

  if (scaleHandler.isWeightStable()) {
    String tendance = scaleHandler.getWeightTendency();
    // Vérifier la condition d'envoi de données (état de la boîte + tendance)
    if (scaleHandler.isBoxStateChanged() && tendance != "" && !scaleHandler.getBoxState()) {
      Serial.println("Send data!");
      String json = scaleHandler.prepareDataForJson(wifiHandler.getDeviceAddress(), tendance);
      wifiHandler.sendData(json, serverName);
    }
  }

  // Vérifier l'état de la boîte et afficher si nécessaire
  if (scaleHandler.isBoxStateChanged()) {
    String boxState = scaleHandler.getBoxState() ? "ouverte" : "fermée";
    Serial.println("Boîte " + boxState);
  }

  delay(1000);
}
