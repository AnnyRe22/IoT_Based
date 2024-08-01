#define BLYNK_TEMPLATE_ID "TMPL6j6HKHNDE"
#define BLYNK_TEMPLATE_NAME "AQUABONIC"
#define BLYNK_AUTH_TOKEN "gZ2aCTvgdj3DreyHajX_h-VJSmPEPtl1"


#include <Wire.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <WifiClient.h>


// WiFi credentials
const char* ssid = "Mizo";
const char* password = "rvdd7424";


// Pin definitions
#define BUZZER_PIN 25
#define RED_LED_PIN 26
#define GREEN_LED_PIN 27
#define CURRENT_SENSOR_PIN 34


// Threshold values
#define DO_THRESHOLD 5000
#define CURRENT_THRESHOLD 7
// I2C address for your DO sensor
#define DO_SENSOR_ADDRESS 0x61


BlynkTimer timer;


// Function to read dissolved oxygen sensor
float readDO() {
  Wire.beginTransmission(DO_SENSOR_ADDRESS);
  Wire.write(0x61);
  Wire.endTransmission();


  Wire.requestFrom(DO_SENSOR_ADDRESS, 2);
  if (Wire.available() == 2) {
    uint16_t rawData = Wire.read() << 8 | Wire.read();
    return rawData * 0.1; // Conversion factor, replace with your sensor's specific factor
  }
  return -1;
}


// Function to read current sensor
float readCurrent() {
  int currentValue = analogRead(CURRENT_SENSOR_PIN);
  float currentVoltage = currentValue * (5.0 / 4095.0);
  float currentAmps = currentVoltage / 0.3; // Replace with your sensor's sensitivity
  return currentAmps;
}


// Function to check sensors and update Blynk
void checkSensors() {
  float doValue = readDO();
  float currentAmps = readCurrent();
 
  Serial.print("DO Sensor Value: ");
  Serial.println(doValue);
  Serial.print("Current Sensor Value: ");
  Serial.println(currentAmps);


  Blynk.virtualWrite(V2, doValue); // Update DO value on Blynk LCD
  Blynk.virtualWrite(V3, currentAmps); // Update Current value on Blynk LCD


  bool failure = false;
  String errorMsg = "";


  if (doValue > DO_THRESHOLD) {
    failure = true;
    errorMsg += "Dissolved Oxygen sensor malfunction! ";
  }


  if (currentAmps > CURRENT_THRESHOLD) {
    failure = true;
    errorMsg += "Current sensor malfunction! ";
  }


   if (failure) {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    tone(BUZZER_PIN, 1000);
    Blynk.logEvent("FALIURE", errorMsg);
   


    // Turn on Blynk red virtual LED and off the green virtual LED
    Blynk.virtualWrite(V1, 0); // Green LED off
    Blynk.virtualWrite(V0, 255); // Red LED on
  } else {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
    noTone(BUZZER_PIN);


    // Turn on Blynk green virtual LED and off the red virtual LED
    Blynk.virtualWrite(V1, 255); // Green LED on
    Blynk.virtualWrite(V0, 0); // Red LED off
  }
}


void setup() {
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }


 
  Serial.println("Connected to the WiFi network");
 
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);


  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);


  // Turn on the green LED initially
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);
  noTone(BUZZER_PIN);


  // Initialize I2C
  Wire.begin();


  // Setup a timer to check sensors every second
  timer.setInterval(10000, checkSensors);
}


void loop() {
  Blynk.run();
  timer.run();


 
}