#include "Controller.h"
#include <math.h>
#include <Wire.h>

/**
 * Constructor de Switch
 * @param pin: Pin al que se hace referencia
 * @param parametro: El parametro al que queremos enviar el valor
 * @param valorPosicion1: El valor cuando se activa
 * @param valorPosicion2: El valor cuando se desactiva
*/

Switch::Switch(int pin, const char* parametro, const char* valorPosicion1, const char* valorPosicion2) {
    _pin = pin;
    _parametro = parametro;
    _valorPosicion1 = valorPosicion1;
    _valorPosicion2 = valorPosicion2;
    _estadoAnterior = false;
    _eventoEnviado = false;
}

/**
 * Inicializacion de la clase (Poner en el setup())
*/

void Switch::begin() {
    pinMode(_pin, INPUT_PULLUP);
}

/**
 * Actualiza estado y envia por serial (Poner en el loop()) 
*/

void Switch::update() {
    bool estado = digitalRead(_pin);

    if (estado != _estadoAnterior) {
        delay(50);  // Debounce: esperar un corto tiempo para evitar lecturas falsas debido a rebotes

        if (estado == LOW && !_eventoEnviado) {
            // Interruptor en posición 1, enviar comando serial con valor de 1
            Serial.print(_parametro);
            Serial.print("=");
            Serial.println(_valorPosicion1);
            _eventoEnviado = true;
        }
        else if (estado == LOW && _eventoEnviado) {
            // Interruptor en posición 2, enviar comando serial con valor de 0
            Serial.print(_parametro);
            Serial.print("=");
            Serial.println(_valorPosicion2);
            _eventoEnviado = false;
        }
    }

    _estadoAnterior = estado;
}

PushButton::PushButton(int pin, const char* parametro, const char* valorPresionado, const char* valorSuelto) {
    _pin = pin;
    _parametro = parametro;
    _valorPresionado = valorPresionado;
    _valorSuelto = valorSuelto;
    _estadoAnterior = false;
    _eventoEnviado = false;
}

void PushButton::begin() {
    pinMode(_pin, INPUT_PULLUP);
}

void PushButton::update() {
    bool estado = digitalRead(_pin);

    if (estado != _estadoAnterior) {
        delay(50);  // Debounce: esperar un corto tiempo para evitar lecturas falsas debido a rebotes

        if (estado == LOW && !_eventoEnviado) {
            // Pulsador presionado, enviar comando serial continuamente con valor de 1
            Serial.print(_parametro);
            Serial.print("=");
            Serial.println(_valorPresionado);
            _eventoEnviado = true;
        }
        else if (estado == HIGH && _eventoEnviado) {
            // Pulsador suelto, enviar comando serial una vez con valor de 0
            Serial.print(_parametro);
            Serial.print("=");
            Serial.println(_valorSuelto);
            _eventoEnviado = false;
        }
    }

    _estadoAnterior = estado;
}

/*PushButton_PCF8574::PushButton_PCF8574(int pin, const char* parametro, const char* valorPresionado, const char* valorSuelto, byte addr) : PCF8574(addr, Wire) {
    _pin = pin;
    _parametro = parametro;
    _valorPresionado = valorPresionado;
    _valorSuelto = valorSuelto;
    _estadoAnterior = false;
    _eventoEnviado = false;
}

void PushButton_PCF8574::begin() {
    PCF8574::pinMode(_pin, INPUT_PULLUP);
}

void PushButton_PCF8574::update() {
  bool estado = PCF8574::digitalRead(_pin);
  if (estado != _estadoAnterior) {
    delay(50);  // Debounce: esperar un corto tiempo para evitar lecturas falsas debido a rebotes

    if (estado == LOW && !_eventoEnviado) {
      // Pulsador presionado, enviar comando serial continuamente con valor de 1
      Serial.print(_parametro);
      Serial.print("=");
      Serial.println(_valorPresionado);
      _eventoEnviado = true;
    }
    else if (estado == HIGH && _eventoEnviado) {
      // Pulsador suelto, enviar comando serial una vez con valor de 0
      Serial.print(_parametro);
      Serial.print("=");
      Serial.println(_valorSuelto);
      _eventoEnviado = false;
    }
  }
  _estadoAnterior = estado;
}
*/
PotFilter::PotFilter(int pin, int minPot, int maxPot, int neutro, float minValue, float maxValue, int filterSize) {
  _pin = pin;
  _minValue = minValue;
  _maxValue = maxValue;
  _filterSize = filterSize;
  _filterBuffer = new int[filterSize];
  for (int i = 0; i < filterSize; i++) {
    _filterBuffer[i] = 0;
  }
  _filterIndex = 0;
  _filterSum = 0;
  _value = 0.0;
}

void PotFilter::update() {
  // Leer el valor del potenciómetro
  int rawValue = analogRead(_pin);

  // Filtrar el valor del potenciómetro
  _filterSum -= _filterBuffer[_filterIndex];
  _filterBuffer[_filterIndex] = rawValue;
  _filterSum += rawValue;
  _filterIndex = (_filterIndex + 1) % _filterSize;
  int filteredValue = _filterSum / _filterSize;

    // Mapear el valor filtrado al rango deseado
  float mappedValue = map(filteredValue, 0, 1023, _minValue * 1000, _maxValue * 1000) / 1000.0;

    // Almacenar el valor mapeado
  _value = mappedValue;
}

float PotFilter::getValue() {
  return _value;
}

SerialInterface::SerialInterface(Stream &stream, void (*lineHandler)(const char *, const char *)) : lineHandler(lineHandler), serial(stream) {
  newData = false;
}

void SerialInterface::begin() { 
  Serial.begin(115200);
}

void SerialInterface::update()
{
  if (reindex >= windex)
  {
    windex = reindex = 0;
  }
  if (reindex > 60)
  {
    memmove(data, data + reindex, windex - reindex);
    windex = windex - reindex;
    reindex = 0;
  }
  if (serial.available() && windex < 128)
  {
    windex += serial.readBytes(data + windex, min(serial.available(), 128 - windex));
    newData = true;
  }
  if (newData)
  {
    newData = false;
    for (int i = reindex; i < windex; i++)
    {
      if (data[i] == '\r' || data[i] == '\n')
      {
        if (i == reindex)
        {
          reindex = i + 1;
          continue;
        }
        char *line = reinterpret_cast<char *>(data + reindex);
        line[i - reindex] = 0;
        char *value = strchr(line, '=') + 1;
        value[-1] = 0;
        lineHandler(line, value);
        reindex = i + 1;
      }
      else if (i + 1 >= 128)
      {
        windex = reindex = 0;
      }
    }
  }
}

OutputManager::OutputManager(const char* parameter, int pin) {
  _command = parameter;
  _pin = pin;
}
void OutputManager::begin() {
  Serial.print("register(");
  Serial.print(_command);
  Serial.println(")");
  pinMode(_pin, OUTPUT);
}

void OutputManager::outputDigital(const char* command, const char* value, int targetValue) {
  if (strncmp(command, _command, strlen(_command)) == 0) {
    int state = (atoi(value) == targetValue);
    digitalWrite(_pin, state);
  }
}

void OutputManager::outputAnalog(const char* command, const char* value, int targetValue) {
  if (strncmp(command, _command, strlen(_command)) == 0) {
    int inputValue = atoi(value);
    int intensity = map(inputValue, 0, 1023, 0, 255); // Mapear de 0-1023 a 0-255
    analogWrite(_pin, intensity);
  }
}