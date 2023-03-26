# mains_freq
Measure frequency of the 240V mains

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
