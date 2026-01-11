#include <LiquidCrystal.h>
#include <stdio.h>
#include <Arduino.h>

// Liquid Crystal display setup
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Date Time Structure
struct Time
{
  int months;
  int days;
  int hours;
  int minutes;
  int seconds;
};

// Motor Run Time Structure
struct MotorRunTime
{
  int hours;
  int minutes;
  int seconds;
};

// Time reset values
const Time RESET_TIME = {0, 0, 0, 0, 0};

// Initialize your time variables with the reset values
Time currentTime = RESET_TIME;

// Create a constant Motor Run Time structure with reset values
const MotorRunTime MOTOR_RESET_TIME = {0, 0, 0};

// Initialize your time variables with the reset values
MotorRunTime mixTimes = MOTOR_RESET_TIME;

// Pin Definitions
const int manualSwitchPin = 7;  // Manual switch input for motor
const int relayPin = 8;         // Relay output to control motor
const int controlSwitchPin = 9; // Pin connected to the toggle switch

String batchState = "Stopped";
int revolution = 0;                 // Number of rotations
const int revolutionCorrection = 1; // Number of rotations till a correction is needed for tumbler location
const int mixSetTime = 1;

const unsigned long lcdRefreshInterval = 3000; // 3 seconds in milliseconds
unsigned long lastLCDRefresh = 0;              // Variable to store the last refresh time

// Variables for state change detection
int lastManualSwitchState = LOW; // Initially assume manual switch is off

