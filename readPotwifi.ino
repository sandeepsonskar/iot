#include <WiFi.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WiFiServer server(80);

// Motor Pins
#define ENA 25
#define IN1 26
#define IN2 27

const int pwmChannel = 0;
const int pwmFreq = 1000;
const int pwmResolution = 8;

int motorSpeed = 0;

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(ENA, pwmChannel);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  WiFi.begin(ssid, password);
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
    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("/forward") != -1) {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
    }

    if (request.indexOf("/reverse") != -1) {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
    }

    if (request.indexOf("/stop") != -1) {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
    }

    if (request.indexOf("/speed=") != -1) {
      int pos = request.indexOf("=");
      motorSpeed = request.substring(pos + 1).toInt();
      ledcWrite(pwmChannel, motorSpeed);
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();

    client.println("<!DOCTYPE html><html>");
    client.println("<head><meta name='viewport' content='width=device-width, initial-scale=1'>");
    client.println("<style>body{text-align:center;font-family:Arial;} button{padding:10px 20px;margin:5px;font-size:18px;} input{width:80%;}</style>");
    client.println("</head><body>");
    client.println("<h2>ESP32 DC Motor Control</h2>");

    client.println("<a href='/forward'><button>Forward</button></a>");
    client.println("<a href='/reverse'><button>Reverse</button></a>");
    client.println("<a href='/stop'><button>Stop</button></a><br><br>");

    client.println("<form action='/'>");
    client.print("Speed: <input type='range' name='speed' min='0' max='255' value='");
    client.print(motorSpeed);
    client.println("' oninput='this.form.submit()'>");
    client.println("</form>");

    client.println("</body></html>");
    client.stop();
  }
}
