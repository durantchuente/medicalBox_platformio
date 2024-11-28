#include "HX711Handler.h"

HX711Handler::HX711Handler(int doutPin, int sckPin, int boxPin, int echoPin)
  : doutPin(doutPin), sckPin(sckPin), boxPin(boxPin), echoPin(echoPin), weightLast(0), lastBoxState(LOW), currentBoxState(LOW) {}

void HX711Handler::setup() {
  scale.begin(doutPin, sckPin);
  scale.tare();  // Réinitialiser la balance au démarrage
  scale.set_scale(396);
  pinMode(boxPin, INPUT_PULLUP);  // Pin pour la boîte
}

void HX711Handler::readWeight() {
  if (scale.is_ready()) {
    long sum = 0;
    int readings = 10;  // Nombre de lectures pour moyenne
    for (int i = 0; i < readings; i++) {
      sum += scale.get_units(1);  // Lire une fois et additionner
    }
    weight = sum / readings;  // Moyenne des lectures pour stabiliser
    Serial.print("Poids moyen : ");
    Serial.println(weight);

    float weightDifference = weight - weightLast;

    if (abs(weightDifference) >= stabilityThreshold) {
      if (weightDifference > 0) {
        tendance = "H";
        Serial.println("Augmentation de poids détectée !");
      } else {
        tendance = "L";
        Serial.println("Réduction de poids détectée !");
      }
      weightLast = weight;
    } else {
      Serial.print("Poids stable : ");
      Serial.print(weight);
      Serial.println(" g");
    }
  } else {
    Serial.println("HX711 non prêt.");
  }
}

bool HX711Handler::isWeightStable() {
  return abs(weight - weightLast) < stabilityThreshold;
}

String HX711Handler::getWeightTendency() {
  return tendance;
}

String HX711Handler::prepareDataForJson(String deviceAddress, String tendance) {
  return "{\"address\":\"" + deviceAddress + "\",\"tendance\":\"" + tendance + "\",\"weight\":" + String(weight) + "}";
}

bool HX711Handler::isBoxStateChanged() {
    currentBoxState = digitalRead(boxPin);
    if (currentBoxState != lastBoxState) {
        lastBoxState = currentBoxState;
        return true;  // L'état de la boîte a changé
    }
    return false;  // Aucun changement
}

bool HX711Handler::getBoxState() {
  return currentBoxState;
}

bool HX711Handler::getLastBoxState() {
  return lastBoxState;
}
