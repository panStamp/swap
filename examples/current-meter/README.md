This application sends RMS current values, temperature and and battery voltage periodically, based on the value of Tx Interval. If you are powering the board from a battery pack, we recommend setting Tx Interval to something between 30 and 255 seconds. Configuring values for Tx Interval below that will probably drain the battery too quickly. In any case, if you are powering the board from an external power supply then you can set Tx Interval even below 5 seconds or replace swap.goToSleep() by a simple delay().

This application only provides RMS current in mA. If you want to get active power or power consumption then you will need to do some calculations on the receiver. For active power, simply assume an AC voltage level and a power factor. Power factor is typically assumed to be 0.8 in most domestic cases so for a 240VAC system we would apply this formula:

Active_Power (W) = RMS_Current/1000 x 240 x 0.8

Power consumption (in Wh) requires the receiver or main controller to run some accumulative work. Here the controller needs to know the time (in seconds) elapsed between transmissions and use this value in the following formula:

Power_Consumption (Wh) = Power_Consumption + Active_Power x time_elapsed/3600

This sketch works with our [current meter board](https://github.com/panStamp/panstamp/wiki/Current%20meter).

![current-meter](http://panstamp.org/pictures/current-meter_03.jpg)


