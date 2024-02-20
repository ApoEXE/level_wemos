#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "TP-JAVI";
const char* password = "xavier1234";
const char* mqtt_server = "192.168.0.250";
const char* TOPIC = "Tanque1/canal/level/sensor1";
WiFiClient espClient;
PubSubClient client(espClient);
//messages
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

//app
#define echoPin D2  // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin D3  //attach pin D3 Arduino to pin Trig of HC-SR04

// defines variables
long duration;  // variable for the duration of sound wave travel
float distance;   // variable for the distance measurement


//app functions
void reconnect();
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);  // Initialize the BUILTIN_LED pin as an output
  pinMode(trigPin, OUTPUT);      // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT);       // Sets the echoPin as an INPUT
  Serial.begin(115200);
  //Serial display
  Serial.println("Level sensor");

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  delay(50);
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  snprintf(msg, MSG_BUFFER_SIZE, "%.2f", distance);
  client.publish(TOPIC, msg);

}


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {

  if (strcmp(TOPIC, topic) == 0) {
    digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(TOPIC, "Water Level Sensor");
      // ... and resubscribe
      client.subscribe(TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
