#include <WiFi.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WiFiServer server(80);

const int potPin = 34;   // ADC pin
int potValue = 0;

void setup() {
  Serial.begin(115200);
  pinMode(potPin, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    potValue = analogRead(potPin);   // Read ADC (0–4095)

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();

    client.println("<!DOCTYPE html><html>");
    client.println("<head>");
    client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
    client.println("<meta http-equiv='refresh' content='1'>"); // refresh every 1 sec
    client.println("<style>body{text-align:center;font-family:Arial;} h1{color:#333;}</style>");
    client.println("</head>");
    client.println("<body>");
    client.println("<h1>ESP32 Potentiometer Value</h1>");
    client.print("<h2>");
    client.print(potValue);
    client.println("</h2>");
    client.println("</body></html>");

    client.stop();
  }
}
