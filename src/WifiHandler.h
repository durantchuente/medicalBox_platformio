#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <WiFi.h>
#include <HTTPClient.h>

class WifiHandler {
public:
  WifiHandler(const char* ssid, const char* password, const char* serverLogin);
  void setupWifi();
  void getAccessToken();
  void refreshAccessToken();
  void sendData(String json, String serverName);
  String getDeviceAddress() { return deviceAddress; }
  void updateDeviceAddress();

private:
  const char* ssid;
  const char* password;
  const char* serverLogin;
  String bearerToken;
  String refreshToken;
  String deviceAddress;
};

#endif
