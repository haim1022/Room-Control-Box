void handleAC()
{
  handleRoot();
  if(server.args() > 0)
  {
    if(server.hasArg("state"))
      _ac._state = constrain(strtoul(server.arg("state").c_str(), NULL, 10), 0, 1);
    if(server.hasArg("mode"))
      _ac._mode = constrain(strtoul(server.arg("mode").c_str(), NULL, 10), 1, 4);
    if(server.hasArg("speed"))
      _ac._speed = constrain(strtoul(server.arg("speed").c_str(), NULL, 10), 1, 3);
    if(server.hasArg("temp"))
      _ac._temp = constrain(strtoul(server.arg("temp").c_str(), NULL, 10), 16, 30);

    File file = SPIFFS.open(PRESET_FILE_NAME, "w");
    StaticJsonDocument<PRESET_FILE_SIZE> doc;
    JsonObject AC = doc.createNestedObject("A/C");
    
    AC["state"] = _ac._state;
    AC["mode"] = _ac._mode;
    AC["speed"] = _ac._speed;
    AC["temp"] = _ac._temp;
    serializeJsonPretty(doc, file);
    file.close();
    
    tadiran.setState(_ac._state);
    tadiran.setMode(_ac._mode);
    tadiran.setFan(_ac._speed);
    tadiran.setTemeprature(_ac._temp);
    irsend.sendRaw(tadiran.codes, TADIRAN_BUFFER_SIZE, 38);
  }
}
