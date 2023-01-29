void configSetup()
{
  bool configErrF = true, presetErrF = true; //Will be set to false at the end
  bool existF = SPIFFS.exists(CONFIG_FILE_NAME);
  File file;
  StaticJsonDocument<CONFIG_FILE_SIZE> doc;

  if(existF) //Check existence of config file
  {
    file = SPIFFS.open(CONFIG_FILE_NAME, "r"); //open file for reading
    DeserializationError error = deserializeJson(doc, file);
    if (!error)
    {
      _config._wifi._SSID = doc["Wi-Fi"]["SSID"].as<String>();
      _config._wifi._PASS = doc["Wi-Fi"]["Password"].as<String>();
      _config._spotify._clientID = doc["Spotify"]["Client ID"].as<String>();
      _config._spotify._clientSecret = doc["Spotify"]["Client secret"].as<String>();
      _config._FTPUsername = doc["FTP credentials"]["Username"].as<String>();
      _config._FTPPassword = doc["FTP credentials"]["Password"].as<String>();
      _config._mDNS = doc["mDNS address"].as<String>();
      
      configErrF = false;
    }
  }
  if(configErrF)
  {
    if(existF) //File exists but JSON format is corrupted
      SPIFFS.remove(CONFIG_FILE_NAME);

    file = SPIFFS.open(CONFIG_FILE_NAME, "w");
    doc["mDNS address"] = DEFAULT_MDNS_ADDRESS;

    JsonObject Wi_Fi = doc.createNestedObject("Wi-Fi");
    Wi_Fi["SSID"] = "";
    Wi_Fi["Password"] = "";

    JsonObject FTP_credentials = doc.createNestedObject("FTP credentials");
    FTP_credentials["Username"] = DEFAULT_FTP_USERNAME;
    FTP_credentials["Password"] = DEFAULT_FTP_PASSWORD;

    JsonObject Spotify = doc.createNestedObject("Spotify");
    Spotify["Client ID"] = "";
    Spotify["Client secret"] = "";

    serializeJsonPretty(doc, file);
  }
  file.close();
  if(SPIFFS.exists(PRESET_FILE_NAME))
  {
    file = SPIFFS.open(PRESET_FILE_NAME, "r"); //open file for reading
    DeserializationError error = deserializeJson(doc, file);
    if(!error)
    {
      _ac._state = doc["A/C"]["state"].as<bool>();
      _ac._mode = doc["A/C"]["mode"].as<byte>();
      _ac._speed = doc["A/C"]["speed"].as<byte>();
      _ac._temp = doc["A/C"]["temp"].as<byte>();

      presetErrF = false;
    }
  }
  if(presetErrF) //Set default values
  {
    _ac._state = LOW;
    _ac._mode = 1;
    _ac._speed = 1;
    _ac._temp = 23;
  }
}
