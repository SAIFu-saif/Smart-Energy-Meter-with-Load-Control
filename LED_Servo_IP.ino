#include <WiFi.h>
#include <ESP32Servo.h>

// 🔁 Replace with your Wi-Fi credentials
const char* ssid = "Saif's S24 Ultra";
const char* password = "sidku8090";

WiFiServer server(80);           // HTTP server on port 80
Servo myServo;

const int servoPin = 13;         // Servo signal pin
const int ledPin = 12;           // LED pin

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);       // Set LED pin as output
  digitalWrite(ledPin, LOW);     // Start with LED OFF

  myServo.setPeriodHertz(50);    // Set 50Hz PWM for servo
  myServo.attach(servoPin, 500, 2400); // Set servo min/max pulse width
  myServo.write(90);             // Center position

  WiFi.begin(ssid, password);    // Connect to Wi-Fi
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi connected!");
  Serial.print("🔗 ESP32 IP address: ");
  Serial.println(WiFi.localIP()); // Shows IP to access in browser

  server.begin(); // Start server
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  String request = client.readStringUntil('\r');
  client.flush();
  Serial.println("📥 Request: " + request);

  // --- Control Servo ---
  if (request.indexOf("/servo=0") != -1) myServo.write(0);
  if (request.indexOf("/servo=90") != -1) myServo.write(90);
  if (request.indexOf("/servo=180") != -1) myServo.write(180);

  // --- Control LED ---
  if (request.indexOf("/led=on") != -1) digitalWrite(ledPin, HIGH);
  if (request.indexOf("/led=off") != -1) digitalWrite(ledPin, LOW);

  // --- HTML Web Interface ---
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  client.println("<html><head><title>ESP32 Control</title>");
  client.println("<style>");
  client.println("body{font-family:sans-serif;text-align:center;padding:20px;}");
  client.println("button{padding:15px 30px;font-size:18px;margin:10px;}");
  client.println("</style></head><body>");

  client.println("<h2>🔧 ESP32 Servo + LED Control</h2>");

  client.println("<h3>Servo Control</h3>");
  client.println("<a href=\"/servo=0\"><button>Rotate 0°</button></a>");
  client.println("<a href=\"/servo=90\"><button>Rotate 90°</button></a>");
  client.println("<a href=\"/servo=180\"><button>Rotate 180°</button></a>");

  client.println("<h3>LED Control</h3>");
  client.println("<a href=\"/led=on\"><button style='background:green;color:white;'>LED ON</button></a>");
  client.println("<a href=\"/led=off\"><button style='background:red;color:white;'>LED OFF</button></a>");

  client.println("</body></html>");

  delay(1);
  Serial.println("✅ Client disconnected.\n");
}
