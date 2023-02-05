void handleShades()
{
  handleRoot();
  byte shadesWebRequestPreset;

  if(server.hasArg("preset"))
  {
    shadesWebRequestPreset = strtoul(server.arg("preset").c_str(), NULL, 10);
    shadesSetPosition(constrain(shadesWebRequestPreset, 1, 6));
  }
}
