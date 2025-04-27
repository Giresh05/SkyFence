#include <SPI.h>
#include <LoRa.h>
#include <TinyGPSPlus.h>

#define SCK     5
#define MISO    19
#define MOSI    27
#define SS      18
#define RST     14
#define DIO0    26

HardwareSerial SerialGPS(2); // use UART2
TinyGPSPlus gps;

SPIClass SPI_LoRa(VSPI);

void setup() {
  Serial.begin(115200);
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17); // GPS RX=16, TX=17

  SPI_LoRa.begin(SCK, MISO, MOSI, SS);
  LoRa.setSPI(SPI_LoRa);
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    while (1);
  }

  Serial.println("LoRa Sender + GPS ready.");
}

void loop() {
  while (SerialGPS.available()) {
    gps.encode(SerialGPS.read());
  }

  if (gps.location.isUpdated()) {
    float lat = gps.location.lat();
    float lon = gps.location.lng();

    String payload = String(lat, 6) + "," + String(lon, 6);
    Serial.println("Sending: " + payload);
    LoRa.beginPacket();
    LoRa.print(payload);
    LoRa.endPacket();
  } else {
    Serial.println("Waiting for valid GPS...");
  }

  delay(1000); // send every second
}