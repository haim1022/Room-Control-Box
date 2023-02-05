void handleAPI()
{
  //Handle HTML page first
  File file = SPIFFS.open("/api_settings.html", "r"); //open file for reading
  if (!file)
  {
    handleNotFound();
    return;
  }
  server.send(200, "text/html", file.readString());
  file.close();

  if(server.args() > 0)
  {
    file = SPIFFS.open(CONFIG_FILE_NAME, "w");
    StaticJsonDocument<CONFIG_FILE_SIZE> doc;

    if(server.hasArg("MDNS_ADDRESS"))
      _config._mDNS = server.arg("MDNS_ADDRESS");
    doc["mDNS address"] = _config._mDNS;
    
    JsonObject Wi_Fi = doc.createNestedObject("Wi-Fi");
    if(server.hasArg("WIFI_SSID"))
      _config._wifi._SSID = server.arg("WIFI_SSID");
    if(server.hasArg("WIFI_PASS"))
      _config._wifi._PASS = server.arg("WIFI_PASS");
    
    JsonObject FTP_credentials = doc.createNestedObject("FTP credentials");
    if(server.hasArg("FTP_USERNAME"))
      _config._FTPUsername = server.arg("FTP_USERNAME");
    if(server.hasArg("FTP_PASSWORD"))
      _config._FTPPassword = server.arg("FTP_PASSWORD");

    JsonObject Spotify = doc.createNestedObject("Spotify");
    if(server.hasArg("SPOTIFY_CLIENT_ID"))
      _config._spotify._clientID = server.arg("SPOTIFY_CLIENT_ID");
    if(server.hasArg("SPOTIFY_CLIENT_SECRET"))
      _config._spotify._clientSecret = server.arg("SPOTIFY_CLIENT_SECRET");

    Wi_Fi["SSID"] = _config._wifi._SSID;
    Wi_Fi["Password"] = _config._wifi._PASS;
    FTP_credentials["Username"] = _config._FTPUsername;
    FTP_credentials["Password"] = _config._FTPPassword;
    Spotify["Client ID"] = _config._spotify._clientID;
    Spotify["Client secret"] = _config._spotify._clientSecret;

    serializeJsonPretty(doc, file);
    file.close();

    ESP.reset(); //Reset ESP to apply changes
  }
}
