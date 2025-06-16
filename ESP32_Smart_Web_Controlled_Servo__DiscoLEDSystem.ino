#include <WiFi.h>
#include <ESP32Servo.h>

// Replace with your Wi-Fi credentials
const char* ssid = "Saif's S24 Ultra";
const char* password = "sidku8090";

WiFiServer server(80);
Servo myServo;

// Pin definitions
const int servoPin = 13;
const int ledPins[] = {12, 14, 27, 26};
const int ledCount = 4;

// Servo control variables
bool rotateServo = false;
bool clockwise = true;
int currentAngle = 0;
int rotationSpeed = 100; // Delay in ms

// LED disco mode
bool discoMode = false;
unsigned long lastFlashTime = 0;
int ledIndex = 0;

void setup() {
  Serial.begin(115200);
  
  for (int i = 0; i < ledCount; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  myServo.setPeriodHertz(50);
  myServo.attach(servoPin, 500, 2400);
  myServo.write(90);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();
    Serial.println(request);

    // Servo manual
    if (request.indexOf("/servo=0") != -1) myServo.write(0);
    if (request.indexOf("/servo=90") != -1) myServo.write(90);
    if (request.indexOf("/servo=180") != -1) myServo.write(180);

    // Continuous rotation
    if (request.indexOf("/rotate=on") != -1) rotateServo = true;
    if (request.indexOf("/rotate=off") != -1) rotateServo = false;

    // Direction control
    if (request.indexOf("/dir=cw") != -1) clockwise = true;
    if (request.indexOf("/dir=ccw") != -1) clockwise = false;

    // Disco Mode
    if (request.indexOf("/disco=on") != -1) discoMode = true;
    if (request.indexOf("/disco=off") != -1) discoMode = false;

    // LED Control
    for (int i = 0; i < ledCount; i++) {
      if (request.indexOf("/led" + String(i+1) + "=on") != -1)
        digitalWrite(ledPins[i], HIGH);
      if (request.indexOf("/led" + String(i+1) + "=off") != -1)
        digitalWrite(ledPins[i], LOW);
    }

    // Speed control
    int speedIndex = request.indexOf("/speed=");
    if (speedIndex != -1) {
      int speedValue = request.substring(speedIndex + 7).toInt();
      if (speedValue >= 10 && speedValue <= 1000) rotationSpeed = speedValue;
    }

    // Send web page
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html\n");

    client.println("<html><head><title>ESP32 Control</title><style>");
    client.println("button{padding:10px;margin:5px;font-size:16px;}");
    client.println("</style></head><body><h2>ESP32 Web Control</h2>");

    client.println("<h3>Servo</h3>");
    client.println("<a href='/servo=0'><button>0°</button></a>");
    client.println("<a href='/servo=90'><button>90°</button></a>");
    client.println("<a href='/servo=180'><button>180°</button></a><br>");
    client.println("<a href='/rotate=on'><button>Start</button></a>");
    client.println("<a href='/rotate=off'><button>Stop</button></a>");
    client.println("<a href='/dir=cw'><button>Clockwise</button></a>");
    client.println("<a href='/dir=ccw'><button>Anticlockwise</button></a><br>");

    client.println("<form><label>Speed (10-1000 ms):</label><input name='speed' type='number' min='10' max='1000'><input type='submit' value='Set'></form>");

    client.println("<h3>LEDs</h3>");
    for (int i = 0; i < ledCount; i++) {
      client.println("<a href='/led" + String(i+1) + "=on'><button>LED " + String(i+1) + " ON</button></a>");
      client.println("<a href='/led" + String(i+1) + "=off'><button>OFF</button></a><br>");
    }

    client.println("<h3>Disco Mode</h3>");
    client.println("<a href='/disco=on'><button style='background:#0f0;'>Start</button></a>");
    client.println("<a href='/disco=off'><button style='background:#f00;color:#fff;'>Stop</button></a>");

    client.println("</body></html>");
    delay(1);
    client.stop();
  }

  // Handle continuous servo rotation
  if (rotateServo) {
    if (clockwise) currentAngle += 5;
    else currentAngle -= 5;

    if (currentAngle > 180) currentAngle = 0;
    if (currentAngle < 0) currentAngle = 180;

    myServo.write(currentAngle);
    delay(rotationSpeed);
  }

  // Handle disco flashing
  if (discoMode && millis() - lastFlashTime > 150) {
    for (int i = 0; i < ledCount; i++) digitalWrite(ledPins[i], LOW);
    digitalWrite(ledPins[ledIndex], HIGH);
    ledIndex = (ledIndex + 1) % ledCount;
    lastFlashTime = millis();
  }
} // End of loop
