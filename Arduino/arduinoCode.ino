// Arduino Code
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// Initialize the LCD with the I2C address and LCD size
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define DHT sensor type and pin
#define DHTTYPE DHT22
#define DHTPIN 7

// Define Rain Sensor pins
#define RAIN_DIGITAL_PIN 8
#define RAIN_ANALOG_PIN A1

// Define LDR pin
#define LDR_PIN A2

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  lcd.init();                      // Initialize the LCD
  lcd.backlight();                 // Turn on the backlight
  dht.begin();                     // Initialize the DHT sensor
  pinMode(RAIN_DIGITAL_PIN, INPUT); // Set rain sensor digital pin as input
  Serial.begin(9600);              // Initialize serial communication for debugging
  Serial3.begin(115200);           // Initialize Serial3 for communication with ESP32
}

void loop() {
  delay(2000); // Wait a few seconds between measurements

  // Read temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    lcd.setCursor(0, 0);
    lcd.print("Sensor error");
    return;
  }

  // Read rain sensor values
  int rainDigital = digitalRead(RAIN_DIGITAL_PIN);
  int rainAnalog = analogRead(RAIN_ANALOG_PIN);

  // Read LDR value
  int ldrValue = analogRead(LDR_PIN);
  float V_out = ldrValue * (5.0 / 1023.0); // Convert analog value to voltage
  float R_fixed = 10000.0; // 10kΩ resistor
  float R_LDR = (5.0 - V_out) * R_fixed / V_out; // Calculate LDR resistance
  float illuminance = pow((R_LDR / 1000.0), -1.25); // Estimate illuminance (lux)

  // Clear the LCD before printing new values
  lcd.clear();

  // Display temperature and humidity
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print((char)223); // Degree symbol
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print("%");

  delay(2000); // Display temp and humidity for 2 seconds

  // Clear the LCD before displaying rain status
  lcd.clear();

  // Display rain status
  lcd.setCursor(0, 0);
  if (rainDigital == LOW) {
    lcd.print("Rain Detected");
  } else {
    lcd.print("No Rain");
  }

  lcd.setCursor(0, 1);
  lcd.print("Rain Level: ");
  lcd.print(rainAnalog);

  delay(2000); // Display rain status for 2 seconds

  // Clear the LCD before displaying light intensity
  lcd.clear();

  // Display light intensity
  lcd.setCursor(0, 0);
  lcd.print("Light Intensity:");
  lcd.setCursor(0, 1);
  lcd.print(illuminance);
  lcd.print(" lux");

  delay(2000); // Display light intensity for 2 seconds

  // Send sensor data to ESP32
  String sensorData = String(temperature) + "," + String(humidity) + "," + String(rainDigital) + "," + String(rainAnalog) + "," + String(illuminance);
  Serial3.println(sensorData); // Send data to ESP32 via Serial3
}