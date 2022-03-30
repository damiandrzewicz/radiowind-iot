#include <Arduino.h>
#include <ArduinoLog.h>
#include <Wire.h>
#include "RadioWindNode.hpp"
#include "SparkFunBME280.h"

class AirSensor : public RadioWindNode
{
public:
  BME280 bme280;
};

AirSensor airSensor;

unsigned int c = 0;

void setup() {
  // Initialize UART interface
  Serial.begin(115200);

  // put your setup code here, to run once:
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  airSensor.setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  airSensor.loop();
}