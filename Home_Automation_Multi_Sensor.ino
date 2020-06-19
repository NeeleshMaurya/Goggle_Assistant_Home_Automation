//Google Assistant Home Automation
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"
#include <WiFiManager.h>
#include <DNSServer.h>

/************************* Pin Definition *********************************/

//Relays for switching appliances
#define Relay1            D1    //GPIO 5
#define Relay2            D2    //GPIO 4
#define Relay3            D3    //GPIO 0
#define Relay4            D4    //GPIO 2
#define Relay5            D8    //GPIO 15
#define Relay6            D9    //GPIO19/ Rx pin 

//DHT11 for reading temperature and humidity value
#define DHTPIN            D5    //GPIO 14

//buzzer to know the status of MQTT connections and can be used for any other purpose according to your project need.
#define buzzer            D0    //GPIO 16

//Selection pins for multiplexer module to switch between different sensors and give data on a single analog pin.
#define S0                D7    //GPIO 12
#define S1                D6    //GPIO 13

//Analog pin to read the incoming analog value from different sensors.
#define analogpin         A0    //Analog pin

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883     // use 8883 for SSL
#define AIO_USERNAME    "My_Project"    // Username
#define AIO_KEY         "fdb0ab64090f4fe6a0dcf77b10e47979"    // Auth Key

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiClientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish Humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish Temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish CO2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/co2");
Adafruit_MQTT_Publish Motion = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/motion");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe lights = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay1");
Adafruit_MQTT_Subscribe kitchenlights = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay2");
Adafruit_MQTT_Subscribe fan = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay3");
Adafruit_MQTT_Subscribe bulb = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay4");
Adafruit_MQTT_Subscribe lamp = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay5");
Adafruit_MQTT_Subscribe projector = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay6");

/************ Necessary declaration for DHT11 ******************/
#define DHTTYPE           DHT11     // DHT 11 

DHT dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

/*************************** Sketch Code ************************************/

void MQTT_connect();
void setup() {
  Serial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.autoConnect("Home_Automation");
  Serial.println("connection...yeey :)");

  delay(10);

  pinMode(buzzer, OUTPUT);
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
  pinMode(Relay5, OUTPUT);
  pinMode(Relay6, OUTPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(A0, INPUT);

  Serial.println(F("Adafruit MQTT demo"));
  //Setting up DHT sensor
  dht.begin();

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&lights);
  mqtt.subscribe(&kitchenlights);
  mqtt.subscribe(&fan);
  mqtt.subscribe(&bulb);
  mqtt.subscribe(&lamp);
  mqtt.subscribe(&projector);
}

uint32_t x = 0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  
  MQTT_connect();
  
  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20000))) {
    if (subscription == &lights) {
      Serial.print(F("Got: "));
      Serial.println((char *)lights.lastread);
      int lights_State = atoi((char *)lights.lastread);
      digitalWrite(Relay1, lights_State);
      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(200);
    }
    if (subscription == &kitchenlights) {
      Serial.print(F("Got: "));
      Serial.println((char *)kitchenlights.lastread);
      int kitchenlights_State = atoi((char *)kitchenlights.lastread);
      digitalWrite(Relay2, kitchenlights_State);
      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(200);
    }
    if (subscription == &fan) {
      Serial.print(F("Got: "));
      Serial.println((char *)fan.lastread);
      int fan_State = atoi((char *)fan.lastread);
      digitalWrite(Relay3, fan_State);
      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(200);
    }
    if (subscription == &bulb) {
      Serial.print(F("Got: "));
      Serial.println((char *)bulb.lastread);
      int bulb_State = atoi((char *)bulb.lastread);
      digitalWrite(Relay4, bulb_State);
      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(200);
    }
        if (subscription == &lamp) {
      Serial.print(F("Got: "));
      Serial.println((char *)lights.lastread);
      int lamp_State = atoi((char *)lamp.lastread);
      digitalWrite(Relay5, lamp_State); 
      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(200);
    }
        if (subscription == &projector) {
      Serial.print(F("Got: "));
      Serial.println((char *)projector.lastread);
      int projector_State = atoi((char *)projector.lastread);
      digitalWrite(Relay6, projector_State); 
      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(200);
    }
  }

  // Now we can publish stuff!
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  Serial.print("Motion "); 
  Serial.println(analogRead(analogpin));
  Serial.print("...");
  int Value = analogRead(analogpin);
  if(Value>400)
  Value=1;
  else
  Value=0;
  if (! Motion.publish(Value)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);
    delay(200);
  }

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  Serial.print("C02 "); 
  Serial.println(analogRead(analogpin));
  Serial.print("...");
  Value = analogRead(analogpin);
  if (! CO2.publish(Value)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  
/*
  digitalWrite(S0, LOW);
  digitalWrite(S1, HIGH);
  Serial.print("Sound "); Serial.println(analogRead(analogpin));
  Serial.print("...");
  int raw_sound = analogRead(analogpin);
  Value = map(raw_sound,0,1024,0,100);
  if (! Sound.publish(Value)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  */
/*
  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH);
  Serial.print("Light "); Serial.println(analogRead(analogpin));
  Serial.print("...");
  int raw_light = analogRead(analogpin);
  Value = map(raw_light,1024,0,0,100);
  if (! Light.publish(Value)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
*/

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  if (! Humidity.publish(h)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  if (! Temperature.publish(t)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  
  /*                  
    if(! mqtt.ping()) {       // ping the server to keep the mqtt connection alive 
    mqtt.disconnect();        // NOT required if you are publishing once every KEEPALIVE seconds
    }
  */
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(200);
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(200);
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
  digitalWrite(buzzer, HIGH);
  delay(2000);
  digitalWrite(buzzer, LOW);
}
