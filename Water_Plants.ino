#include <Credentials.h> // contains WiFi credentials, could contain any secret key
#include <CustomOTA.h> // enables WiFi, sets up OTA updates
#include <AsyncTCP.h> // Base for Async Web Server
#include <ESPAsyncWebServer.h> // Creates asynchronous web servers
#include <WebSerial.h> // enables reading "Serial" output over web (acts as an emulator)
#include <Plant_Waterer.h>

#define DEVICE_HOSTNAME "PLANT_WATERER_1" // change this for every new device
#define STATIC_IP_HOST_ADDRESS 81 // change this for every new device

#define PIN_SENSOR_1 39
#define PIN_MOTOR_1 23

#define PIN_SENSOR_2 35
#define PIN_MOTOR_2 21

#define PIN_SENSOR_3 33
#define PIN_MOTOR_3 5

#define PIN_SENSOR_4 26
#define PIN_MOTOR_4 4

Plant_Waterer plant_1(PIN_SENSOR_1, PIN_MOTOR_1);
Plant_Waterer plant_2(PIN_SENSOR_2, PIN_MOTOR_2);
Plant_Waterer plant_3(PIN_SENSOR_3, PIN_MOTOR_3);
Plant_Waterer plant_4(PIN_SENSOR_4, PIN_MOTOR_4);

void setupWiFi() {
    const char *ssid = WIFI_SSID;
    const char *password = WIFI_PW;

    IPAddress staticIP(192, 168, 178, STATIC_IP_HOST_ADDRESS);
    IPAddress gateway(192, 168, 178, 1);
    IPAddress subnet(255, 255, 255, 0);
    IPAddress dns1(192, 168, 178, 13); // optional
    IPAddress dns2(1, 1, 1, 1);        // optional

    if (!WiFi.config(staticIP, gateway, subnet, dns1, dns2)) {
        Serial.println("STA Failed to configure");
    }

    setupOTA(DEVICE_HOSTNAME, WIFI_SSID, WIFI_PW);
}
AsyncWebServer server(80);
void beginOnlineServer() {
    server.begin();
}
void enableWebSerial() {
    WebSerial.begin(&server);
    WebSerial.msgCallback(processWebSerialInput);
}
void processWebSerialInput(uint8_t *data, size_t len) {
    WebSerial.println("Received Data...");
    String d = "";
    for (int i = 0; i < len; i++) {
        d += char(data[i]);
    }
    Serial.println(d);
}

void setup() {
    Serial.begin(115200);
    analogReadResolution(12);

    setupWiFi();
    enableWebSerial();
    beginOnlineServer();
}
void loop() {
    ArduinoOTA.handle();

    WebSerial.print(plant_1.get_moisture_level());
    WebSerial.print("\t");
    WebSerial.print(plant_2.get_moisture_level());
    WebSerial.print("\t");
    WebSerial.print(plant_3.get_moisture_level());
    WebSerial.print("\t");
    WebSerial.println(plant_4.get_moisture_level());

    plant_1.check_dryness_and_water();
    plant_2.check_dryness_and_water();
    plant_3.check_dryness_and_water();
    plant_4.check_dryness_and_water();

    delay(250);  // delay in between reads for clear read from serial
}
