#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

ESP8266WebServer server(80);

float temperature;
float humidity;
DHT dht11(D4, DHT11);

const int light = D6;

void init_wifi(String ssid, String password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Read Temperature and Humidity by DHT11
void readDHTData() {
  temperature = dht11.readTemperature();
  humidity = dht11.readHumidity();
}

void setup(void) {
  // put your setup code here, to run once:
  Serial.begin(115200);
  init_wifi("IoTC604", "ccsadmin");
  server.begin();
  Serial.println("HTTP server started");

  temperature = 0.0;
  humidity = 0.0;
  dht11.begin();

  pinMode(light, OUTPUT);

  server.on(F("/"), HTTP_GET, []() {
    String html = "<html><head>";
    html += "<style> body { font-size: 20px; text-align: center; display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh; } </style>";
    html += "</head><body>";
    html += "<h1>Temperature and Humidity on ESP8266</h1>";
    html += "<button onclick='toggleLight(true)' style='background-color: MediumSeaGreen; color: white; font-size: 24px;'>Turn On</button>&nbsp;";
    html += "<button onclick='toggleLight(false)' style='background-color: Tomato; color: white; font-size: 24px;'>Turn Off</button>";
    html += "<p>Temperature: " + String(temperature) + " &#8451;&nbsp;|&nbsp;Humidity: " + String(humidity) + " %</p>";
    html += "<p>Status Light: <span style='color:" + String((digitalRead(light) == HIGH ? "MediumSeaGreen" : "Tomato")) + ";'>" + String((digitalRead(light) == HIGH ? "ON" : "OFF")) + "</span></p>";
    html += "<script>";
    html += "function toggleLight(state) {";
    html += "  var xhr = new XMLHttpRequest();";
    html += "  xhr.open('GET', '/light?state=' + state, true);";
    html += "  xhr.send();";
    html += "  xhr.onload = function() {";
    html += "    location.reload();";
    html += "  };";
    html += "}";
    html += "";
    html += "</script>";
    html += "</body></html>";

    server.send(200, "text/html", html);
  });

  server.on("/light", HTTP_GET, []() {
    String stateParam = server.arg("state");
    if (stateParam == "true") {
      digitalWrite(light, HIGH);
      server.send(200, "text/plain", "ON");
    } else if (stateParam == "false") {
      digitalWrite(light, LOW);
      server.send(200, "text/plain", "OFF");
    } else {
      server.send(400, "text/plain", "Invalid");
    }
  });
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  readDHTData();
}
