/////////////////////////////////////////////////////////////////////////////////////
// code to determine the mains frequency (50Hz) with a resolution of approx 1mHz   //
//                                                                                 //
// (c) 2023 Ste7an                                                                 //
/////////////////////////////////////////////////////////////////////////////////////
//
// depends on the following libraries
// RPi_Pico_TimerInterrupt: https://github.com/khoih-prog/RPI_PICO_TimerInterrupt
// Commander: https://github.com/CreativeRobotics/Commander/wiki

// principle:
// Measure time to complete 50 full periods of the mains sine. Use the 0 crossing of (A0-a_off). 
// The frequency is determined by: freq = 1 / (time(50 periods)) / 50)
//
// Serial2 Uart is used to communicate with an ESP8266 module running tasmota with a serial bridge to MQTT
// On the Serial2 port a Commander is run with the following commands:
// get <var_name>			gets value of <var_name>
// set <var_name> <value>     sets <var_name> to value <value>
// readflash			reads the stored variables from EEPROM/flash
// writeflash			writes the variables to EEPROM/flash
//
// the following variables can be set/get:
//
// us			us_factor is the callibrated time of 1s in microseconds, (us = 1000000)
// a_off		the offset that is subtracted from the A0 signal (the 0 crossing of the sine wave) (a_off = 500)
// a_min		a negative offset to determine the raising edge (a_min = -50)
// a_max		a positive offset to determine the raising edge (a_max = 50)
// alpha		the smoothing factor in freq_smoothed = freq_smoothed * (1-alpha) + freq * alpha (alpha = 0.40)
// discard		The factor used to discard outliers (discard = 1.0005)
// readflash	indicates whether variables stored in EEPROM/flash are read at boot (readflash==1)
// flash_count	counter indicating the number of times the flash was written
//
////////////////////////////////////////////////////////////////////////////////////



#include <Commander.h>
#include <EEPROM.h>
#include "RPi_Pico_TimerInterrupt.h"

Commander cmd;

RPI_PICO_Timer ITimer0(0);
bool time_out = false;
int time_out_ms = 1200; // 1.2s timeout
bool TimerHandler0(struct repeating_timer *t)
{
  (void) t;
  static bool toggle0 = false;
  time_out = true; // set global time_out to true
  return true;
}

float us_factor = 1000000.;
int a_off = 500;
int a_min = -50;
int a_max = 50;
float  alpha = 0.4;
float discard = 1.0005;
int readflash = 0;
int flash_count = 0;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

#define PIN_MAINS   D5

unsigned long startMicros;
unsigned long currentMicros;
int hz50 = 50;
int frequencyAnalogPin = A0;
int periodcount = 0;
float frequency = 0 ;
long delta = 0;
int switch01 = 0;
int vAnalogRead = 0;

// templates to write and read variables to and from EEPROM
//
template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
  const byte* p = (const byte*)(const void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++)
    EEPROM.write(ee++, *p++);
  return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
  byte* p = (byte*)(void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++)
    *p++ = EEPROM.read(ee++);
  return i;
}


bool write_to_flash() {
  int n;
  n = EEPROM_writeAnything(0, readflash);
  flash_count++;
  n = EEPROM_writeAnything(4, flash_count);
  n = EEPROM_writeAnything(8, a_off);
  n = EEPROM_writeAnything(12, a_min);
  n = EEPROM_writeAnything(16, a_max);
  n = EEPROM_writeAnything(20, alpha);
  n = EEPROM_writeAnything(24, us_factor);
  n = EEPROM_writeAnything(28, discard);
  EEPROM.commit();
  return true;
}

bool read_from_flash() {
  int n;
  n = EEPROM_readAnything(0, readflash);
  n = EEPROM_readAnything(4, flash_count);
  // read other variables only when readflash equal 1
  if ((readflash == 1) ) { 
    n = EEPROM_readAnything(8, a_off);
    n = EEPROM_readAnything(12, a_min);
    n = EEPROM_readAnything(16, a_max);
    n = EEPROM_readAnything(20, alpha);
    n = EEPROM_readAnything(24, us_factor);
    n = EEPROM_readAnything(28, discard);
  }
  return true;
}

void setup() {
  EEPROM.begin(512); // enable eeprom
  read_from_flash();
  pinMode(PIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  Serial2.setRX(9);
  Serial2.setTX(8);
  Serial2.begin(9600);
  initialiseCommander();

}

float measure() {
  switch01 = 0;
  periodcount = 0;
  time_out = false;
  // start timer
  ITimer0.attachInterruptInterval(time_out_ms * 1000, TimerHandler0);

  while ((periodcount < hz50 + 1 ) && !time_out) {

    vAnalogRead = analogRead(frequencyAnalogPin) - a_off;

    if (vAnalogRead > a_max && switch01 == 0) 
    {
      periodcount += 1;
      switch01 = 1;
    }

    if (vAnalogRead < a_min && switch01 == 1) {
      switch01 = 0;
    }
  }

  if (periodcount == hz50 + 1) {
    delta = micros() - startMicros ;
    Serial.println(delta);
    frequency = 1. / ((float(delta) / us_factor) / float(hz50 )); //kleiner getal is lagere freq
    Serial.println(frequency, 3);
    startMicros = micros();
    return frequency;

  }
  //startMicros = micros();
  return -1; // timeout -> freq is negative
}

float freq = 0;
float value = hz50;
void loop() {

  cmd.update();

  freq = measure();
  if (freq > 0) {
    value = alpha * freq + (1 - alpha) * value;

    snprintf(msg, MSG_BUFFER_SIZE, "%9.3f", value);
    Serial.println(msg);
    if ((freq < value * discard) && (freq >  value / discard )) { // only send data when data is within 'dicard' range.
      Serial2.println(msg);
    } else {
      Serial.print("discarded freq:");
      Serial.println(freq);
    }
  } else
    Serial.println("Timeout");

}
