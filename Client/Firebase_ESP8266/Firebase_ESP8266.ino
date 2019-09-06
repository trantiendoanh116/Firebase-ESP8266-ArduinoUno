#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <FirebaseArduino.h>
#include <SerialCommand.h>


#define FIREBASE_HOST "smarthome-116.firebaseio.com"
#define FIREBASE_AUTH "rWeJcZn1qQERia7vGaU1i89thiHnPAdjTY6xFJix"

const int DEBUG = 1;

const byte RX = 5;
const byte TX = 4;
SoftwareSerial serialArduino(RX, TX, false, 256);
SerialCommand sCmd(serialArduino);

//giá trị của các thiết bị
int mValueLight = -1, mValueFan = -1, mValueApt = -1;

void setup() {
  Serial.begin(57600);
  serialArduino.begin(57600);

  //Kết nối wifi
  connectWifi();

  // Khởi tạo Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  // mValueLight = Firebase.getInt("LIGHT");
  // mValueFan = Firebase.getInt("FAN");
  // mValueApt = Firebase.getInt("APT");
  // Nhận dữ liệu từ Arduino gửi lên
  sCmd.addDefaultHandler(receiveData);

}
void connectWifi() {
  Serial.print("Wifi connecting");
  WiFiManager wifiManager;
  //wifiManager.resetSettings(); //Reset setup wifiManager
  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect()) {
    Serial.println(F("Ket noi den wifi that bai!"));
    //Nếu kết nối thất bại, thử kết nối lại bằng cách reset thiết bị
    ESP.reset();
    delay(1000);
  }
  Serial.println();
  Serial.println("Wifi connected: ");
}
void configModeCallback (WiFiManager *myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void loop() {

  sendDataToArduino();

  sCmd.readSerial();
  delay(500);

}
void sendDataToArduino() {
  bool isHaveDataChange = false;
  int valueLight = Firebase.getInt("LIGHT");
  int valueFan = Firebase.getInt("FAN");
  int valueApt = Firebase.getInt("APT");
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  if (mValueLight != valueLight) {
    mValueLight = valueLight;
    root["LIGHT"] = valueLight;
    isHaveDataChange = true;
  }
  if (mValueFan != valueFan) {
    mValueFan = valueFan;
    root["FAN"] = valueFan;
    isHaveDataChange = true;
  }
  if (mValueApt != valueApt) {
    mValueApt = valueApt;
    root["APT"] = valueApt;
    isHaveDataChange = true;
  }
  if (isHaveDataChange) {
    serialArduino.print("DATA");
    serialArduino.print('\r');
    root.printTo(serialArduino);
    serialArduino.print('\r');
    //print for debug
    if (DEBUG) {
      Serial.print("Send to Arduino: ");
      Serial.print(' ');
      root.printTo(Serial);
      Serial.println("");

    }
  }

}

void receiveData(String command)
{
  char *json = sCmd.next();
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);

  if (root.containsKey("LIGHT")) {
    Firebase.setInt("LIGHT", root["LIGHT"]);
  }
  if (root.containsKey("APT")) {
    Firebase.setInt("APT", root["APT"]);
  }
  if (root.containsKey("FAN")) {
    Firebase.setInt("FAN", root["FAN"]);
  }

  if (root.containsKey("TEMP")) {
    Firebase.setFloat("TEMP", root["TEMP"]);
  }

  if (root.containsKey("HUMI")) {
    Firebase.setFloat("HUMI", root["HUMI"]);
  }
  if (root.containsKey("CO2")) {
    Firebase.setFloat("CO2", root["CO2"]);
  }

  if (DEBUG) {
    Serial.print("Arduino sent: ");
    Serial.print(' ');
    root.printTo(Serial);
    Serial.println();
  }
}
