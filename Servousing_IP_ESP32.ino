#include <WiFi.h>
#include <ESP32Servo.h>

const char* ssid = "Saif's S24 Ultra";
const char* password = "sidku8090";

WiFiServer server(80);
Servo myServo;

int servoPin = 13;  // GPIO13

void setup() {
  Serial.begin(115200);
  myServo.setPeriodHertz(50);    // standard 50 Hz
  myServo.attach(servoPin, 500, 2400); // min/max microseconds for SG90
  myServo.write(90);  // Center position

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("New Client");
  String request = client.readStringUntil('\r');
  client.flush();
  Serial.println(request);

  if (request.indexOf("/?angle=") != -1) {
    int angleStart = request.indexOf("=") + 1;
    int angle = request.substring(angleStart).toInt();
    angle = constrain(angle, 0, 180);  // Limit to valid range
    myServo.write(angle);
    Serial.print("Set angle to: ");
    Serial.println(angle);
  }

  // Send simple response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html\n");
  client.println("<html><body><h2>ESP32 Servo Control</h2><p>Use /?angle=0 to 180</p></body></html>");

  delay(1);
  Serial.println("Client disconnected.");
}
