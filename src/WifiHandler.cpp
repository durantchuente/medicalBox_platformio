#include "WifiHandler.h"

WifiHandler::WifiHandler(const char* ssid, const char* password, const char* serverLogin)
  : ssid(ssid), password(password), serverLogin(serverLogin), bearerToken(""), refreshToken("votre_refresh_token") {}

void WifiHandler::setupWifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au WiFi...");
  }
  Serial.println("Connecté au WiFi");
}

void WifiHandler::getAccessToken() {
  HTTPClient http;
  http.begin(serverLogin);
  http.addHeader("Content-Type", "application/json");

  String postData = "{\"username\":\"duran\",\"password\":\"duran\"}";

  int httpResponseCode = http.POST(postData);
  if (httpResponseCode == 200) {
    String response = http.getString();
    Serial.println("Réponse du serveur pour le token : ");
    Serial.println(response);

    int accessStartIndex = response.indexOf("\"access\":");
    if (accessStartIndex != -1) {
      int tokenStartIndex = response.indexOf("\"", accessStartIndex + 9) + 1;
      int tokenEndIndex = response.indexOf("\"", tokenStartIndex);
      if (tokenStartIndex > 0 && tokenEndIndex > 0) {
        bearerToken = response.substring(tokenStartIndex, tokenEndIndex);
        Serial.println("Token d'accès récupéré : ");
        Serial.println(bearerToken);
      }
    }
  } else {
    Serial.print("Erreur lors de la récupération du token: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void WifiHandler::refreshAccessToken() {
  HTTPClient http;
  http.begin(serverLogin);
  http.addHeader("Content-Type", "application/json");

  String postData = "{\"refresh\":\"" + refreshToken + "\"}";

  int httpResponseCode = http.POST(postData);
  if (httpResponseCode == 200) {
    String response = http.getString();
    Serial.println("Réponse du serveur pour le rafraîchissement du token : ");
    Serial.println(response);

    int accessStartIndex = response.indexOf("\"access\":");
    if (accessStartIndex != -1) {
      int tokenStartIndex = response.indexOf("\"", accessStartIndex + 9) + 1;
      int tokenEndIndex = response.indexOf("\"", tokenStartIndex);
      if (tokenStartIndex > 0 && tokenEndIndex > 0) {
        bearerToken = response.substring(tokenStartIndex, tokenEndIndex);
        Serial.println("Nouveau token d'accès récupéré : ");
        Serial.println(bearerToken);
      }
    }
  }
  http.end();
}

void WifiHandler::sendData(String json, String serverName) {
  HTTPClient http;
  if (bearerToken == "") {
    Serial.println("Token expiré ou inexistant. Récupération du token...");
    refreshAccessToken();
    if (bearerToken == "") {
      Serial.println("Impossible de rafraîchir le token. Abandon de l'envoi.");
      return;
    }
  }

  http.begin(serverName);
  http.addHeader("Authorization", "Bearer " + bearerToken);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(json);
  if (httpResponseCode == 201) {
    String response = http.getString();
    Serial.println("Réponse du serveur : " + response);
  } else {
    Serial.print("Erreur lors de l'envoi : ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void WifiHandler::updateDeviceAddress() {
  deviceAddress = WiFi.macAddress();
}
