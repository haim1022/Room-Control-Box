//Set library flags
#define ARDUINOJSON_ENABLE_COMMENTS 1

//Include libraries
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266FtpServer.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <Tadiran.h>
#include <ArduinoSpotify.h>

//Declare I/O pins
#define RELAY_UP_PIN D6
#define RELAY_DOWN_PIN D7
#define IR_SEND_PIN D2
#define IR_RECV_PIN D1
#define MOTION_SENSOR_PIN D0

//Constatnt parameters
#define WIFI_CONNECTION_TIMEOUT 10000 //ms
#define CONFIG_FILE_NAME "/config.txt"
#define CONFIG_FILE_SIZE 512
#define PRESET_FILE_NAME "/preset.txt"
#define PRESET_FILE_SIZE 128
#define IR_REPEAT_TIMES 1 //Software repeat to assure data transfer
#define IR_REPEAT_DELAY 30 //Delay for user requested repeats (ms)
#define SHADES_DURATION_FULLY_CLOSE 31000 //shade going down is faster than going up
#define SHADES_DURATION_FULLY_OPEN 36000
#define SHADES_PRESET_FULLY_CLOSE 1
#define SHADES_PRESET_FULLY_OPEN 2
#define SHADES_PRESET_HALF 3
#define SHADES_PRESET_QUARTERLY_CLOSE 4
#define SHADES_PRESET_QUARTERLY_OPEN 5
#define SHADES_PAUSE 6
#define MOTION_SENSOR_ACTIVATION_THRESHOLD 20000 //Set how long to wait before triggering again
#define SPOTIFY_COUNTRY_CODE "IL"
#define SPOTIFY_REFRESH_TOKEN "AQDkGzC_0AGcSvd5P-RYVVRRwvFRXU2giwf0K4PHZfo2OH3QYhOQFkqoJDVF_SJ0GBz5IxAVtVf1dYnk9yP-3PdhusUnN_hnEEKpRbIiSMtVYQpY9qO9HSeFeXwkwfDjUJI"
#define SPOTIFY_ACTION_PLAY 0
#define SPOTIFY_ACTION_PAUSE 1
#define SPOTIFY_ACTION_NEXT 2
#define SPOTIFY_ACTION_PREVIOUS 3
#define DEFAULT_AP_SSID "Smartroom"
#define DEFAULT_AP_PASS "00000000"
#define DEFAULT_MDNS_ADDRESS "smartroom"
#define DEFAULT_FTP_USERNAME "smartroom"
#define DEFAULT_FTP_PASSWORD "Aa123456"

//Declare IR codes
#define IR_CODE_SHADES_UP 0xA35CFF00
#define IR_CODE_SHADES_DOWN 0xA25DFF00

//Declare configuration variables
struct _Config
{
  struct _Wifi
  {
    String _SSID;
    String _PASS;
  } _wifi;
  struct _Spotify
  {
    String _clientID;
    String _clientSecret;
  } _spotify;

  String _FTPUsername;
  String _FTPPassword;
  String _mDNS;
} _config;

//Declare air conditioner variables
struct _AC
{
  bool _state;
  byte _mode; //Cold - 1, Wet - 2, Circulate - 3, Hot - 4
  byte _speed;
  byte _temp;
} _ac;

//Declare hardware variables
byte shadesRequestedPreset = NULL; //After user request to change the shades preset, this flag gets a value accordingly, 0 - idle
byte shadesPresetStep = 0; //if the shades are set to any preset other than fully open/close, there is a step to fully close/open in order to know the shades position
unsigned long lastSensorActivation = 0, lastShadesPresetChange = 0;

//Declare spotify API variables
//Any changes made needed to be updated in the API test page HTML file
char spotifyClientID[] = "d9fd06c0c7554f6296a479e249e9a948";
char spotifyClientSecret[] = "d7c9c762efb54342848b803896e44858";
char spotifyScopes[] = "user-read-playback-state%20user-modify-playback-state";
char spotifyCallbackURI[] = "http%3A%2F%2Fsmartroom.local%2Fspotify";

