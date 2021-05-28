## Alarmclock with Nextion Touch and ESP8266

### New Version 1.10 for PlatformIO
Due to the problems with the Arduino IDE and the various libraries, I switched to Visual Studio Code and PlatformIO a few years ago. 
This new variant of the project is based entirely on these tools. 
All required libraries are included in the project. (Folder "lib") the ESP core version is set to 2.4.2.
How To: https://platformio.org/install/ide?install=vscode

On this occasion I also made small improvements to the HMI project. -> "Wecker_v1.2.HMI" 
I created the HMI project with the old Nextion Editor v0.43. 
I recommend the LTS version of Nextion to open the project. https://nextion.tech/download/nextion-setup-vLTS.zip

### Improvements in v1.10
- Non blocking WiFiManager setting. -> The alarm clock reliably wakes you up even without WiFi
- Save title and volume settings for alarm tone
- All libraries updated 

### If you still want to use the Arduino IDE: 
Place the two .ino files in the same directory. 
Install all libraries from the "lib" folder. 
Install ESP8266 Core version 2.4.2. 

I haven't tested this, but it might work

### ESP8266, 4 Alarms, Nextion HMI, MP3 alarm sound, light sensor, weather forecast, RTC with time sync..

Another ESP8266 project that has to do with IoT.
This is an alarm clock with 4 independent alarm times.
-The internal Real Time Clock (RTC) is synchronized via the Internet.

-The alarm melody is played by a mini MP3 player with an SD card.

-Current and future weather information is obtained from the Internet and displayed as symbols and text.

-An ambient light sensor DIMMs the display in the dark.

-The moon phases are calculated and displayed as symbols

This alarm clock wakes me up since December 2017 reliably every day.

#### List of resources:

#### Hardware:
Hardware:
ESP8266:                     http://s.click.aliexpress.com/e/XrrdFeY

Nextion 3,5" Display:        http://s.click.aliexpress.com/e/bCgigI0u

Mini MP3 Player:             http://s.click.aliexpress.com/e/IVM1sSy

Real Time Clock:             http://s.click.aliexpress.com/e/Ex0pRvm

Ambiente Lichtsensor:        https://s.click.aliexpress.com/e/_AM6MiR

USB to serial cable for programming the Nextion display: https://s.click.aliexpress.com/e/_Am9OsB

Useful:

Cheap but good Fluke Multimeter: https://s.click.aliexpress.com/e/_9Q278J

Cable set to:                 http://s.click.aliexpress.com/e/s7EqNMre


#### Services used:
-Weather:     https://openweathermap.org

-Time:        https://timezonedb.com (please donate)

#### Other projects of mine:

Https://www.youtube.com/playlist?list=PLMaFfEa45zsyNV_LPKk5LShPhTd0iBbzv

On the Web: 
www.waldesbuehl.com
(German only)
