#ifndef HX711_HANDLER_H
#define HX711_HANDLER_H

#include "HX711.h"

class HX711Handler {
public:
  HX711Handler(int doutPin, int sckPin, int boxPin, int echoPin);
  void setup();
  void readWeight();
  bool isWeightStable();
  String getWeightTendency();
  String prepareDataForJson(String deviceAddress, String tendance);
  bool isBoxStateChanged();
  bool getBoxState();
  bool getLastBoxState();

private:
  int doutPin;
  int sckPin;
  int boxPin;
  int echoPin;
  bool lastBoxState;
  bool currentBoxState;
  HX711 scale;
  float weight;
  float weightLast;
  float calibrationFactor = 396;
  float stabilityThreshold = 2.0;
  String tendance;
};

#endif