//Declare objects
ESP8266WebServer server(80);
FtpServer ftpSrv;
IRsend irsend(IR_SEND_PIN);
IRrecv irrecv(IR_RECV_PIN, 1024, 50, true);
decode_results results;
Tadiran tadiran(_ac._mode, _ac._speed, _ac._temp, _ac._state);
WiFiClientSecure client;
ArduinoSpotify spotify(client, spotifyClientID, spotifyClientSecret, SPOTIFY_REFRESH_TOKEN);

//Declare functions
void systemRoutine(); //Handle shitty Wi-Fi and shades preset change
void configSetup(); //Load config parameters to system variables
void wifiSetup(); //Handle first startup (AP) and lost connection
void handleCSSStyle(); //Handle CSS styling
void handleJSScript(); //Handle JS scripting
void handleManifest(); //Handle Progressive web app's manifest
void handleServiceWorker(); //Handle the service worker for the PWA
void handleRoot(); //Handle API's interface
void handleAPI(); //Handle API's settings
void handleIR(); //Handle infrared requests
void handleAC(); //Handle A/C requests
void handleACInfo(); //Handle A/C info
void handleShades(); //Handle shades request
void handleTripWire(); //Handle room entrance sensor
void handleSpotify(); //Handle Spotify API authentication
void handleNotFound(); //Error 420
void shadesSetPosition(byte shadesPreset = NULL); //For electric safety reasons, only use this function, 0 - call for shades handle

void setup()
{
  pinMode(RELAY_UP_PIN, OUTPUT);
  pinMode(RELAY_DOWN_PIN, OUTPUT);
  pinMode(IR_SEND_PIN, OUTPUT);
  pinMode(IR_RECV_PIN, INPUT);
  pinMode(MOTION_SENSOR_PIN, INPUT);
  shadesSetPosition(NULL);
  Serial.begin(1000000);
  SPIFFS.begin();
  irsend.begin();
  irrecv.enableIRIn();

  configSetup();

  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  wifiSetup();

  if(MDNS.begin(_config._mDNS))
    Serial.println("MDNS responder started");
  client.setFingerprint(SPOTIFY_FINGERPRINT);
  server.on("/style.css", handleCSSStyle);
  server.on("/main.js", handleJSScript);
  server.on("/manifest.json", handleManifest);
  server.on("/sw.js", handleServiceWorker);
  server.on("/", handleRoot);
  server.on("/api", handleAPI);
  server.on("/ir", handleIR);
  server.on("/ac", handleAC);
  server.on("/acinfo", handleACInfo);
  server.on("/shades", handleShades);
  server.on("/tripwire", handleTripWire);
  server.on("/spotify", handleSpotify);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  ftpSrv.begin(_config._FTPUsername, _config._FTPPassword);
  Serial.println("FTP server started at port 21, log in with username: SmartRoom, password: Aa123456");
}

void loop()
{
  MDNS.update();
  server.handleClient();
  ftpSrv.handleFTP();
  
  //Handle motion sensor);
  if(digitalRead(MOTION_SENSOR_PIN) == HIGH && (millis() - lastSensorActivation > MOTION_SENSOR_ACTIVATION_THRESHOLD))
  {
    lastSensorActivation = millis();
    irsend.sendNEC(4261543426, 32, IR_REPEAT_TIMES); //Room light on
    irsend.sendNEC(3208707840, 32, IR_REPEAT_TIMES); //Equalizer on
  }

  //Handle IR input
  if(irrecv.decode(&results))
  {
    unsigned long decodedVal = strtoul(resultToHexidecimal(&results).c_str(), NULL, 0);
    if(decodedVal == IR_CODE_SHADES_UP)
      shadesSetPosition(SHADES_PRESET_FULLY_OPEN);
    else if(decodedVal == IR_CODE_SHADES_DOWN)
      shadesSetPosition(SHADES_PRESET_FULLY_CLOSE);
  }

  //Handle shades preset
  shadesSetPosition();

  //Make sure the Wi-Fi connection is alive
  wifiSetup();
}
