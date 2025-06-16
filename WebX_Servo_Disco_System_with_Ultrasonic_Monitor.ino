
// ESP32 Web-Controlled Servo + Disco LEDs + Ultrasonic Sensor
#include <WiFi.h>
#include <ESP32Servo.h>

// ==== WiFi Credentials ====
const char* ssid = "Saif's S24 Ultra";
const char* password = "sidku8090";

WiFiServer server(80);
Servo myServo;

// ==== Pins ====
const int servoPin = 13;
const int ledPins[4] = {14, 27, 26, 25};
const int trigPin = 33;
const int echoPin = 32;

// ==== Servo Control ====
bool rotateServo = false;
bool rotateCW = true;
int currentAngle = 0;
int rotationSpeed = 100; // milliseconds

// ==== Disco LED Control ====
bool discoMode = false;
unsigned long lastDiscoTime = 0;
int discoIndex = 0;

// ==== Ultrasonic ====
long duration;
int distanceCM = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  server.begin();
  myServo.setPeriodHertz(50);
  myServo.attach(servoPin, 500, 2400);
  myServo.write(90);

  for (int i = 0; i < 4; i++) pinMode(ledPins[i], OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("Client Connected");
  String request = client.readStringUntil('\r');
  client.flush();
  Serial.println(request);

  // ==== Handle Servo Buttons ====
  if (request.indexOf("/servo=0") != -1) myServo.write(0);
  if (request.indexOf("/servo=90") != -1) myServo.write(90);
  if (request.indexOf("/servo=180") != -1) myServo.write(180);
  if (request.indexOf("/rotate=on") != -1) rotateServo = true;
  if (request.indexOf("/rotate=off") != -1) rotateServo = false;
  if (request.indexOf("/dir=cw") != -1) rotateCW = true;
  if (request.indexOf("/dir=ccw") != -1) rotateCW = false;

  // ==== Handle Speed Slider ====
  int speedIndex = request.indexOf("/speed=");
  if (speedIndex != -1) {
    int val = request.substring(speedIndex + 7).toInt();
    if (val >= 10 && val <= 1000) rotationSpeed = val;
  }

  // ==== LED Buttons ====
  for (int i = 0; i < 4; i++) {
    if (request.indexOf("/led" + String(i) + "=on") != -1) digitalWrite(ledPins[i], HIGH);
    if (request.indexOf("/led" + String(i) + "=off") != -1) digitalWrite(ledPins[i], LOW);
  }

  // ==== Disco Mode ====
  if (request.indexOf("/disco=on") != -1) discoMode = true;
  if (request.indexOf("/disco=off") != -1) discoMode = false;

  // ==== Distance Request ====
  if (request.indexOf("/getDistance") != -1) measureDistance();

  // ==== Web Response ====
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html\n");

  client.println("<html><head><title>ESP32 Control</title><meta name='viewport' content='width=device-width, initial-scale=1'>");
  client.println("<style>button{padding:14px 25px;font-size:16px;margin:5px;}body{font-family:sans-serif;text-align:center;}</style>");
  client.println("</head><body><h2>ESP32 Smart Control Panel</h2>");

  // Servo controls
  client.println("<h3>Servo Controls</h3>");
  client.println("<a href='/servo=0'><button>0°</button></a>");
  client.println("<a href='/servo=90'><button>90°</button></a>");
  client.println("<a href='/servo=180'><button>180°</button></a><br>");
  client.println("<a href='/rotate=on'><button>Start Continuous</button></a>");
  client.println("<a href='/rotate=off'><button>Stop</button></a><br>");
  client.println("<a href='/dir=cw'><button>Clockwise</button></a>");
  client.println("<a href='/dir=ccw'><button>Anticlockwise</button></a><br>");

  // Speed slider (basic input range)
  client.println("<form><label>Speed (ms):</label><input type='range' min='10' max='1000' value='" + String(rotationSpeed) + "' name='speed' onchange='this.form.submit()'></form>");

  // LEDs
  client.println("<h3>Disco LEDs</h3>");
  for (int i = 0; i < 4; i++) {
    client.println("<a href='/led" + String(i) + "=on'><button>LED " + String(i+1) + " ON</button></a>");
    client.println("<a href='/led" + String(i) + "=off'><button>OFF</button></a><br>");
  }
  client.println("<a href='/disco=on'><button style='background:#4caf50;color:white;'>Disco ON</button></a>");
  client.println("<a href='/disco=off'><button style='background:#f44336;color:white;'>Disco OFF</button></a><br>");

  // Distance
  client.println("<h3>Ultrasonic Distance</h3>");
  client.println("<p>Distance: <strong>" + String(distanceCM) + " cm</strong></p>");
  client.println("<a href='/getDistance'><button>Get Distance</button></a>");

  client.println("</body></html>");
  delay(1);

  Serial.println("Client Disconnected");

  // Continuous rotation
  if (rotateServo) {
    if (rotateCW) currentAngle += 5;
    else currentAngle -= 5;
    if (currentAngle > 180) currentAngle = 0;
    if (currentAngle < 0) currentAngle = 180;
    myServo.write(currentAngle);
    delay(rotationSpeed);
  }

  // Disco flashing
  if (discoMode && millis() - lastDiscoTime > 150) {
    for (int i = 0; i < 4; i++) digitalWrite(ledPins[i], LOW);
    digitalWrite(ledPins[discoIndex], HIGH);
    discoIndex = (discoIndex + 1) % 4;
    lastDiscoTime = millis();
  }
}

void measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 30000);
  distanceCM = duration * 0.034 / 2;
}
