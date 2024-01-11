// libraries to include
#include "BluetoothSerial.h"
#include <Adafruit_NeoPixel.h>
#include <map>

// constants
#define LED_PIN A0
#define LED_COUNT 96


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
String well_color = "";
String well_id = "";
int led_location;
int error_state = 0;
// this is for LED brightness
// needed for pwr mgmt
int bright = 60;
int dim = 10;

uint32_t GREEN = strip.Color(255,   0,   0);
uint32_t RED = strip.Color(  0, 255,   0); 
uint32_t BLUE = strip.Color(  0,   0, 255); 
// establish LED array map
std::map<String, int> led_map = {
  {"H01", 0},
  {"H02", 1},
  {"H03", 2},
  {"H04", 3},
  {"H05", 4},
  {"H06", 5},
  {"H07", 6},
  {"H08", 7},
  {"H09", 8},
  {"H10", 9},
  {"H11", 10},
  {"H12", 11},
  {"G01", 23},
  {"G02", 22},
  {"G03", 21},
  {"G04", 20},
  {"G05", 19},
  {"G06", 18},
  {"G07", 17},
  {"G08", 16},
  {"G09", 15},
  {"G10", 14},
  {"G11", 13},
  {"G12", 12},
  {"F01", 24},
  {"F02", 25},
  {"F03", 26},
  {"F04", 27},
  {"F05", 28},
  {"F06", 29},
  {"F07", 30},
  {"F08", 31},
  {"F09", 32},
  {"F10", 33},
  {"F11", 34},
  {"F12", 35},
  {"E01", 47},
  {"E02", 46},
  {"E03", 45},
  {"E04", 44},
  {"E05", 43},
  {"E06", 42},
  {"E07", 41},
  {"E08", 40},
  {"E09", 39},
  {"E10", 38},
  {"E11", 37},
  {"E12", 36},
  {"D01", 48},
  {"D02", 49},
  {"D03", 50},
  {"D04", 51},
  {"D05", 52},
  {"D06", 53},
  {"D07", 54},
  {"D08", 55},
  {"D09", 56},
  {"D10", 57},
  {"D11", 58},
  {"D12", 59},
  {"C01", 71},
  {"C02", 70},
  {"C03", 69},
  {"C04", 68},
  {"C05", 67},
  {"C06", 66},
  {"C07", 65},
  {"C08", 64},
  {"C09", 63},
  {"C10", 62},
  {"C11", 61},
  {"C12", 60},
  {"B01", 72},
  {"B02", 73},
  {"B03", 74},
  {"B04", 75},
  {"B05", 76},
  {"B06", 77},
  {"B07", 78},
  {"B08", 79},
  {"B09", 80},
  {"B10", 81},
  {"B11", 82},
  {"B12", 83},
  {"A01", 95},
  {"A02", 94},
  {"A03", 93},
  {"A04", 92},
  {"A05", 91},
  {"A06", 90},
  {"A07", 89},
  {"A08", 88},
  {"A09", 87},
  {"A10", 86},
  {"A11", 85},
  {"A12", 84}
};



// setup function, establish connection, wipe board, and set brightness for LEDs
void setup() {
  clearBoard();
  Serial.begin(115200);
  SerialBT.begin("SBBoard"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  strip.begin(); // initialize strip
  strip.show(); // clear strip
  strip.setBrightness(bright);

}

// main loop
void loop() {
  // we have a device connected
  if (SerialBT.available()) {
    cmd = SerialBT.readStringUntil('\n'); // read command as string
    //cmd = SerialBT.readString();
    Serial.println(cmd); // for debug
    //clean input
    cmd.trim();
    cmd.toUpperCase();
    //newline but no data 
    if (cmd.length() <1){
      Serial.println("Bad Command: No Data"); //for debug
      return;
    }
    

    //reset from error
    //this will clear the board even if
    //its a bad input but that's vegas baby
    if (error_state == 1){
      error_state = 0;
      clearBoard();
      strip.setBrightness(bright);
    }

    //clear all LEDs explicitly
    if (cmd.compareTo("CLS") == 0) {
        clearBoard();
        return;
    }
  
    //trigger error
    if (cmd.compareTo("ERROR") == 0) {
        errorBoard();
        return;
    }
    
    //try to break input into well and color
    bool parse_check = parseInput(cmd);
    if (!parse_check){
      Serial.println("Failed to parse: " + cmd); //debug
      return;
    }

    //main LED coloring code
    auto it = led_map.find(well_id); // find matching key in map
    if (it != led_map.end()) {
      // Key found, light the appropriate well
      Serial.println(it->second); // for debug
      //strip.setPixelColor(it->second, strip.Color(0, 0, 255));
      strip.setPixelColor(it->second, getColor());
      strip.show();
    } else {
      Serial.println("Bad Command: " + cmd); //for debug
    }

    // delay for smoothing out project
    delay(10);
  }

}

// loop over all LEDs and set them all to no color
void clearBoard() {
  for(int i=0; i<LED_COUNT; i++){
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}

// loop over all LEDs and set them all to red
void errorBoard() {
  error_state = 1;
  strip.setBrightness(dim); //all 96 on is a high current
  for(int i=0; i<strip.numPixels(); i++){
    strip.setPixelColor(i, RED);
  }
  strip.show();
  delay(100);
}

//convert color string to appropriate type
// based on color variable defined above
uint32_t getColor(){
  std::map<String, uint32_t> color_map = {
  {"BLUE", BLUE},
  {"RED", RED},
  {"GREEN", GREEN}
  };

  auto clr = color_map.find(well_color); // find matching color
  return clr->second;
}

// break input into well and well color
bool parseInput(String inputString){
  well_color = "";
  well_id = "";

  int comma_location = inputString.indexOf(',');
  if (comma_location != -1){
    well_id = inputString.substring(0, comma_location);
    well_color = inputString.substring(comma_location+1, -1);
    return 1;
  }
  else{
      return 0;
    }
  }


//adafruit test code
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}