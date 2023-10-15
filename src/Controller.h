#ifndef Controller_h
#define Controller_h

#include <Arduino.h>
#include <PCF8574.h>
#include <Wire.h>

class Switch{
public:
    Switch(int pin, const char* parametro, const char* valorPosicion1, const char* valorPosicion2);
    void begin();
    void update();

private:
    int _pin;
    const char* _parametro;
    const char* _valorPosicion1;
    const char* _valorPosicion2;
    bool _estadoAnterior;
    bool _eventoEnviado;
    byte _addr;
};

class PushButton {
public:
    PushButton(int pin, const char* parametro, const char* valorPresionado, const char* valorSuelto);
    void begin();
    void update();

private:
    int _pin;
    const char* _parametro;
    const char* _valorPresionado;
    const char* _valorSuelto;
    bool _estadoAnterior;
    bool _eventoEnviado;
};

class PushButton_PCF8574 : PCF8574{
public:
    PushButton_PCF8574(int pin, const char* parametro, const char* valorPresionado, const char* valorSuelto, byte addr);
    void begin();
    void update();

private:
    int _pin;
    const char* _parametro;
    const char* _valorPresionado;
    const char* _valorSuelto;
    bool _estadoAnterior;
    bool _eventoEnviado;
    byte _addr;
};

class PotFilter {
public:
    PotFilter(int pin, int minPot, int maxPot, int neutro, float minValue, float maxValue, int filterSize);
    void update();
    float getValue();

private:
    int _pin;
    float _minValue;
    float _maxValue;
    int _filterSize;
    int* _filterBuffer;
    int _filterIndex;
    int _filterSum;
    float _value;
    int _minPot;
    int _maxPot;
    int _neutro;
};

class SerialInterface {
public:
    SerialInterface(Stream &stream, void (*lineHandler)(const char *, const char *));
    void begin();
    void update();

private:
    byte data[512];
    int windex = 0;
    int reindex = 0;
    void (*lineHandler)(const char *, const char *);
    Stream &serial;
    bool newData;
};


class OutputManager {
public:
  OutputManager(const char* parameter, int pin);
  void begin();
  void outputDigital(const char* command, const char* value, int targetValue);
  void outputAnalog(const char* command, const char* value, int targetValue);

private:
  const char* _command;
  int _pin; 
};

#endif
