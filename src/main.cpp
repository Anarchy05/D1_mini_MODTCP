#include <Arduino.h>
#include <ESPWifiConfig.h>
#include <ModbusIP_ESP8266.h>
#include <ElegantOTA.h>
#include <ESP8266WebServer.h>



// put function declarations here:
int Config_reset_btn = 0; // GPIO0, D0 on Node32, D3 on NodeMCU8266. Pressing this button for more than 5-10sec will reset the WiFi configuration
bool debug = true; // Prints info on Serial when true
int inputPin1 = 14; // Adjust this to your actual input pin
int inputPin2 = 12; // Adjust this to your actual input pin
int inputPin3 = 13; // Adjust this to your actual input pin
int inputPin4 = 15; // Adjust this to your actual input pin
int OutputPin1 = 2; // LED Output

const char* OTAuser = "admin";
const char* OTApass = "admin123"; 


ESPWifiConfig WifiConfig("D1_Mini_AP", 80, Config_reset_btn, false, "fallback_wifi", "fallback_pass", debug);
ModbusIP mb;
ESP8266WebServer server(81);


void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    server.on("/", []() {
    server.send(200, "text/plain", "Hi! This is OTA Updates by ElegantOTA. access at /update");
  });

  ElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("OTA server started at port 81");
  ElegantOTA.setAuth(OTAuser, OTApass);
  mb.server(502);
  mb.addIsts(0, 0); // Placeholder for __________ (you can update this with actual data)
  mb.addIsts(1, 0); // Placeholder for __________ (you can update this with actual data)
  mb.addIsts(2, 0); // Placeholder for __________ (you can update this with actual data)
  mb.addIsts(3, 0); // Placeholder for __________ (you can update this with actual data)
  mb.addCoil(0, OutputPin1);// Placeholder for __________ (you can update this with actual data)

    pinMode(inputPin1, INPUT);
    pinMode(inputPin2, INPUT);
    pinMode(inputPin3, INPUT);
    pinMode(inputPin4, INPUT);
    pinMode(OutputPin1, OUTPUT);


 // Initialize WiFi configuration
    if (WifiConfig.initialize() == AP_MODE) {
        WifiConfig.Start_HTTP_Server(0);
    }

    WifiConfig.print_settings();
    WifiConfig.ESP_debug("Hello");
}

#define NO_CONNECTION_RESTART_DELAY 3600000 //ms, 1 hour
#define NO_CONNECTION_GO_WILD_DELAY 1800000 //ms, 30 minutes
unsigned long last_wifi_connect_time = 0;
unsigned long reconnect_delay = 10000;
unsigned long no_conn_restart_delay = 100000000;
int wifiStatus = WiFi.status();

void loop() {

if (WiFi.status() == WL_CONNECTED){
//Serial.println("WiFi connected!");

  //Serial.println("Test");
    digitalWrite(OutputPin1, mb.Coil(0));
    mb.task();
    mb.Ists(0, inputPin1);
    mb.Ists(1, inputPin2);
    mb.Ists(2, inputPin3);
    mb.Ists(3, inputPin4);
    Serial.println(mb.Coil(0));
    server.handleClient();
    ElegantOTA.loop();

}
else{
  WifiConfig.handle(reconnect_delay); 
  if (WifiConfig.ESP_mode == AP_MODE)  //Can't connect to internet while in this mode
  {
    //Connect to the AP wifi myESP_XXXXXX to configure it
  }
  else
  {
    //Client Mode
    if (WifiConfig.ESP_mode == CLIENT_MODE)
    { if(WifiConfig.wifi_connected)
      {
        last_wifi_connect_time = millis();
        reconnect_delay = 100000;
      }
      else
      {       
        reconnect_delay = 30000; //10s by default

        if ((millis() - last_wifi_connect_time) > no_conn_restart_delay) //Restart after 1 hour of no connection
        {
          Serial.println("Restarting...");
          ESP.restart();          
        }
        else{}
      }
    }
    else{}
  }
};
}
