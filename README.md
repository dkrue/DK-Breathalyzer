# DK-Breathalyzer
I've been experimenting with an Arduino based alcohol breathalyzer and driving a 7-segment LED with PWM techniques. It's a fun thing to break out at a party.

![Arduino Breathalyzer](/images/breathalyzer.jpg)
## How it works
Plug it in to a USB battery power souce and blow into the sensor. The sensor does need to warm up for a few minutes before stabilizing.
## Ingredients
An inexpensive Pro Micro board, a 4 digit 7-segment LED, and a cheap gas sensor (FC-22).
## Interesting Twist
Usually an LED requires a resistor to limit the flow of current (_aka not blow it up_). This project avoids that through the use of PWM (_pulse width modulation_) to control the brightness of the 7-segment LED display.