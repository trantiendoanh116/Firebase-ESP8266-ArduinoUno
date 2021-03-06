#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <FirebaseArduino.h>
#include <SerialCommand.h>

#define WIFI_SSID "Hung tang 3"
#define WIFI_PASSWORD "0966606670"

#define FIREBASE_HOST "smarthome-116.firebaseio.com"
#define FIREBASE_AUTH "XEuLN4OrBXM9aJgLM84oYC5Ygpxpms47vcfmPbGf"

const int DEBUG = 1;

const byte RX = 5;
const byte TX = 4;
SoftwareSerial serialArduino(RX, TX, false, 256);
SerialCommand sCmd(serialArduino);

//giá trị của các thiết bị
String PATH_ACTION_FIREBASE = "/action";
String  PATH_VALUES_FIREBASE = "/values";
void setup() {
  Serial.begin(57600);
  serialArduino.begin(57600);

  //Kết nối wifi
 connectWifi();
//wifiConnect();

  // Khởi tạo Firebase
    Firebase.begin("smarthome-116.firebaseio.com", "FlPp0B4gmG8cPQkHjI9kdO9RBnajTKrNoXMuB7xH");
  // Firebase.stream("/dev/action");
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

}

void sendDataToArduino() {
  String pathChangeLight = PATH_ACTION_FIREBASE + "/change_light";
  String pathChangeApt = PATH_ACTION_FIREBASE + "/change_apt";
  String pathOffFan =  PATH_ACTION_FIREBASE + "/off_fan";
  String pathOnFan =  PATH_ACTION_FIREBASE + "/on_fan";

    bool isHaveDataChange = false;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    if (Firebase.getBool(pathChangeLight)) {
      root["change_light"] = true;
      isHaveDataChange = true;
    }
    if (Firebase.getBool(pathChangeApt)) {
      root["change_apt"] = true;
      isHaveDataChange = true;
     // Firebase.setBool(pathChangeApt, false);
    }
    if (Firebase.getBool(pathOffFan)) {
      root["off_fan"] = true;
      isHaveDataChange = true;
      //Firebase.setBool(pathOffFan, false);
    }
    if (Firebase.getBool(pathOnFan)) {
      root["on_fan"] = true;
      isHaveDataChange = true;
      //Firebase.setBool(pathOnFan, false);
    }
    if (isHaveDataChange) {
      serialArduino.print("data");
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
    Firebase.setInt(PATH_VALUES_FIREBASE + "/light", root["light"]);
  }
  if (root.containsKey("apt")) {
    Firebase.setInt(PATH_VALUES_FIREBASE + "/apt", root["apt"]);
  }
  if (root.containsKey("fan")) {
    Firebase.setInt(PATH_VALUES_FIREBASE + "/fan", root["fan"]);
  }

  if (root.containsKey("temp")) {
    Firebase.setFloat(PATH_VALUES_FIREBASE + "/temp", root["temp"]);
  }

  if (root.containsKey("humi")) {
    Firebase.setFloat(PATH_VALUES_FIREBASE + "/humi", root["humi"]);
  }
  if (root.containsKey("co2")) {
    Firebase.setFloat(PATH_VALUES_FIREBASE + "/co2", root["co2"]);
  }

  if (DEBUG) {
    Serial.print("Arduino sent: ");
    Serial.print(' ');
    root.printTo(Serial);
    Serial.println();
  }
}
