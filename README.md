## Alarmclock with Nextion Touch Display and ESP8266
Another ESP8266 project that has to do with IoT.
This is an alarm clock with 4 independent alarm times.

### New Version 1.10 for PlatformIO
Due to the problems with the Arduino IDE and the various libraries, I switched to Visual Studio Code and PlatformIO a few years ago. 
This new variant of the project is based entirely on these tools. 
All required libraries are included in the project. (Folder "lib") the ESP core version is set to 2.4.2.
How To: https://platformio.org/install/ide?install=vscode

On this occasion I also made small improvements to the HMI project.  
-> "Wecker_v1.2.HMI" created with the old Nextion Editor v0.53. 
I recommend the LTS version of Nextion to open this project. https://nextion.tech/download/nextion-setup-vLTS.zip
<br />
-> "Wecker_v1.3.HMI" a even newer version for use with recent Nextion Editor 1.63.1!

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
- The internal Real Time Clock (RTC) is synchronized via the Internet.
- The alarm melody is played by a mini MP3 player with an SD card.
- Current and future weather information is obtained from the Internet and displayed as symbols and text.
- An ambient light sensor DIMMs the display in the dark.
- The moon phases are calculated and displayed as symbols

*This alarm clock wakes me up since May 2017 reliably every day.*

### List of resources:

#### Hardware:
- ESP8266:                     http://s.click.aliexpress.com/e/XrrdFeY
- Nextion 3,5" Display:        http://s.click.aliexpress.com/e/bCgigI0u
- Mini MP3 Player:             http://s.click.aliexpress.com/e/IVM1sSy
- Real Time Clock:             http://s.click.aliexpress.com/e/Ex0pRvm
- Ambiente light sensor:       https://s.click.aliexpress.com/e/_AM6MiR
- USB to serial cable for programming the Nextion display: https://s.click.aliexpress.com/e/_9gwYTN

#### Useful tools:
- Cheap but good Fluke Multimeter: https://s.click.aliexpress.com/e/_9Q278J
- Cable set to:                 http://s.click.aliexpress.com/e/s7EqNMre

#### Services used:
- Weather:     https://openweathermap.org
- Time:        https://timezonedb.com (please donate)

### How to use
- Download the project from here as zip. (Green Button on Top)
- Extract the zip in a folder on your PC. Extract also the .rar packet in folder \01 ESP8266.
- After that, the structure should look like this:
<img align="left" src="https://github.com/DIYDave/ESP8266-Alarmclock/blob/1.10/Structure.jpg">
<br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /><br />


#### ESP
- Open the project in VS Code (doubble click on workspace file) and make changes in \scr\Wecker_v1.10.ino for your language. (Name of days, months..)
- Set your own ID and location for weather and time.
- Compile and upload to the ESP.
- Configure the WiFi settings acording to WiFiMagaer page. https://github.com/tzapu/WiFiManager
#### HMI
-  Connect the USB to serial adaptor to your PC.
-  Connect the serial end of the adaptor to the HMI. red-red, black-black, yellow-green, blue-white
-  Upload the programm to HMI

#### If you have built an alarm clock with this, please send me a photo. The email address is at the top of the ESP program

#### Other projects of mine:
Https://www.youtube.com/playlist?list=PLMaFfEa45zsyNV_LPKk5LShPhTd0iBbzv

On the Web: 
www.waldesbuehl.com
(German only)
