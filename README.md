# T1A - Single Axis Solar Tracker PCB board and Control App

That was first 'full' version of this device, with custom, make from scratch pcb board, with socket to 
connect Arduino Nano, to control just projected and build prototype single axis solar tracker, in company i was co-owner.
<br><br>
I also make app for control this device, for Arduino Nano.
 
<br>

#### Board have connection for:
- Linear Motor Actuator (24V)
- Power DC (24V)

<br>

#### 4 IN/OUT ports with [JST Connectors 2.54mm](https://www.amazon.co.uk/gp/product/B07QNPZDTW/ref=ppx_yo_dt_b_asin_title_o08_s01?ie=UTF8&psc=1):
- **Keyboard** - for the prototype [1x4 Matrix Array 4 Buttons Film Keyboard ](https://www.ebay.ie/itm/223730888564?chn=ps&norover=1&mkevt=1&mkrid=5282-166454-587998-0&mkcid=2&itemid=223730888564&targetid=293946777986&device=c&mktype=pla&googleloc=1007850&poi=&campaignid=20320759638&mkgroupid=152362499922&rlsatarget=pla-293946777986&abcId=&merchantid=5086394679&gclid=Cj0KCQjwpc-oBhCGARIsAH6ote-b_LJOvnV3kze3KVuqcWUIJQtANgfqCDRLPjK4TdgbSaS1SVaoshAaAl8eEALw_wcB)
- **Display / LCD** - [in this case - Youmile IIC/I2C/TWI Serial 2004 20x4 LCD Module](https://www.amazon.co.uk/gp/product/B07PWWTB94/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&psc=1)
- **Solar sensor with accelerometer** - I make board with [4x BPW34 Photodiode](https://www.amazon.co.uk/gp/product/B07HBQNMYW/ref=ppx_yo_dt_b_asin_title_o08_s00?ie=UTF8&psc=1) (as on project images below), and connect it together with [YOUMILE GY-61 ADXL335 3-axis Analog Output Accelerometer](https://www.amazon.co.uk/gp/product/B07TWLPCHJ/ref=ppx_yo_dt_b_asin_title_o05_s00?ie=UTF8&psc=1)
- **Anemometer - [wind speed sensor](https://www.amazon.co.uk/gp/product/B07BMVYBW9/ref=ppx_yo_dt_b_asin_title_o04_s00?ie=UTF8&psc=1)**  


#### Arduino Nano
- [LAFVIN Nano V3.0, Nano board ATmega328P](https://www.amazon.co.uk/gp/product/B07P8ZPTZH/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1)
(any other will work too)
<br>

### Working project with Arduino Nano

[![PCB with Arduino Nano][pcba-working-project-with-arduino-01]](images/pcb-working-with-arduino-nano.jpg)

#### Project without with Arduino Nano

[![PCB with Arduino Nano][pcba-witout-arduino-01]](images/pcb-without-arduino.jpg)

### PCB - project

Project files was send to [PCBWay](https://www.pcbway.com/), and they make PCB with all elements, like on image above.


[![EasyEDA 2D-View][easyeda-2d]](images/EasyEDA-2D-View.jpg)

[![EasyEDA 3D-View 1][easyeda-01]](images/EasyEDA-3D-View-01.jpg)

[![EasyEDA 3D-View 2][easyeda-02]](images/EasyEDA-3D-View-02.jpg)

[![EasyEDA 3D-View 3][easyeda-03]](images/EasyEDA-3D-View-03.jpg)

[![EasyEDA 3D-View 4][easyeda-04]](images/EasyEDA-3D-View-04.jpg)


#### Solar sensor with accelerometer (front) - project

[![Solar sensor front][solar-sensor-project-01]](images/solar-sensor-with-accelerometer-front.jpeg)

#### Solar sensor with accelerometer (back) - project

[![Solar sensor back][solar-sensor-project-02]](images/solar-sensor-with-accelerometer-back.jpeg)

<br>

## App for controlling Single Axis Solar Tracker for my project of PCB with Arduino Nano.

### Function of app

- what solution i used
- why i used this solutions
- that actually one of my first project with Arduino











<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[pcba-working-project-with-arduino-01]: images/pcb-working-with-arduino-nano.jpg
[pcba-with-arduino-01]: images/pcb-project-wiht-arduino.jpeg
[pcba-witout-arduino-01]: images/pcb-without-arduino.jpg
[solar-sensor-project-01]: images/solar-sensor-with-accelerometer-front.jpeg
[solar-sensor-project-02]: images/solar-sensor-with-accelerometer-back.jpeg

[easyeda-01]: images/EasyEDA-3D-View-T1A-01.jpg
[easyeda-02]: images/EasyEDA-3D-View-T1A-02.jpg
[easyeda-03]: images/EasyEDA-3D-View-T1A-03.jpg
[easyeda-04]: images/EasyEDA-3D-View-T1A-04.jpg
[easyeda-2d]: images/EasyEDA-2D-View-T1A.jpg