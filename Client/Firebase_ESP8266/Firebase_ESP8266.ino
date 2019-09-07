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
  delay(200);

}
void sendDataToArduino() {
  bool isHaveDataChange = false;
  bool ischangeLight = Firebase.getBool("/action/change_light");
  bool ischangeApt = Firebase.getBool("/action/change_apt");
  bool isOffFan = Firebase.getBool("/action/off_fan");
  bool isOnFan = Firebase.getBool("/action/on_fan");
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  if (ischangeLight) {
    root["change_light"] = true;
    isHaveDataChange = true;
    Firebase.setBool("/action/change_light", false);
  }
  if (ischangeApt) {
    root["change_apt"] = true;
    isHaveDataChange = true;
     Firebase.setBool("/action/change_apt", false);
  }
  if (isOffFan) {
    root["off_fan"] = true;
    isHaveDataChange = true;
     Firebase.setBool("/action/off_fan", false);
  }
   if (isOnFan) {
    root["on_fan"] = true;
    isHaveDataChange = true;
     Firebase.setBool("/action/on_fan", false);
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

  if (root.containsKey("light")) {
    Firebase.setInt("/values/light", root["light"]);
  }
  if (root.containsKey("apt")) {
    Firebase.setInt("/values/apt", root["apt"]);
  }
  if (root.containsKey("fan")) {
    Firebase.setInt("/values/fan", root["fan"]);
  }

  if (root.containsKey("temp")) {
    Firebase.setFloat("/values/temp", root["temp"]);
  }

  if (root.containsKey("humi")) {
    Firebase.setFloat("/values/humi", root["humi"]);
  }
  if (root.containsKey("co2")) {
    Firebase.setFloat("/values/co2", root["co2"]);
  }

  if (DEBUG) {
    Serial.print("Arduino sent: ");
    Serial.print(' ');
    root.printTo(Serial);
    Serial.println();
  }
}
