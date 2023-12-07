#include "BluetoothSerial.h"
#include <Adafruit_NeoPixel.h>
#include <map>

#define LED_PIN 21
#define LED_COUNT 24

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;
String cmd = "";
int led_location;

std::map<String, int> led_map = {
  {"H12", 11},
  {"H11", 10},
  {"H10", 9},
  {"H09", 8},
  {"H08", 7},
  {"H07", 6},
  {"H06", 5},
  {"H05", 4},
  {"H04", 3},
  {"H03", 2},
  {"H02", 1},
  {"H01", 0},
  {"G12", 12},
  {"G11", 13},
  {"G10", 14},
  {"G09", 15},
  {"G08", 16},
  {"G07", 17},
  {"G06", 18},
  {"G05", 19},
  {"G04", 20},
  {"G03", 21},
  {"G02", 22},
  {"G01", 23}
};
void clearBoard() {
  //loop through all LEDs and set pixel color for each to 0,0,0
  for(int i=0; i<LED_COUNT; i++){
    strip.setPixelColor(i, (0, 0, 0));
  }
  strip.show();
}

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  strip.begin(); // initialize strip
  strip.show(); // clear strip
  strip.setBrightness(50);
}

void loop() {
  if (SerialBT.available()) {
    cmd = SerialBT.readStringUntil('\n');
    cmd.remove(cmd.length()-1, 1);
    Serial.println(cmd);

    // clear all LEDs
    clearBoard();

    auto it = led_map.find(cmd);

    if (it != led_map.end()) {
        // Key found, print the corresponding value
       Serial.println(it->second);
       strip.setPixelColor(it->second, (0, 0, 255));
       strip.setBrightness(100);
       strip.show();
    }
  }
  delay(200);
}
