#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <WebServer.h>

class MyWebServer: public WebServer{
protected:
    void setup(String SSID, String ip, String password, String domain){
        IPAddress apIP;
        apIP.fromString(ip);
        WiFi.begin();
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        WiFi.softAP(SSID.c_str(), password.c_str()); // WiFi.softAP(const char* ssid, const char* passphrase = NULL, int channel = 1, int ssid_hidden = 0);
        dnsServer.start(53, domain.c_str(), apIP);
        WiFi.mode(WIFI_MODE_AP);
   }

    String _urlDecode(String input) {
          String s = input;
          s.replace("%20", " ");
          s.replace("+", " ");
          s.replace("%21", "!");
          s.replace("%22", "\"");
          s.replace("%23", "#");
          s.replace("%24", "$");
          s.replace("%25", "%");
          s.replace("%26", "&");
          s.replace("%27", "\'");
          s.replace("%28", "(");
          s.replace("%29", ")");
          s.replace("%30", "*");
          s.replace("%31", "+");
          s.replace("%2C", ",");
          s.replace("%2E", ".");
          s.replace("%2F", "/");
          s.replace("%2C", ",");
          s.replace("%3A", ":");
          s.replace("%3A", ";");
          s.replace("%3C", "<");
          s.replace("%3D", "=");
          s.replace("%3E", ">");
          s.replace("%3F", "?");
          s.replace("%40", "@");
          s.replace("%5B", "[");
          s.replace("%5C", "\\");
          s.replace("%5D", "]");
          s.replace("%5E", "^");
          s.replace("%5F", "-");
          s.replace("%60", "`");
          return s;
    }

public:
    DNSServer dnsServer;
    MyWebServer(int port = 80) : WebServer(port) {} //reuse parent's constructor

    void startWebServer(String SSID, String ip, String password, String domain) {
        setup(SSID, ip, password, domain);
        on("/current.csv", [&]() {
            send(200, "text/csv", cLog::boat_data_csv_headers() + '\n' + cLog::lastBoatData);
        });

        on("/log_files.json", [&]() {
            send(200, "text/json",  cLog::logFiles().c_str());
        });

        serveStatic("/logs/", SD, "/logs/");
        serveStatic("/", SD, "/www/index.html");
        serveStatic("/", SD, "/www/");
        onNotFound([&]() {
          send(200, "text/html", "<a href='/index.html'>GO HOME</a>");
        });
        begin();
    }
};

MyWebServer webServer(80);
