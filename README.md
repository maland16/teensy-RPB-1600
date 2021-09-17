# Teensy-RPB-1600
Arduino Library for communicating with and programming the Meanwell RPB-1600 via PMBus with a Teensy 4.0. While this library was designed for use with the Teensy 4.0, it's written on top of the Wire library and should be usable other Arduino hardware. I've been using it as part of [this project](https://github.com/maland16/citicar-charger). In the absence of any real documentation (yet) that project, and the curve configurator in this repo, can serve as examples of how to use this library.  

## How to use this library
-Download a zip of this library using the green button above  
-Follow [the instructions here](https://www.arduino.cc/en/guide/libraries) under "Manual installation"  
-Use the public functions defined in "rpb-1600.h" to your heart's content  
-Don't forget to construct a RPB_1600 object and Init()!  
-See the example that's included in this library  

## Curve Configurator  
This example arduino sketch can be used to read data from and write data to the RPB-1600 over the PMBus protocol via I2C.

## Hardware
Communication with the charger is done over I2C using Pins 7 & 8 of CN500 (the smaller 8 pin connector). [See Meanwell's instructions for more details](https://www.meanwell.com/webapp/product/search.aspx?prod=RPB-1600). I didn't find it necessary to use pull up resistors in addition to the ones internal to the teensy, but you milage may vary. I purchased the connectors and crimped my own wires and I highly recommend this. At first I tried to make due with a hacky solution but buying the right connector was infinitely less frustrating.  

## PMBus i2c Protocol
The PMBus protocol can be found on [the PMBus website](https://pmbus.org/specification-archives/). You have to be granted access to the latest specifications, but the older ones (like the version the RPB-1600 uses) are free under their "archives" section. **There were multiple instances where the RPB-1600 datasheet is misleading about how to write or read data from it. Included in this repo is an email exchange between myself and a Meanwell rep who helped me sort through some of the issues I was having.

## Future work