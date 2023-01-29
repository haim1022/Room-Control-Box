void shadesSetPosition(byte shadesPreset) //Every call to change the digital state of the shades pins will need to change both of them as a fail safe
{
  if(shadesRequestedPreset == NULL) //No user requested preset to handle, make sure motor is not running
  {
    digitalWrite(RELAY_UP_PIN, HIGH);
    digitalWrite(RELAY_DOWN_PIN, HIGH);
  }
  //Option to pause by requesting the same preset again - mainly for IR remote control
  if((shadesPreset == SHADES_PRESET_FULLY_OPEN && shadesRequestedPreset == SHADES_PRESET_FULLY_OPEN) || (shadesPreset == SHADES_PRESET_FULLY_CLOSE && shadesRequestedPreset == SHADES_PRESET_FULLY_CLOSE))
    shadesRequestedPreset = NULL;
  else if(shadesPreset != 0)
  {
    lastShadesPresetChange = millis();
    shadesPresetStep = 0;
    shadesRequestedPreset = shadesPreset;
    if(shadesRequestedPreset == SHADES_PRESET_FULLY_CLOSE || shadesRequestedPreset == SHADES_PRESET_HALF || shadesRequestedPreset == SHADES_PRESET_QUARTERLY_OPEN)
    {
      digitalWrite(RELAY_UP_PIN, HIGH);
      delay(5); //Prevent any unwanted pin state before activating another pin
      digitalWrite(RELAY_DOWN_PIN, LOW);
    }
    else if(shadesRequestedPreset == SHADES_PRESET_FULLY_OPEN || shadesRequestedPreset == SHADES_PRESET_QUARTERLY_CLOSE)
    {
      digitalWrite(RELAY_DOWN_PIN, HIGH);
      delay(5);
      digitalWrite(RELAY_UP_PIN, LOW);
    }
    else if(shadesRequestedPreset == SHADES_PAUSE)
      shadesRequestedPreset = NULL;
  }
  else //shadesPreset == NULL
  {
    if(shadesRequestedPreset == SHADES_PRESET_FULLY_CLOSE && ((millis() - lastShadesPresetChange) > SHADES_DURATION_FULLY_CLOSE)
        || shadesRequestedPreset == SHADES_PRESET_FULLY_OPEN && ((millis() - lastShadesPresetChange) > SHADES_DURATION_FULLY_OPEN))
      shadesRequestedPreset = NULL;
    else if(shadesPresetStep == 0 && ((shadesRequestedPreset == SHADES_PRESET_HALF || shadesRequestedPreset == SHADES_PRESET_QUARTERLY_OPEN) && ((millis() - lastShadesPresetChange) > SHADES_DURATION_FULLY_CLOSE)
        || shadesRequestedPreset == SHADES_PRESET_QUARTERLY_CLOSE && ((millis() - lastShadesPresetChange) > SHADES_DURATION_FULLY_OPEN)))
      shadesPresetStep = 1;
    if(shadesPresetStep == 1)
    {
      if((shadesRequestedPreset == SHADES_PRESET_HALF && (millis() - lastShadesPresetChange) < (SHADES_DURATION_FULLY_CLOSE + SHADES_DURATION_FULLY_OPEN/2)) || (shadesRequestedPreset == SHADES_PRESET_QUARTERLY_OPEN && (millis() - lastShadesPresetChange) < (SHADES_DURATION_FULLY_CLOSE + SHADES_DURATION_FULLY_OPEN/4)))
      {
        digitalWrite(RELAY_DOWN_PIN, HIGH);
        delay(5);
        digitalWrite(RELAY_UP_PIN, LOW);
      }
      else if(shadesRequestedPreset == SHADES_PRESET_QUARTERLY_CLOSE && (millis() - lastShadesPresetChange) < (SHADES_DURATION_FULLY_OPEN + SHADES_DURATION_FULLY_CLOSE/4))
      {
        digitalWrite(RELAY_UP_PIN, HIGH);
        delay(5);
        digitalWrite(RELAY_DOWN_PIN, LOW);
      }
      else //All shades presets has been handled
        shadesRequestedPreset = NULL;
    }
  }
}
