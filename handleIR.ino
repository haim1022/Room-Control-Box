void handleIR()
{
  handleRoot();
  uint32_t IRcode;
  byte repeatTimes, codesAmount;
  
  if(server.hasArg("amount"))
    codesAmount = strtoul(server.arg("amount").c_str(), NULL, 10);
  else
    codesAmount = 1;
  if(server.hasArg("repeat"))
    repeatTimes = strtoul(server.arg("repeat").c_str(), NULL, 10);
  else
    repeatTimes = 1;
  for(byte amountCounter = 0; amountCounter < codesAmount; amountCounter++)
  {
    if(server.hasArg("code" + String(amountCounter+1)))
      IRcode = strtoul(server.arg("code" + String(amountCounter+1)).c_str(), NULL, 10);
    else if(server.hasArg("code"))
      IRcode = strtoul(server.arg("code").c_str(), NULL, 10);
    for(uint8_t repeatCounter = 0; repeatCounter < repeatTimes; repeatCounter++)
    {
      irsend.sendNEC(IRcode, 32, IR_REPEAT_TIMES);
      delay(IR_REPEAT_DELAY);
    }
  }
}
