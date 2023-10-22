#include <Arduino.h>
#include <Controller.h>

PushButton pul1 = PushButton(2, "asfa::pulsador::rec", "1", "0");
OutputManager led1 = OutputManager("asfa::ilum::rec", 3);

void handleLine(const char* line, const char* value) {
  led1.outputDigital(line, value, 1);
}

SerialInterface s = SerialInterface(Serial, handleLine);

void setup() {
  pul1.begin();
  s.begin();
  led1.begin();
}

void loop() {
  pul1.update();
  s.update();
}
