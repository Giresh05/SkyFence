#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <LoRa.h>

// 🔼 Web page HTML (keep this at the top)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>LoRa GPS Tracker</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://unpkg.com/leaflet@1.7.1/dist/leaflet.css" />
  <style>
    body { margin:0; font-family:Arial; }
    #map { height: 90vh; }
    #status { padding: 10px; font-size: 1.2em; }
  </style>
</head>
<body>
  <div id="status">Connecting...</div>
  <div id="map"></div>

  <script src="https://unpkg.com/leaflet@1.7.1/dist/leaflet.js"></script>
  <script>
    var map = L.map('map').setView([12.8297, 80.0473], 16);
    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png').addTo(map);

    var marker = L.marker([12.8297, 80.0473]).addTo(map);
    var fence = L.circle([12.8297, 80.0473], {radius: 2000, color: 'green'}).addTo(map);

    var statusDiv = document.getElementById("status");

    if (!!window.EventSource) {
      var source = new EventSource('/events');
      source.addEventListener('gps', function(e) {
        var coords = e.data.split(',');
        var lat = parseFloat(coords[0]);
        var lon = parseFloat(coords[1]);
        marker.setLatLng([lat, lon]);
        map.setView([lat, lon]);
      }, false);

      source.addEventListener('fence', function(e) {
        var state = e.data;
        if (state === "INSIDE") {
          statusDiv.innerHTML = "🟢 Inside Geofence";
          fence.setStyle({ color: 'green' });
        } else {
          statusDiv.innerHTML = "🔴 Outside Geofence";
          fence.setStyle({ color: 'red' });
        }
      }, false);
    }
  </script>
</body>
</html>
)rawliteral";

// Replace with your WiFi credentials
const char* ssid = "realme GT NEO 3 150W";
const char* password = "jack@1234";

// LoRa pins for ESP32
#define SCK     5
#define MISO    19
#define MOSI    27
#define SS      18
#define RST     14
#define DIO0    26

SPIClass SPI_LoRa(VSPI);

// Geo-fence center
float centerLat = 12.8292;
float centerLon = 80.0482;
float radiusMeters = 2000.0;

// Globals for tracking GPS data
String lastGPS = "0,0";
bool insideFence = false;

AsyncWebServer server(80);
AsyncEventSource events("/events");

// ✅ Haversine formula to calculate distance (in meters)
float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
  const float R = 6371000; // Earth radius in meters
  float dLat = radians(lat2 - lat1);
  float dLon = radians(lon2 - lon1);
  float a = sin(dLat / 2) * sin(dLat / 2) +
            cos(radians(lat1)) * cos(radians(lat2)) *
            sin(dLon / 2) * sin(dLon / 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return R * c;
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP address: " + WiFi.localIP().toString());

  // Start LoRa
  SPI_LoRa.begin(SCK, MISO, MOSI, SS);
  LoRa.setSPI(SPI_LoRa);
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    while (true);
  }
  Serial.println("LoRa init successful.");

  // Setup web server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  events.onConnect([](AsyncEventSourceClient *client){
    client->send("connected", NULL, millis(), 1000);
  });
  server.addHandler(&events);

  server.begin();
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received = "";
    while (LoRa.available()) {
      received += (char)LoRa.read();
    }

    Serial.println("Received: " + received);
    lastGPS = received;

    // Parse lat/lon
    int comma = received.indexOf(',');
    if (comma > 0) {
      float lat = received.substring(0, comma).toFloat();
      float lon = received.substring(comma + 1).toFloat();

      float distance = calculateDistance(lat, lon, centerLat, centerLon);
      insideFence = (distance <= radiusMeters);
    }

    // Push update to browser
    events.send(lastGPS.c_str(), "gps");
    events.send(insideFence ? "INSIDE" : "OUTSIDE", "fence");
  }
}
