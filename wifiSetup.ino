void wifiSetup()
{
  if(WiFi.status() != WL_CONNECTED && WiFi.getMode() != WIFI_AP)
  {
    if(_config._wifi._PASS != "")
      WiFi.begin(_config._wifi._SSID, _config._wifi._PASS);
    else if(_config._wifi._SSID != "")
      WiFi.begin(_config._wifi._SSID);
    else
    {
      WiFi.mode(WIFI_AP);
      WiFi.softAP(DEFAULT_AP_SSID, DEFAULT_AP_PASS);
      return;
    }
    unsigned long connectionStartTime = millis();
    while(WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(500);
      if(millis() - connectionStartTime > WIFI_CONNECTION_TIMEOUT)
      {
        WiFi.mode(WIFI_AP);
        WiFi.softAP(DEFAULT_AP_SSID, DEFAULT_AP_PASS);
        return;
      }
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(_config._wifi._SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP().toString());
  }
}
