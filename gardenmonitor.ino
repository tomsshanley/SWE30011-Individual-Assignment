#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <dht.h>

#define DISPLAY_ADDRESS 0x70
#define DHT11_PIN 8 
#define SOIL_PIN A0       // Define the pin where the DHT11 is connected
#define BUTTON_PIN 2
Adafruit_AlphaNum4 disp = Adafruit_AlphaNum4();
dht DHT;

char display_buffer[5];
long last_display_refresh = 0L;

// Current state of 7 segment display
int displayToggle = 0;

// Button state
int lastButtonState = HIGH;

int temp, moisture, humidity;

void setup() {
 Serial.begin(9600);
 pinMode(BUTTON_PIN, INPUT_PULLUP);
 disp.begin(DISPLAY_ADDRESS);
 disp.setBrightness(4);
 disp.clear();
 disp.writeDisplay();

 // Interrupt, swith display mode on button press
 attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, RISING);
}


void loop() {
  int chk = DHT.read11(DHT11_PIN);
  temp = DHT.temperature; 
  humidity = DHT.humidity;

  moisture = analogRead(SOIL_PIN);
  
  // Print to serial monitor (in json formatted string)
  Serial.print("sensor-data {\"temp\":");
  Serial.print(temp);
  Serial.print(", \"humidity\":");
  Serial.print(humidity);
  Serial.print(", \"moisture\":");
  Serial.print(moisture);
  Serial.println("}");

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    if (input == "toggle-display") {
      displayToggle = (displayToggle + 1) % 3;
      Serial.println("Display toggled through serial");
    }
  }

  long current_millis = millis();
  if (current_millis - last_display_refresh > 300) {
    switch (displayToggle) {
    case 0:
      displayTemp();
      break;
    case 1:
      displayHumidity();
      break;
    case 2:
      displayMoisture();
      break;
    default:
      displayToggle = 0;
      break;
    }
    
    // Update the timing variable
    last_display_refresh = current_millis;
    delay(1000);
  }
}

// Interrupt service routine for button press
void handleButtonPress() {
  Serial.println("Interrupt occured");
  displayToggle = (displayToggle + 1) % 3; // Increment caseIndex and wrap around
}

// Switches display to display temperature
void displayTemp() {
  if (isnan(temp)) {
    strcpy(display_buffer, "Err");  // Display error if reading fails
  } else {
    sprintf(display_buffer,"%3dC", temp);  // Format the temperature reading
  }
  // Send the text to the display
  for (int i = 0; i < 4; i++) {
    disp.writeDigitAscii(i, display_buffer[i]);
  }
    
  // Display the text
  disp.writeDisplay();
  }
void displayHumidity() {
  if (isnan(humidity)) {
    strcpy(display_buffer, "Err");  // Display error if reading fails
  } else {
    sprintf(display_buffer,"%3d%%", humidity);  // Format the temperature reading
  }
  // Send the text to the display
  for (int i = 0; i < 4; i++) {
    disp.writeDigitAscii(i, display_buffer[i]);
  }
    
  // Display the text
  disp.writeDisplay();
  }
void displayMoisture() {
  if (isnan(moisture)) {
    strcpy(display_buffer, "Err");  // Display error if reading fails
  } else {
    sprintf(display_buffer,"%3d%%", moisture);  // Format the temperature reading
  }
  // Send the text to the display
  for (int i = 0; i < 4; i++) {
    disp.writeDigitAscii(i, display_buffer[i]);
  }
    
  // Display the text
  disp.writeDisplay();
  }
