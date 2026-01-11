# Jar Tumbler – Automated Vanilla Extract Agitator

Arduino-controlled jar tumbler / rotator system designed to **automatically agitate** a jar of homemade vanilla extract.

This project periodically runs a motor to gently tumble/rotate the jar, improving extraction speed and flavor development compared to passive steeping.

## Why this project?

Homemade vanilla extract is amazing — just vanilla beans + vodka + time.  
Most recipes recommend shaking the jar daily/weekly, but that's easy to forget.

This automated tumbler **runs the motor for short mixing periods** every few hours (configurable), mimicking frequent agitation without manual effort — perfect for speeding up the 2–6+ month extraction process while producing richer flavor.

Great for makers who love baking, DIY food projects, or just hate wasting expensive vanilla extract!

## Features

- **LCD display** (16×2) showing current batch state, running time, mixing timer, etc.
- Three display modes that cycle automatically (batch status + time, time since last mix, mix interval setting)
- **Two control modes**:
  - **Auto mode** — runs continuous timer + periodic short mixing cycles
  - **Manual mode** — forces motor on (great for testing/setup)
- **Safety** — motor only runs when both control & manual switches allow it
- Simple hardware using common parts (Arduino, relay, LCD, toggle switches)
- Revolution counter with optional correction logic (for precise positioning if needed)

## Parts List

Here are the main components used in this build (as of January 2026):

### Core Electronics
- **Arduino Uno** (or compatible clone)  
  [Amazon Link](https://www.amazon.com/dp/B01EWOE0UU)
- **16×2 LCD Display** (HD44780 compatible)  
  [Amazon Link](https://www.amazon.com/dp/B071Y6JX3H)
- **5V Relay Module** (single channel, for motor control)  
  [Amazon Link](https://www.amazon.com/dp/B09G6H7JDT)
- **12V DC Geared Motor** (slow speed, high torque recommended)  
  [Amazon Link](https://www.amazon.com/dp/B07J54C84R)
- **Toggle Switch** (for mode/auto/manual control)  
  [Amazon Link](https://www.amazon.com/dp/B07S2QJKTX)
- **12V Power Supply** (with barrel plug)  
  [Amazon Link](https://www.amazon.com/dp/B0CW2Y42JT)
- **AC-DC Adapter / Power Supply** (12V output)  
  [Amazon Link](https://www.amazon.com/dp/B09BJLQJNX)
- **Fuse Holder** (add 5–10A fuse for safety)  
  [Amazon Link](https://www.amazon.com/dp/B0BF9LDW1P)

### Vanilla Ingredients & Container
- **1/2 Gallon Mason Jar** (wide mouth recommended)
- **1.75L of Good-Quality Vodka** (local or your favorite neutral spirit)
- **Vanilla Beans** (Madagascar, Tahitian, or Mexican — 6–12 beans per jar)  
  [Amazon Link – Example Grade A Beans](https://www.amazon.com/dp/B00PMUNERY)

### Recommended Safety & Mechanical Additions (Not in code but strongly suggested)
- Flyback diode (1N4007) across motor terminals
- 5–10A fuse (for the fuse holder)
- Rubber rollers/wheels or PVC pipe for smooth jar rotation
- Sturdy base/frame (wood, 3D-printed, or metal)
- Optional: Hall effect sensor + magnet for accurate revolution counting

## How It Works (Logic Summary)

- **Auto mode** (`Running` state): Counts total batch time (months:days:hours:min:sec)
- Every `mixSetTime` hours (default: 1 hour) → switches to `Mixing` mode
- In `Mixing`: Runs motor for ~2 minutes + a few extra seconds (with revolution correction logic)
- Then returns to passive `Running` until next cycle
- **Manual mode**: Motor stays on continuously
- **Stopped** state: Everything off, timers reset

## Current Configuration (as of code)

- Mixing interval: every **1 hour**
- Mixing duration: ~**2 minutes** + small correction (configurable via `revolutionCorrection`)
- LCD refresh: every **3 seconds**
- Screen rotation: every **10 seconds** when running

## Installation & Usage

1. Clone/download this repo
2. Open in PlatformIO / Arduino IDE
3. Adjust pin numbers, `mixSetTime`, timing, etc. if needed
4. Upload to your Arduino
5. Wire hardware carefully (especially relay + motor power!)
6. Flip switches → watch it go!

**Safety note**: Use a proper relay and fuse for the motor. Never run high-power motors directly from Arduino pins. Keep alcohol away from sparks/heat.

## Future Improvements (Ideas)

- Add buttons to adjust mix interval / duration on the fly
- RTC module (real time clock) for accurate long-term timing
- Temperature/humidity sensor (vanilla likes stable cool/dark conditions)
- Web/app interface (ESP32 upgrade?)
- Pause/resume functionality
- Better revolution counting (hall sensor / encoder)

## License

MIT License — feel free to use, modify, share!

Happy vanilla making! 🍦  
Questions/PRs welcome.

Made with love for better homemade vanilla.  
(March 1, 2025)
