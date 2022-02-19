# switchtoy
ESP 8266 based wifi switch with LED indicators. Base for simple IOT indicator/switch

## Requirements:

### Hardware:

ESP8266 module  
2x 60MM Arcade buttons (Aliexpress)  
WS2812B strip (2 LEDs) - 60/m  
3d printed case  
2x screws 3x10-15mm  
Optional: Silicone/rubber stick on pads for the bottom

### Software
VS Code  
PlatformIO

## Pinout:
(Wemos markings):  
VN/5V -> led strip power  
GND -> red switch p1, green switch p1, red led p1, green led p1  
D1 -> green switch p2  
D2 -> red switch p2  
D5 -> Green led +  
D6 -> red led +  
D7 -> led strip signal  

##Build instructions
Print the upper and lower part of the case (without support)  
Screw in the switches  
Cut a two LED piece of LED strip, solder wires and place it in the rail  
Wire/solder the board esp8266 as per picture


![Soldering](/hardware/soldering.jpg "Soldering")
![Final product](/hardware/box.jpg "Final product")
