// libraries to include
#include "BluetoothSerial.h"
#include <Adafruit_NeoPixel.h>
#include <map>

// constants
#define LED_PIN 21
#define LED_COUNT 24

// initialize the strip
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// fluff stuff might not need
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

// variables
BluetoothSerial SerialBT;
String cmd = "";
int led_location;

// establish LED array map
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

// loop over all LEDs and set them all to no color
void clearBoard() {
  for(int i=0; i<LED_COUNT; i++){
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}

// loop over all LEDs and set them all to red
void errorBoard() {
  Serial.println("oops!!");
  for(int i=0; i<strip.numPixels(); i++){
    strip.setPixelColor(i, strip.Color(0, 255, 0));
  }
  strip.show();
  delay(200);
}

// setup function, establish connection, wipe board, and set brightness for LEDs
void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  strip.begin(); // initialize strip
  strip.show(); // clear strip
  strip.setBrightness(100);
}

// main loop
void loop() {
  // we have a device connected
  if (SerialBT.available()) {
    cmd = SerialBT.readStringUntil('\n'); // read command as string
    cmd.remove(cmd.length()-1, 1); // above adds extra chars to the end
    Serial.println(cmd); // for debug

    // clear all LEDs
    clearBoard();

    auto it = led_map.find(cmd); // find matching key in map

    if (it != led_map.end()) {
      // Key found, light the appropriate well
      Serial.println(it->second); // for debug
      strip.setPixelColor(it->second, strip.Color(0, 0, 255));
      strip.show();
    } else {
      // key not found, show error
      errorBoard();
    }
  }
  // delay for smoothing out project
  delay(200);
}
