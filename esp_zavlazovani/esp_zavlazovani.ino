// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ESP8266mDNS.h>


// Replace with your network credentials
const char* ssid     = "";
const char* password = "";

// Set web server port number to 80
ESP8266WebServer server(80);

// timer parts
// need to disable this after 1 minte
unsigned long start_time;
bool started = false;

void stop_system()
{
  started = false;
  Serial.print("STOP");
}



void setup()
{
  Serial.begin(9600);

  const bool ok = LittleFS.begin();
  if (!ok) {
    Serial.println("Fuck!");
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  MDNS.begin("caladan");
  MDNS.addService("http", "tcp", 80);

  server.on("/index", send_index);
  server.on("/on", send_on);
  server.on("/off", send_off);
  server.on("/auto", send_auto);
  server.on("/force", send_force);
  server.begin(); //Start the server
}

void send_index()
{
  /*if (!SPIFFS.exists("index.html"))
    {
    server.send(404, "text/plain", "404: Not Found");
    return;
    }*/

  File file = LittleFS.open("index.html", "r");
  if (!file)
  {
    server.send(404, "text/plain", "404: Not Found");
    return;
  }
  size_t sent = server.streamFile(file, "text/html"); // And send it to the client
  file.close();
}

void send_on()
{
  Serial.print("START");
  server.send(200, "text/plain", "Zapiname!");
  start_time = millis();
  started = true;
}

void send_force()
{
  Serial.print("FORCE");
  server.send(200, "text/plain", "Forcujeme!");
  start_time = millis();
  started = true;
}


void send_auto()
{
  Serial.print("AUTO");
  server.send(200, "text/plain", "Forcujeme!");
  // basically disable check for timer... stupid hack
  started = false;
}

void send_off()
{
  stop_system();
  server.send(200, "text/plain", "Vypiname!");
}

void loop()
{
  MDNS.update();
  if (started)
  {
    const unsigned long current_time = millis();
    if ((current_time - start_time) >= (1 * 60 * 1000))
    {
      stop_system();
    }
  }

  server.handleClient();
}
