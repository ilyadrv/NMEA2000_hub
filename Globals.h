//============ SYSTEM
int btnLongPress = 1000; // how long is the longpress ms
bool SD_Present = true;
String uptime(){
    char buffer[20];
    unsigned long _sec = trunc(millis()/1000);
    int _h = trunc(_sec/3600);
    int _m = trunc((_sec - _h * 3600)/60);
    int _s = _sec - _h * 3600 - _m * 60;
    sprintf(buffer, "%d:%02d:%02d", _h, _m, _s);
    return String(buffer);
}

unsigned int _pseudo_fps = 0;

void _debug(char* str) {
    //if (!DeviceConfig.debug) return;
    Serial.println(str);
}

void _debug(String str) {
    //if (!DeviceConfig.debug) return;
    char buffer[255];
    str.toCharArray(buffer, 255);
    _debug(buffer);
}

//==================== GPS
struct tMyGpsData {
    uint16_t
        DaysSince1970 = 0;
    unsigned long
        Time = 0; //time in sec since 1970 with correction
    double
        SecondsSinceMidnight = 0,
        Latitude = -1000,
        Longitude = -1000,
        Altitude = -1000,
        Variation = -1000,
        SOG = -1000,
        COG = -1000;
    String
        Source = "";
};

//======= state and config
struct tDeviceState {
  int cur_page = 0, lcd_brightness = 10;
  bool loop_pages = true;
  long Time = 0;
};
tDeviceState DeviceState;
Preferences prefs;




