/*
 Arksaw Automation Firmware
 for the ESP8266

 This software connects relays to the ESP8266 WiFi microcontroller. We communicate
 with the automation API via the MQTT pub/sub protocol (http://mqtt.org/). We connect
 to the broker and wait for calls to come in to interact with the relays.

 Startup Procedure:
  - set 2 GPIO pins to outputs to communicate with the relays
  - connect to configured WiFi
  - connect to the MQTT broker server
  - boradcast that we are connected
*/

// Vendor Libs
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "config.h"

// TODO - get these the hell out of here
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
char* ip_address;
////

void setup() {
  // Set our pin modes
  pinMode(GPIO_PIN_1, OUTPUT);
  pinMode(GPIO_PIN_2, OUTPUT);

  // Spin up serial debugging
  Serial.begin(115200);

  // Connect to the configured wifi network
  setup_wifi();

  // Connect our mqtt broker
  client.setServer(mqtt_host, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);

  // Log what ssid we are connecting to
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  // Connect to wifi
  WiFi.begin(wifi_ssid, wifi_pass);

  // Wait till we are done connecting
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Log what our IP address is
  Serial.println("");
  Serial.println("WiFi Connected: ");
  Serial.println(WiFi.localIP());

  WiFi.localIP().toString().toCharArray(ip_address, 50);
}

void callback(char* topic, byte* payload, unsigned int length) {

  // Don't procede if the topic isn't ours
  //if (strcmp(topic, strcat()) != 0) {
  // return;
  //}

  Serial.println("");
  Serial.println("Retrieved command...");

  char cmd = (char)payload[0];

  switch(cmd) {
    case '1':
      Serial.println("GPIO1 LOW");
      digitalWrite(GPIO_PIN_1, LOW);
      break;
    case '2':
      Serial.println("GPIO1 HIGH");
      digitalWrite(GPIO_PIN_1, HIGH);
      break;
    case '3':
      Serial.println("GPIO2 LOW");
      digitalWrite(GPIO_PIN_2, LOW);
      break;
    case '4':
      Serial.println("GPIO2 HIGH");
      digitalWrite(GPIO_PIN_2, HIGH);
      break;
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    Serial.println(mqtt_user);
    Serial.println(mqtt_pass);
    if (client.connect("ESP8266Client", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.publish("connection", ip_address);
      client.subscribe("/switch/1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      delay(5000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }
}
