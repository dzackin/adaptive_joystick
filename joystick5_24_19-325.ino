#include <Mouse.h>

/*
  JoystickMouseControl

  Controls the mouse from a joystick on an Arduino Leonardo, Micro or Due.
  Uses a push button to turn on and off mouse control, and
  a second push button to click the left mouse button

  Hardware:
  2-axis joystick connected to pins A0 and A1
  push buttons connected to pin D2 and D3

  The mouse movement is always relative. This sketch reads
  two analog inputs that range from 0 to 1023 (or less on either end)
  and translates them into ranges of -6 to 6.
  The sketch assumes that the joystick resting values are around the
  middle of the range, but that they vary within a threshold.

  WARNING: When you use the Mouse.move() command, the Arduino takes
  over your mouse! Make sure you have control before you use the command.
  This sketch includes a push button to toggle the mouse control state, so
  you can turn on and off mouse control.

  created 15 Sept 2011
  updated 28 Mar 2012
  by Tom Igoe

  this code is in the public domain

*/



// set pin numbers for switch, joystick axes, and LED:
const int switchPin = 2; // switch to turn on and off mouse control
const int mouseLeftButton = 3; // input pin for the mouse left click pushButton
const int mouseRightButton = 4; // input pin for the mouse right click pushButton
const int mouseUpScroll = 5; // input pin for sscroll up
const int mouseDownScroll = 6; // input pin for sscroll up
const int minRange = 6;
const int maxRange = 40;
const signed char scrollLinesPerPress = 5;
const int xAxis = A0; // joystick X axis
const int yAxis = A1; // joystick Y axis
const int ledPin = 13; // Mouse control LED

// parameters for reading the joystick:
int range = 12;                    // output range of X or Y movement
int responseDelay = 5;             // response delay of the mouse, in ms
int threshold = range / 4;                 // resting threshold
int center = range / 2;            // resting position value

boolean mouseIsActive = false; // whether or not to control the mouse
int lastSwitchState = LOW; // previous switch state
int lastScrollUpState = LOW;
int lastScrollDownState = LOW;

/*
   reads an axis (0 or 1 for x or y) and scales the
   analog input range to a range from 0 to range
*/

int readAxis(int thisAxis) {
  // read the analog input:
  int rawReading = analogRead(thisAxis);

  // map the reading from the analog input range to the output range:
  int reading = map(rawReading, 0, 1023, 0, range);

  // if the output reading is outside from the
  // rest position threshold, use it:
  int distance = reading - center;
  if(abs(distance) <= threshold) {
      //Serial.println("Distance too small, settting to 0");
      distance = 0;
  }

  Serial.print("Axis= ");
  Serial.print(thisAxis);
  Serial.print("  Raw= ");
  Serial.print(rawReading);
  Serial.print("  Mapped=");
  Serial.print(reading);
  Serial.print("  threshold=");
  Serial.print(threshold);
  Serial.print("  distance=");
  Serial.print(distance);
  Serial.print("  Range=");
  Serial.println(range);

  // return the distance for this axis:
  return distance;
}

void setup() {
  pinMode(switchPin, INPUT);
  pinMode(mouseLeftButton, INPUT);
  pinMode(mouseRightButton, INPUT);
  pinMode(ledPin, OUTPUT); // the LED pin
  Serial.begin(9600);
  // take control of the mouse:
  Mouse.begin();
}

void loop() {
  // WAS read the switch:, but we always want it on now
  // int switchState = digitalRead(switchPin);
  int switchState = HIGH;
  // if it's changed and it's high, toggle the mouse state:

  if (switchState == HIGH) {
    mouseIsActive = !mouseIsActive;
    // turn on LED to indicate mouse state:
    digitalWrite(ledPin, mouseIsActive);

    // save switch state for next comparison:
    lastSwitchState = switchState;

    // read and scale the two axes:
    int xReading = readAxis(xAxis);
    int yReading = readAxis(yAxis);


    // if the mouse control state is active, move the mouse:
    if (mouseIsActive) {
      Mouse.move(-xReading, yReading, 0);
    }

    // read the mouse button and click or not click:
    // if the mouse button is pressed:
    if (digitalRead(mouseLeftButton) == HIGH) {
      // if the mouse is not pressed, press it:
      Serial.println("Left pushed");

      // If the right button and a scroll button is also pressed, then adjust joystick range
      if (digitalRead(mouseRightButton) == HIGH) {
        if (digitalRead(mouseUpScroll) == HIGH && range < maxRange) {
          range++;
          Serial.println("INCREASING RANGE");
        } else if (digitalRead(mouseDownScroll) == HIGH && range > minRange) {
          range--;
          Serial.println("INCREASING RANGE");
        }

        // recalculate ranges
        threshold = range / 4;         // resting threshold
        center = range / 2;            // resting position value
      } else {
        if (!Mouse.isPressed(MOUSE_LEFT)) {
          Serial.println("Mouse left button DOWN");
          Mouse.press(MOUSE_LEFT);
        }
      }
    }
    // else the mouse button is not pressed:
    else {
      // if the mouse is pressed, release it:
      if (Mouse.isPressed(MOUSE_LEFT)) {
        Serial.println("Mouse left button UP");
        Mouse.release(MOUSE_LEFT);
      }
    }

    // read the right mouse button and click or not click:
    // if the mouse button is pressed:
    if (digitalRead(mouseRightButton) == HIGH) {
      Serial.println("Right pushed");
      // if the mouse is not pressed, press it:
      if (!Mouse.isPressed(MOUSE_RIGHT)) {
        Serial.println("Mouse right button DOWN");
        Mouse.press(MOUSE_RIGHT);
      }
    }
    // else the mouse button is not pressed:
    else {
      // if the mouse is pressed, release it:
      if (Mouse.isPressed(MOUSE_RIGHT)) {
        Serial.println("Mouse right button UP");
        Mouse.release(MOUSE_RIGHT);
      }
    }

    // read the scroll up button:
    // if the mouse button is pressed:
    if (digitalRead(mouseUpScroll) == HIGH) {
      Serial.println("Scroll up pushed");
      if (lastScrollUpState == LOW) {
        Serial.println("Scrolling UP");
        for (int x = 0; x < scrollLinesPerPress; x++) {
          Mouse.move(0, 0, -1);
        }
        lastScrollUpState = HIGH;
      }
    }
    else {
      if (lastScrollUpState == HIGH) {
        Serial.println("Resetting scroll UP");
        lastScrollUpState = LOW;
      }
    }


    // read the scroll down button:
    // if the mouse button is pressed:
    if (digitalRead(mouseDownScroll) == HIGH) {
      Serial.println("Scroll down pushed");
      if (lastScrollDownState == LOW) {
        Serial.println("Scrolling DOWN");
        for (int x = 0; x < scrollLinesPerPress; x++) {
          Mouse.move(0, 0, 1);
        }
        lastScrollDownState = HIGH;
      }
    }
    else {
      if (lastScrollDownState == HIGH) {
        lastScrollDownState = LOW;
      }
    }

    delay(responseDelay);
  }
}
