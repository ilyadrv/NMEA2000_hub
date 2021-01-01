// NMEA2000 library developer suggested CAN_TX=GPIO16 and CAN_RX=GPIO4.
// AK-Homberger/M5Stack-NMEA-2000-Display-CAN-BUS suggested CAN_TX=GPIO2 and CAN_RX=GPIO5
// I was testing them with CAN_TX=GPIO1 and CAN_RX=GPIO3; and it works but cause serial.print and upload problems
// Finally CAN_TX=GPIO2 and CAN_RX=GPIO35 works good without having serial port issues
#define ESP32_CAN_TX_PIN GPIO_NUM_2
#define ESP32_CAN_RX_PIN GPIO_NUM_35

// So M5Stack wil include acceleration/Gyro unit
#define M5STACK_MPU6886

#include <M5Stack.h>
#include <Time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <NMEA2000_CAN.h>  // This will automatically choose right CAN library and create suitable NMEA2000 object
#include <N2kMessages.h>
#include <math.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>
#include <Free_Fonts.h>

#include "AMeter/ammeter.cpp"
#include "VMeter/voltmeter.cpp" //vmeter shares some ameter definitions, because ameter hardware has the  same vmeter chip inside
#include <TinyGPS++.h>
#include "Environment/Adafruit_BMP280.cpp"
#include "Environment/SHT3X.cpp"
#include "Compass/bmm150.cpp"

#include "Globals.h"
#include "Math.h"
#include "Classes/cConfig.h"
#include "Classes/cBoatData.h"
#include "Classes/cDisplay.h"
#include "Classes/cLog.h"
#include "States.h"

bool _logoShown = false;

//============= Timers
unsigned long
    t = 0,  // Timer for 1 sec events
    t2 = 0,  // Time for 10sec events
    t_loopPages = 0; //timer for page loop

//============= Misc
void ManageDeviceTime() {
    struct timeval tv;
    const timezone *tz = &DeviceConfig.tz;
    if (!BoatData.Gps.TimeValid(DeviceState.Time) && BoatData.Gps.TimeValid()){
        timeval epoch = {BoatData.Gps.Time(), 0};
        const timeval *tv = &epoch;
        settimeofday(tv, tz);
    }
    gettimeofday(&tv, &tz);
    DeviceState.Time = tv.tv_sec;
}

void long_press_beep(){
  M5.Speaker.tone(661, 1); //NOTE_DH2
  delay(200);
  M5.Speaker.mute();
}

bool show_logo(){
    M5.Lcd.clear();
    M5.Lcd.setBrightness(250);
    if (SD.exists("/logo_320x240.bmp")){
        M5.Lcd.drawBmpFile(SD, "/logo_320x240.bmp", 0 ,0);
        return true;
    }
    if(SD.exists("/logo_320x240.png")){
        M5.Lcd.drawPngFile(SD, "/logo_320x240.png");
        return true;
    }
    if(SD.exists("/logo_320x240.jpg")){
        M5.Lcd.drawJpgFile(SD, "/logo_320x240.jpg");
        return true;
    }
    return false;
}

//============== SETUP ==========================
void setup() {
    M5.begin();
    M5.Power.begin();
    Serial.begin(115200);
    _logoShown = show_logo();

    //start I2C
    Wire.begin(); //parameters can be sda = 21, scl = 22, frequency

    //init randomizer
    srand(time(NULL));

    //LOAD CONFIG
    SD.begin();
    DeviceConfig.load();
    SD.mkdir("/logs"); //creates log dir if not exists;

    //load pages from SD
    MainDisplay.Init();

    //load state from flash
    prefs.begin("app");
    load_state(); // recover last shutdown state

    //Init boat data
    BoatData.Init(DeviceConfig);

    //Init logs
    cLog::Init(&BoatData, &DeviceConfig);
}

void loop() {
  M5.update();

//====================== BUTTONS =============================================
  if (M5.BtnB.wasPressed() == true) {
    DeviceState.loop_pages = false;
    MainDisplay.NextPage();
    t -= 1000;
  }

  if (M5.BtnA.wasPressed() == true) {
    DeviceState.loop_pages = false;
    MainDisplay.PrevPage();
    t -= 1000;
  }

   //reset trip start position
  if (M5.BtnA.pressedFor(btnLongPress)) {
    BoatData.Trip.TripStartTime = millis();
    long_press_beep();
    delay(1000);
  }

  //Start loop
  if (M5.BtnB.pressedFor(btnLongPress)) {
    DeviceState.loop_pages = true;
    MainDisplay.NextPage();
    long_press_beep();
    delay(1000);
  }

  if (M5.BtnC.wasPressed() == true)                         /* Button C pressed ? --> Change brightness */
  {
    if (DeviceState.lcd_brightness < 250)                               /* Maximum brightness not reached ? */
    {
        DeviceState.lcd_brightness += 20;                 /* Increase brightness */
    }
    else                                                    /* Maximum brightness reached ? */
    {
      DeviceState.lcd_brightness = 10;                                  /* Set brightness to lowest value */
    }
    M5.Lcd.setBrightness(DeviceState.lcd_brightness);                   /* Change brightness value */
  }

  //reset screen brightness
  if (M5.BtnC.pressedFor(btnLongPress)) {
    DeviceState.lcd_brightness = 10;
    M5.Lcd.setBrightness(DeviceState.lcd_brightness);
    long_press_beep();
    delay(1000);
  }

//==================== Managing DATA

   BoatData.Refresh();
   BoatData.SendToN2K();

//===================== UI
    if (millis() > t_loopPages + DeviceConfig.loop_page_speed * 1000) {
        t_loopPages = millis();
        if (DeviceState.loop_pages){MainDisplay.NextPage();}
    }

    if (millis() > t + DeviceConfig.page_refresh_speed) {
        t = millis();

        //order is important
        ManageDeviceTime();
        if (BoatData.Gps.CoordinatesValid() && BoatData.Gps.TimeValid() && !BoatData.Gps.PositionValid(BoatData.StartupPosition)){
            BoatData.StartupPosition = BoatData.Gps.Data();
        }

        //we do not want to put delay for logo,
        //instead we want things work  for 3 seconds
        if (!_logoShown || millis() > 3000){
            if (_logoShown){
                M5.Lcd.setBrightness(DeviceState.lcd_brightness);
                _logoShown = false;
            }
            MainDisplay.DrawPage();
            //if SD was out it will be considered absent until restart
            //otherwise SD operations will take seconds
            SD_Present = (SD_Present && SD.exists("/logs")); // this folder was created at startup so its a good candidate for being a flag
        }

        //SLOW OPERATIONS
        // must be here because of the  calculated_boat_data();
        if (millis() > t2 + 30 * 1000) {
            t2 = millis();
            save_state();
            takeoff();
        }

        // must be here because of the  calculated_boat_data();
        if (SD_Present) MainDisplay.LogFileError = !cLog::Log(DeviceState.Time);
        _pseudo_fps = 0;
    }
    _pseudo_fps++;
}