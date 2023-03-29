# mains_freq
Measure frequency of the 240V mains

## Electronics

### Main design
The frequency meter consists of two microcontroller boards. A Raspberry Pico Pi and an ESP8266 board. The pico pi is responsible for doing the actual frequency measurement. The ESP8266 is programmed with a TLS version of the TASMOTA firmware. The anaolgue output of the ZMPT101B module is connected to the ADC0 convertor of te Pico Pi. The Pico Pi determines the rasing zero crossings of the sine wave and counts 50 zero crossings. The time is measured using the internal micro seconds clock. From this measurement the mains frequency is derived. To smooth the output of the frequency measurements, a running average is taken using $freq = &alpha; freq_m + (1-&alpha; ) freq$ 

output of the analo

### ZMPT101B
This is a single phase transformer with an additional OPAMP to convert the transformed voltage between 0 and VCC, where VCC is an externally provided voltage. In out case, we use VCC=3.3V. The potentiometer is adjusted sunch that the output sine waved is well formed and not clipped. Using a small test programm running on

## Firmware
### TASMOTA on ESP8266
The firmware can be installed directly using [this web installer](https://tasmota.github.io/install/). Choose as platform

## Mosquitto server using docker-compose

Set up a mosquitto server. this is descibed [here](https://medium.com/himinds/mqtt-broker-with-secure-tls-and-docker-compose-708a6f483c92). Generate certs using the script `generate_certs.sh`. Create a direcotry `./config` in the docker directory with a subdirectory `./config/certs`. Put the generated certs and keyfiles in the `./config/certs' folder.

This is the docker compose-compose.yml file:
```
services:
  mosquitto:
    container_name: mosquitto_tls
    image: eclipse-mosquitto:2
    volumes:
      - ./config/:/mosquitto/config/
    ports:
      - 9883:8883

volumes:
  data_mqtt: ~
```

This is the `mosquitto.conf` file
```
port 8883

cafile /mosquitto/config/certs/ca.crt
certfile /mosquitto/config/certs/server.crt
keyfile /mosquitto/config/certs/server.key

allow_anonymous false
require_certificate false
#use_identity_as_username true
tls_version tlsv1.2
password_file /mosquitto/config/mosquitto.passwd
```
