#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <FirebaseArduino.h>
#include <SerialCommand.h>


#define FIREBASE_HOST "smarthome-116.firebaseio.com"
#define FIREBASE_AUTH "FlPp0B4gmG8cPQkHjI9kdO9RBnajTKrNoXMuB7xH"
const String FIREBASE_PATH_ACTION = "/action";
const String FIREBASE_PATH_VALUE  = "/values";

const int DEBUG = 1;

const byte RX = 5;
const byte TX = 4;
SoftwareSerial serialArduino(RX, TX, false);
SerialCommand sCmd(serialArduino);

//giá trị của các thiết bị

void setup() {
  Serial.begin(57600);
  serialArduino.begin(57600);

  //Kết nối wifi
  connectWifi();

  // Khởi tạo Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.stream(FIREBASE_PATH_ACTION);
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
  delay(20);

}
void sendDataToArduino() {
 String pathChangeLight =  "/change_light";
  String pathChangeApt =  "/change_apt";
  String pathOffFan =   "/off_fan";
  String pathOnFan =   "/on_fan";

  if (Firebase.available()) {
    FirebaseObject event = Firebase.readEvent();
    String eventType = event.getString("type");
    String path = event.getString("path");
    bool value = event.getBool("data");

//    Serial.print("event: ");
//    Serial.println(eventType);
//    Serial.print("path: ");
//    Serial.println(path);
//    Serial.println(Firebase.getBool(PATH_ACTION_FIREBASE + path));

    bool isHaveDataChange = false;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    if (path == pathChangeLight && value == true) {
      root["change_light"] = true;
      isHaveDataChange = true;
    }
    if (path == pathChangeApt && value == true) {
      root["change_apt"] = true;
      isHaveDataChange = true;
     
    }
    if (path == pathOffFan && value == true) {
      root["off_fan"] = true;
      isHaveDataChange = true;
      
    }
    if (path == pathOnFan && value == true) {
      root["on_fan"] = true;
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

}

void receiveData(String command)
{
  char *json = sCmd.next();
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);

  if (root.containsKey("light")) {
    Firebase.setInt(FIREBASE_PATH_VALUE + "/light", root["light"]);
  }
  if (root.containsKey("apt")) {
    Firebase.setInt(FIREBASE_PATH_VALUE + "/apt", root["apt"]);
  }
  if (root.containsKey("fan")) {
    Firebase.setInt(FIREBASE_PATH_VALUE + "/fan", root["fan"]);
  }

  if (root.containsKey("temp")) {
    Firebase.setFloat(FIREBASE_PATH_VALUE + "/temp", root["temp"]);
  }

  if (root.containsKey("humi")) {
    Firebase.setFloat(FIREBASE_PATH_VALUE + "/humi", root["humi"]);
  }
  if (root.containsKey("co2")) {
    Firebase.setFloat(FIREBASE_PATH_VALUE + "/co2", root["co2"]);
  }

  if (DEBUG) {
    Serial.print("Arduino sent: ");
    Serial.print(' ');
    root.printTo(Serial);
    Serial.println();
  }
}
