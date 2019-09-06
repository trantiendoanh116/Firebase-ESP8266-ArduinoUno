#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>
//#include <DHT.h> // Gọi thư viện DHT22

const byte RX = 5;          // Chân 11 được dùng làm chân RX
const byte TX = 4;          // Chân 10 được dùng làm chân TX

SoftwareSerial serialEsp8266 = SoftwareSerial(RX, TX);
SerialCommand sCmd(serialEsp8266); // Khai báo biến sử dụng thư viện Serial Command

//const int DHTPIN = 12; //Đọc dữ liệu từ DHT22 ở chân D2 trên mạch Arduino
//const int DHTTYPE = DHT22; //Khai báo loại cảm biến, có 2 loại là DHT11 và DHT22
//DHT dht(DHTPIN, DHTTYPE);

//const int pinSensorCo2 = A1;

const int pinLightControl =  10; //8
const int pinLightStatus = 11; //9

//const int pinAPTControl = 2;
//const int pinAPTStatus = 3;

//const int pinFanOff = 22;
//const int pinFanOn = 23;
//const int pinFanStatusSpeed1 = 24;
//const int pinFanStatusSpeed2 = 25;
//const int pinFanStatusSpeed3 = 26;

//
int mValueLight , mValueFan, mValueApt;

//Khai báo cho các chu trình gửi dữ liệu
long timeSendValueSensor;
const long TIME_CYCLE_SEND_VALUE_SENSOR = 10000UL;

const int IS_DEBUG = 1;

void setup() {
  //Khởi tạo Serial
  Serial.begin(57600);
  serialEsp8266.begin(57600);
  // initialize the pin as an output:
  pinMode(pinLightControl, OUTPUT);
  pinMode(pinLightStatus, INPUT);

  //  pinMode(pinAPTControl, OUTPUT);
  //  pinMode(pinFanOff, OUTPUT);
  //  pinMode(pinFanOn, OUTPUT);
  //
  //  pinMode(pinLightStatus, INPUT);
  //  pinMode(pinAPTStatus, INPUT);
  //  pinMode(pinFanStatusSpeed1, INPUT);
  //  pinMode(pinFanStatusSpeed2, INPUT);
  //  pinMode(pinFanStatusSpeed3, INPUT);

  Serial.println("Da san sang nhan lenh");
  //Nhận dữ liệu từ ESP8266
  sCmd.addCommand("DATA",   receiveData);

}
long timeTest;
void loop() {
  sCmd.readSerial();
  //Gửi các giá trị cảm biến
  if (millis() - timeSendValueSensor > TIME_CYCLE_SEND_VALUE_SENSOR) {
    timeSendValueSensor = millis();
    sendValueSensor();
  }
  
  //sendValueDevice();
  delay(500);
}


void receiveData() {
  char *json = sCmd.next();
  if (IS_DEBUG) Serial.println(json);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  
  if (root.containsKey("LIGHT")) {
    int valueLight = root["LIGHT"];
      changeLight(valueLight);
  }

  if (root.containsKey("FAN")) {
    int valueFan = root["FAN"];
    if (valueFan != mValueFan) {
      mValueFan = valueFan;
      changeFan();
    }
  }

  if (root.containsKey("APT")) {
    int valueApt = root["APT"];
    if (valueApt != mValueApt) {
      mValueApt = valueApt;
      changeApt();
    }
  }
  

}

void changeLight(int valueLight) {
  if (IS_DEBUG) Serial.println("Change light");
  if (valueLight != digitalRead(pinLightStatus)) {
//    digitalWrite(pinLightControl, HIGH);
//    delay(100);
//    digitalWrite(pinLightControl, LOW);
  digitalWrite(pinLightControl, valueLight);
   sendValueDevice();
  }
}
void changeApt() {
  if (IS_DEBUG) Serial.println("Change Apt");
  if (mValueApt != mValueApt) {
//    digitalWrite(pinAPTControl, HIGH);
//    delay(100);
//    digitalWrite(pinAPTControl, LOW);
  }
}
void changeFan() {
  if (IS_DEBUG) Serial.println("Change Fan");
  if (mValueFan == 0) {
    digitalWrite(pinLightControl, LOW);
  } else {
    digitalWrite(pinLightControl, HIGH);
  }

}


void sendValueSensor() {
  StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root = jsonBuffer2.createObject();
  root["TEMP"] = random(20, 50);
  root["HUMI"] = random(60, 90);
  root["CO2"] = random(0, 100);
  //in ra cổng software serial để ESP8266 nhận
  serialEsp8266.print("DATA");   //gửi tên lệnh
  serialEsp8266.print('\r');           // gửi \r
  root.printTo(serialEsp8266); //gửi chuỗi JSON
  serialEsp8266.print('\r');           // gửi \r

  if (IS_DEBUG) {
    Serial.print("Update value sensor to esp8266: ");
    root.printTo(Serial);
    Serial.print('\n');
  }
}
void sendValueDevice() {
  bool isHaveChangeValue = false;
  int valueLight = digitalRead(pinLightStatus);
  int valueFan = readSpeedFan();
  int valueApt = mValueApt;
  //Send data to ESP8266
  StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root = jsonBuffer2.createObject();
  if (valueLight != mValueLight) {
    root["LIGHT"] = valueLight;
    isHaveChangeValue = true;
    mValueLight = valueLight;
  }
  if (valueFan != mValueFan) {
    root["FAN"] = valueFan;
    isHaveChangeValue = true;
    mValueFan = valueFan;
  }
  if (valueApt != mValueApt) {
    root["APT"] = valueApt;
    isHaveChangeValue = true;
    mValueApt = valueApt;
  }
 
  if (isHaveChangeValue) {
    serialEsp8266.print("DATA");
    serialEsp8266.print('\r');
    root.printTo(serialEsp8266);
    serialEsp8266.print('\r');
    if (IS_DEBUG) {
      Serial.print("Update value device control to Esp8266: ");
      root.printTo(Serial);
      Serial.print('\n');
    }
  }

}

int readSpeedFan() {
  return mValueFan;
}
