#include <WiFi.h>
#include <ESP32Servo.h>

// Replace with your Wi-Fi credentials
const char* ssid = "Saif's S24 Ultra";
const char* password = "sidku8090";

// Create server on port 80
WiFiServer server(80);
Servo myServo;

// Define pins
const int servoPin = 13;
const int led1 = 12;
const int led2 = 14;
const int led3 = 27;
const int led4 = 26;

void setup() {
  Serial.begin(115200);

  // Setup LED pins
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);

  // Setup servo
  myServo.setPeriodHertz(50);
  myServo.attach(servoPin, 500, 2400);
  myServo.write(90); // center position

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi connected!");
  Serial.print("🔗 ESP32 IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  String request = client.readStringUntil('\r');
  client.flush();
  Serial.println("📥 Request: " + request);

  // --- Servo Control ---
  if (request.indexOf("/servo=0") != -1) myServo.write(0);
  if (request.indexOf("/servo=90") != -1) myServo.write(90);
  if (request.indexOf("/servo=180") != -1) myServo.write(180);

  // --- LED Control ---
  digitalWrite(led1, request.indexOf("/led1=on") != -1);
  digitalWrite(led2, request.indexOf("/led2=on") != -1);
  digitalWrite(led3, request.indexOf("/led3=on") != -1);
  digitalWrite(led4, request.indexOf("/led4=on") != -1);

  // --- Web Page ---
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html\n");

  client.println("<html><head><title>ESP32 Disco Control</title>");
  client.println("<style>");
  client.println("body{font-family:sans-serif;text-align:center;padding:20px;}");
  client.println("button{padding:15px 25px;margin:10px;font-size:18px;}");
  client.println("</style></head><body>");

  client.println("<h2>💡 ESP32 Disco LEDs + Servo</h2>");

  // Servo buttons
  client.println("<h3>Servo Control</h3>");
  client.println("<a href=\"/servo=0\"><button>Rotate 0°</button></a>");
  client.println("<a href=\"/servo=90\"><button>Rotate 90°</button></a>");
  client.println("<a href=\"/servo=180\"><button>Rotate 180°</button></a>");

  // LED buttons
  client.println("<h3>LED Control</h3>");
  client.println("<a href=\"/led1=on\"><button style='background:#ff4444;color:white;'>LED 1</button></a>");
  client.println("<a href=\"/led2=on\"><button style='background:#44ff44;color:white;'>LED 2</button></a>");
  client.println("<a href=\"/led3=on\"><button style='background:#4444ff;color:white;'>LED 3</button></a>");
  client.println("<a href=\"/led4=on\"><button style='background:#ffff44;color:black;'>LED 4</button></a>");

  client.println("<p>Tap buttons multiple times to flash manually like disco!</p>");
  client.println("</body></html>");

  delay(1);
  Serial.println("✅ Client disconnected.\n");
}