void setup()
{
  lcd.begin(16, 2);
  delay(50); // Small delay to stabilize LCD
  pinMode(controlSwitchPin, INPUT_PULLUP);
  pinMode(manualSwitchPin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Ensure relay is off at start
  updateLCD();
  // debub monitoring
  Serial.begin(9600); // In setup()
}

void loop()
{
  // Read the current state of the toggle switch
  int currentControlSwitchState = digitalRead(controlSwitchPin);
  // Manual switch to turn motor on/off
  int manualSwitchState = digitalRead(manualSwitchPin);

  // Debugging monitoring
  Serial.print("\n Batch State: ");
  Serial.print(batchState);
  Serial.print("\n Revolution: ");
  Serial.print(revolution);
  Serial.print("\n Revolution Correction: ");
  Serial.print(revolutionCorrection);
  Serial.print("\n mixTimes Minutes: ");
  Serial.print(mixTimes.minutes);
  Serial.print("\n mixTimes Seconds: ");
  Serial.print(mixTimes.seconds);
  Serial.println("");

  // Periodically reset/refresh the LCD screen
  if (millis() - lastLCDRefresh >= lcdRefreshInterval)
  {
    refreshLCD();              // Re-draw the LCD content
    lastLCDRefresh = millis(); // Update last refresh time
  }

  // Check if both switches are LOW
  if (currentControlSwitchState == LOW && manualSwitchState == LOW)
  {
    // Both switches are on (LOW due to INPUT_PULLUP), stop everything
    batchState = "Stopped";
    digitalWrite(relayPin, LOW); // Ensure motor is off
  }
  else if (currentControlSwitchState == LOW && manualSwitchState == HIGH)
  {
    if (batchState != "Running" && batchState != "Mixing")
    {
      batchState = "Running";
      // resetTime();  // Reset time when starting from Stopped or Mixing
      // resetMixTimes();
    }
  }
  else if (currentControlSwitchState == HIGH && manualSwitchState == LOW)
  {
    if (batchState != "Running" && batchState != "Mixing")
    {
      batchState = "Manual";
      // resetTime();  // Reset time when starting from Stopped or Mixing
      // resetMixTimes();
    }
  }
  else
  {
    batchState = "Stopped";
    digitalWrite(relayPin, LOW); // Ensure motor is off
  }

  /*
  If Batch State is Stopped
  */
  if (batchState == "Stopped")
  {
    refreshLCD(); // Re-draw the LCD content
    resetTime();
    resetMixTimes();
    digitalWrite(relayPin, LOW);
  }

  // ============================ Manual Motor Control ============================

  if (batchState == "Manual")
  {
    refreshLCD(); // Re-draw the LCD content
    resetTime();  // Reset time when stopping
    resetMixTimes();
    revolution = 0;
    digitalWrite(relayPin, HIGH); // Turn motor ON
  }

  // ============================ Batch State = "Running" or "Mixing" ============================

  if (batchState == "Running" || batchState == "Mixing")
  {
    refreshLCD();       // Re-draw the LCD content
    updateTimer();      // Update Running Timer
    updateMixingTime(); // Update Mixing Timer

    // Start the Mixing
    if (mixTimes.hours == mixSetTime)
    { // Time to start mixing
      batchState = "Mixing";
      resetMixTimes();              // Reset Mixing Timer
      digitalWrite(relayPin, HIGH); // Turn motor ON
    }

    // Stopping the Mixing
    if (batchState == "Mixing" && mixTimes.minutes == 2)
    { // If mixing and control switch is LOW, check if mixing time is over
      if (revolution < revolutionCorrection && mixTimes.seconds == 6)
      {
        batchState = "Running";
        digitalWrite(relayPin, LOW);
        resetMixTimes();
        revolution++;
      }
      if (revolution == revolutionCorrection && mixTimes.seconds == 7)
      {
        batchState = "Running";
        digitalWrite(relayPin, LOW);
        resetMixTimes();
        revolution = 0;
      }
    }
  }

  delay(1000); // Update every second when running
}

void refreshLCD()
{
  resetLCD();
  updateLCD();
}

// Update LCD
void updateLCD()
{
  static unsigned long lastSwitchTime = 0;
  static int screenState = 0; // 0, 1, or 2 for the three screens

  if (millis() - lastSwitchTime >= 10000 && batchState != "Stopped")
  {                                      // Switch display every 5 seconds
    screenState = (screenState + 1) % 3; // Cycle through 0, 1, 2
    lastSwitchTime = millis();
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Batch: ");
  lcd.print(batchState);

  // Second row: Switch between current time and time since last run
  lcd.setCursor(0, 1);
  if (batchState == "Stopped" || batchState == "Mixing" || batchState == "Manual")
  {
    lcd.setCursor(0, 0);
    lcd.print("Batch: ");
    lcd.print(batchState);
    lcd.setCursor(0, 1);
    lcd.print("                "); // 16 spaces to clear the line
  }
  else
  {
    switch (screenState)
    {
    case 0: // Screen 1: Batch State and time since started
      lcd.setCursor(0, 0);
      lcd.print("Batch: ");
      lcd.print(batchState);
      lcd.setCursor(0, 1);
      char timeStr[20];
      sprintf(timeStr, "%02d:%02d:%02d:%02d:%02d", currentTime.months, currentTime.days, currentTime.hours, currentTime.minutes, currentTime.seconds);
      lcd.print(timeStr);
      break;

    case 1: // Screen 2: Time since last run
      lcd.setCursor(0, 0);
      lcd.print("Time since last   ");
      lcd.setCursor(0, 1);
      char timeSinceStr[12]; // Buffer for formatted time since last run
      sprintf(timeSinceStr, "%02d:%02d:%02d", mixTimes.hours, mixTimes.minutes, mixTimes.seconds);
      lcd.print("run: ");
      lcd.print(timeSinceStr);
      break;

    case 2: // Screen 3: Time for mixes
      lcd.setCursor(0, 0);
      lcd.print("Mix Time every:       ");
      lcd.setCursor(0, 1);
      lcd.print(mixSetTime);
      lcd.print(" hours");
      break;
    }
  }
}

// Update Timer
void updateTimer()
{
  currentTime.seconds++;

  // Increase Months after 30 days
  if (currentTime.days > 30)
  {
    currentTime.months++;
    currentTime.days = 0; // Reset days after increasing months
  }

  if (currentTime.seconds >= 60)
  {
    currentTime.seconds = 0;
    currentTime.minutes++;
    if (currentTime.minutes >= 60)
    {
      currentTime.minutes = 0;
      currentTime.hours++;
      if (currentTime.hours >= 24)
      {
        currentTime.hours = 0;
        currentTime.days++;
      }
    }
  }
}

// Reset time to initial values
void resetTime()
{
  currentTime = RESET_TIME;
}

// Reset Motor Run time to initial values
void resetMixTimes()
{
  mixTimes = MOTOR_RESET_TIME;
}

void resetLCD()
{
  lcd.clear();
  // Optionally, you can re-initialize the LCD if necessary
  lcd.begin(16, 2);
  updateLCD(); // Redraw with current state
}

// Update Timer
void updateMixingTime()
{
  mixTimes.seconds++;

  if (mixTimes.seconds >= 60)
  {
    mixTimes.seconds = 0;
    mixTimes.minutes++;
    if (mixTimes.minutes >= 60)
    {
      mixTimes.minutes = 0;
      mixTimes.hours++;
      if (mixTimes.hours >= 24)
      {
        mixTimes.hours = 0;
        mixTimes.minutes = 0;
        mixTimes.seconds = 0;
      }
    }
  }
}