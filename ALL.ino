#include <HX711_ADC.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // LiquidCrystal_I2C library

HX711_ADC LoadCell(4, 5); // Parameters: dt pin, sck pin
LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x27 is the I2C address of the LCD

// Define the weight limits for the percentage calculation
const float maxWeight = 500; // Maximum weight for 100% (500 grams)
const float minWeight = 0;   // Minimum weight (0 grams)
const int numReadings = 10;   // Number of readings to average

float readings[numReadings];   // Array to hold weight readings
int readIndex = 0;             // Index for the readings array
float total = 0;               // Total of readings
float averageWeight = 0;       // Average weight

void setup() {
  Serial.begin(9600);
  lcd.init();      // Initialize the LCD
  lcd.backlight(); // Turn on the backlight

  LoadCell.begin(); // Start connection to HX711
  LoadCell.start(2000); // Allow load cell time to stabilize
  LoadCell.setCalFactor(419); // Replace with your calibrated factor
}

void loop() {
  LoadCell.update(); // Retrieve data from the load cell
  float currentWeight = LoadCell.getData(); // Get the current weight value

  // Update readings array with the current weight
  total -= readings[readIndex];
  readings[readIndex] = currentWeight;
  total += readings[readIndex];
  readIndex = (readIndex + 1) % numReadings;
  averageWeight = total / numReadings; // Calculate average weight

  // Calculate percentage based on average weight
  float percentage = map(averageWeight, minWeight, maxWeight, 0, 100);
  percentage = constrain(percentage, 0, 100); // Ensure percentage is within 0-100

  // Display weight and percentage on the LCD
  lcd.clear(); // Clear the display
  lcd.setCursor(0, 0); // Set cursor to first row
  lcd.print("Weight: "); // Print weight label
  lcd.print(averageWeight, 1); // Print average weight value with one decimal place
  lcd.print(" g"); // Units in grams

  lcd.setCursor(0, 1); // Set cursor to second row
  lcd.print("Percent: "); // Print percentage label
  lcd.print(percentage, 1); // Print percentage value with one decimal place
  lcd.print("%"); // Units in percent

  // Print to the Serial Monitor
  Serial.print("Weight: ");
  Serial.print(averageWeight, 1);
  Serial.print(" g\t");
  Serial.print("Percent: ");
  Serial.print(percentage, 1);
  Serial.println("%");

  delay(100); // Add a small delay to make updates visible and prevent flickering
}

// Helper function to map weight to percentage
float map(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
  return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}
