#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WiFiServer server(80);

const int ledPin = 2;  // GPIO 2
String header;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop(){
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {

            if (header.indexOf("GET /on") >= 0) {
              digitalWrite(ledPin, HIGH);
            }
            if (header.indexOf("GET /off") >= 0) {
              digitalWrite(ledPin, LOW);
            }

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name='viewport' content='width=device-width, initial-scale=1'>");
            client.println("<style>body{text-align:center;font-family:Arial;} button{padding:15px 30px;font-size:20px;}</style></head>");
            client.println("<body><h1>ESP32 LED Control</h1>");
            client.println("<p><a href='/on'><button>ON</button></a></p>");
            client.println("<p><a href='/off'><button>OFF</button></a></p>");
            client.println("</body></html>");

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    header = "";
    client.stop();
    Serial.println("Client disconnected.");
  }
}
