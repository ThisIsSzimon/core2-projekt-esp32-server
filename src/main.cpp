#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

// ====== WiFi ======
const char* ssid     = "Szymon Komputer itaka";  // <-- TWOJ SSID
const char* password = "K)r43606";               // <-- TWOJE HASLO

// ====== LED na ESP32 ======
const int espLedPin = 2;
bool espLedState = false;

// ====== WebServer ======
WebServer server(80);

// ====== UART do CORE2 (mostek) ======
HardwareSerial core2Serial(2);  // UART2: RX=16, TX=17

// =================== HANDLERY HTTP ===================

// / -> index.html
void handleIndex() {
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    server.send(500, "text/plain", "Brak pliku /index.html w SPIFFS");
    return;
  }
  server.streamFile(file, "text/html");
  file.close();
}

// /style.css
void handleStyle() {
  File file = SPIFFS.open("/style.css", "r");
  if (!file) {
    server.send(404, "text/plain", "Brak pliku /style.css");
    return;
  }
  server.streamFile(file, "text/css");
  file.close();
}

// /script.js
void handleScript() {
  File file = SPIFFS.open("/script.js", "r");
  if (!file) {
    server.send(404, "text/plain", "Brak pliku /script.js");
    return;
  }
  server.streamFile(file, "application/javascript");
  file.close();
}

// /cmd?c=X  -> wysyła znak X na UART do CORE2 + toggluje LED na ESP
void handleCmd() {
  if (!server.hasArg("c")) {
    server.send(400, "text/plain", "missing c");
    return;
  }

  String cStr = server.arg("c");
  if (cStr.length() == 0) {
    server.send(400, "text/plain", "empty c");
    return;
  }

  char cmd = cStr[0];

  // Debug na USB
  Serial.print("CMD from HTTP: ");
  Serial.println(cmd);

  // Wyslij znak na CORE2 (na razie nie ma gadać, ale już działa)
  core2Serial.write((uint8_t)cmd);

  // Dla testu: toggle LED na ESP
  espLedState = !espLedState;
  digitalWrite(espLedPin, espLedState ? HIGH : LOW);

  // Odpowiedz na żądanie
  server.send(200, "text/plain", "ok");
}

// =================== SETUP & LOOP ===================

void setup() {
  // USB do PC
  Serial.begin(115200);
  delay(500);

  // UART do CORE2: RX=16, TX=17
  core2Serial.begin(115200, SERIAL_8N1, 16, 17);

  Serial.println();
  Serial.println("ESP32 webserver + UART to CORE2");

  // LED na ESP
  pinMode(espLedPin, OUTPUT);
  digitalWrite(espLedPin, LOW);
  espLedState = false;

  // SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Blad montowania SPIFFS!");
  } else {
    Serial.println("SPIFFS OK.");
  }

  // WiFi
  Serial.print("Laczenie z WiFi: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Polaczono z WiFi!");
  Serial.print("IP ESP32: ");
  Serial.println(WiFi.localIP());

  // Routing HTTP
  server.on("/", HTTP_GET, handleIndex);
  server.on("/index.html", HTTP_GET, handleIndex);   // na wszelki wypadek
  server.on("/style.css", HTTP_GET, handleStyle);
  server.on("/script.js", HTTP_GET, handleScript);
  server.on("/cmd", HTTP_GET, handleCmd);

  server.begin();
  Serial.println("Serwer HTTP wystartowal");
}

void loop() {
  server.handleClient();

  // (opcjonalnie) mostek USB <-> CORE2, jak chcesz mieć podgląd
  while (Serial.available() > 0) {
    int c = Serial.read();
    core2Serial.write(c);
  }

  while (core2Serial.available() > 0) {
    int c = core2Serial.read();
    Serial.write(c);
  }
}
