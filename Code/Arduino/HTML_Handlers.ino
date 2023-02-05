void handleRoot()
{
  File file = SPIFFS.open("/index.html", "r"); //open file for reading
  if (!file)
  {
    handleNotFound();
    return;
  }
  server.send(200, "text/html", file.readString());
  file.close();
}

void handleCSSStyle()
{
  File file = SPIFFS.open("/style.css", "r"); //open file for reading
  if (!file)
  {
    handleNotFound();
    return;
  }
  server.send(200, "text/css", file.readString());
  file.close();
}

void handleJSScript()
{
  File file = SPIFFS.open("/main.js", "r"); //open file for reading
  if (!file)
  {
    handleNotFound();
    return;
  }
  server.send(200, "text/javascript", file.readString());
  file.close();
}

void handleManifest ()
{
  File file = SPIFFS.open("/manifest.json", "r"); //open file for reading
  if (!file)
  {
    handleNotFound();
    return;
  }
  server.send(200, "application/json", file.readString());
  file.close();
}

void handleServiceWorker()
{
  File file = SPIFFS.open("/sw.js", "r"); //open file for reading
  if (!file)
  {
    handleNotFound();
    return;
  }
  server.send(200, "text/javascript", file.readString());
  file.close();
}

void handleNotFound()
{
  String message = "Error 420: Page Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}
