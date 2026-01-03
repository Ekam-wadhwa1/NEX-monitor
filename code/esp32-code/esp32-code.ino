//made by ekam wadhwa
//student of faith academy school
//about me:i am a tech enthusiast and love to work on iot projects

//Project: ESP32 Environmental Monitoring System
//Description: This code is designed to run on an ESP32 microcontroller to monitor various environmental parameters
// such as temperature, humidity, air quality, and particulate matter. It utilizes multiple sensors and
// displays the data on an OLED screen. The system can also send alerts based on predefined thresholds.

// Include necessary libraries
// Libraries for sensors, display, and other functionalities
// Make sure to install these libraries via the Arduino Library Manager
// or download them from GitHub if not already installed.
// Example libraries included below:
// Note: Adjust the included libraries based on your actual hardware and requirements.
// For ESP32 development

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <Adafruit_MPU6050h>
#include <Adafruit_Sensor.h>
#include <MHZ19.h>
#include <PMS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <FastLED.h>
#include <arduinoFFT.h>
#include <Adafruit_CCS811.h>
#include <WiFi.h>


// Define constants and pins

//I2C pins for ESP32
#define SDA_PIN 8
#define SCL_PIN 9

//OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

//rotary encoder (KY040)
#define ENC_A 2
#define ENC_B 3
#define ENC_BTN 4

//analog sensors
#define PH_PIN 1
#define TDS_PIN 5
#define TURBIDITY_PIN 6
#define MQ2_PIN 7
#define MQ7_PIN 11
#define MQ135_PIN 12
#define SOUND_PIN 10

//Ultrasonic
#define TRIG_PIN 14
#define ECHO_PIN 15

//ds18b20B20
#define ONE_WIRE_BUS 16

//RGB LED matrix 
#define LED_PIN 17
#define NUM_LEDS 64

//buzzer
#define BUZZER_PIN 18


//create objects for sensors and display

// ---------- OLED ----------
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// ---------- Sensors ----------
Adafruit_BME280 bme;
Adafruit_MPU6050 mpu;
Adafruit_CCS811 ccs;

MHZ19 mhz19;
HardwareSerial mhzSerial(1);

PMS pms(Serial2);
PMS::DATA pmsData;

// ---------- Water Temp ----------
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature waterTemp(&oneWire);

// ---------- LED Matrix ----------
CRGB leds[NUM_LEDS];

// ---------- FFT ----------
#define SAMPLES 128
double vReal[SAMPLES];
double vImag[SAMPLES];
arduinoFFT FFT(vReal, vImag, SAMPLES, 40000);


// Global variables

int menuIndex = 0;
unsigned long lastUpdate = 0;
bool buzzerState = false;


//setup function

void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);

  pinMode(BUZZER, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);

  // Sensors
  bme.begin(0x76);
  mpu.begin();
  ccs.begin();

  // CO2
  mhzSerial.begin(9600, SERIAL_8N1, 19, 20);
  mhz19.begin(mhzSerial);

  // PMS5003
  Serial2.begin(9600);

  // Water temp
  waterTemp.begin();

  // LED Matrix
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

  splashScreen();
}


//main loop function

void loop() {
  airMonitoring();
  delay(1500);

  waterMonitoring();
  delay(1500);

  noiseMonitoring();
  delay(1500);

  gasMonitoring();
  delay(1500);
}

//functions for splashScreen

void splashScreen() {
  display.clearDisplay();
  display.setCursor(20, 25);
  display.println("NEX Monitor");
  display.display();
  delay(2000);
}

