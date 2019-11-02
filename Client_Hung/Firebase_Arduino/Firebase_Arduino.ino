#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>
#include <DHT.h> // Gọi thư viện DHT22
#include <RCSwitch.h>

const int DHTPIN = 13;
const int DHTTYPE = DHT22;
DHT dht(DHTPIN, DHTTYPE);

RCSwitch mySwitch = RCSwitch();
RCSwitch FanSwitch = RCSwitch();

const byte RX = 3;          // 3
const byte TX = 2;          // 2

SoftwareSerial serialEsp8266 = SoftwareSerial(RX, TX);
SerialCommand sCmd(serialEsp8266); // Khai báo biến sử dụng thư viện Serial Command

//const int fanOffPin = 4;
//const int fanOnPin = 5;
//
const int speed1Pin = 6;
const int speed2Pin = 7;
const int speed3Pin = 8;

//const int lightControlPin = 10; //9
const int lightStatusPin = 9; //11

//const int ATControlPin = 10;
const int ATStatusPin = 10;

void setup() {
  //Khởi tạo Serial
  Serial.begin(115200);
  serialEsp8266.begin(115200);
  //mySwitch.enableTransmit(11);
  FanSwitch.enableTransmit(5);
  offFan();
  dht.begin(); // Khởi động cảm biến nhiệt
  // initialize the pin as an output:
  // pinMode(fanOnPin, OUTPUT);
  // pinMode(fanOffPin, OUTPUT);
  // pinMode (ATControlPin, OUTPUT);
  //pinMode(lightControlPin, OUTPUT);

  pinMode (lightStatusPin, INPUT);
  pinMode (ATStatusPin, INPUT);
  pinMode(speed1Pin, INPUT);
  pinMode(speed2Pin, INPUT);
  pinMode(speed3Pin, INPUT);

  //digitalWrite(fanOffPin, HIGH);
  Serial.println("Da san sang nhan lenh");

  Serial.println("Da san sang nhan lenh");
  //Nhận dữ liệu từ ESP8266
  sCmd.addCommand("DATA",   receiveData);

}

//Khai báo cho các chu trình gửi dữ liệu
unsigned long chuky = 0;
const unsigned long TIME_RETRIEVE_DATA = 10000UL; //Cứ sau 2000ms = 5s thì chu kỳ lặp lại

unsigned long chuky1 = 0;
const unsigned long TIME_RETRIEVE_DATA_1 = 10000UL; //Cứ sau 2000ms = 5s thì chu kỳ lặp lại


const int IS_DEBUG = 1;

void loop() {
  //Gửi các giá trị cảm biến
//  if (millis() - chuky > TIME_RETRIEVE_DATA) {
//    chuky = millis();
//    sendData();
//  }

   if (millis() - chuky1 > TIME_RETRIEVE_DATA_1) {
    chuky1 = millis();
    sendValueDeviceControl();
  }
 


  sCmd.readSerial();
  delay(200);
  
}


void receiveData() {
  char *json = sCmd.next();
  if (IS_DEBUG) Serial.println(json);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);

  if (root.containsKey("change_light")) {
    changeLight();
  }

  if (root.containsKey("change_apt")) {
    changeApt();
  }

  if (root.containsKey("off_fan")) {
    offFan();
  }
  if (root.containsKey("on_fan")) {
    onFan();
  }


}
// *******************************************
//     Bật tắt đèn
// *******************************************

void changeLight() {
  if (IS_DEBUG) Serial.println("Change light");

  unsigned long address = 0b0101010101010101;
  FanSwitch.send((address << 8) | 0b00001100, 24);
  delay(80);
  FanSwitch.send((address << 8) | 0b00000000, 24);
  
 int valueLight = !digitalRead(lightStatusPin);
 Serial.println(valueLight);
   StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root = jsonBuffer2.createObject();
    root["light"] = valueLight;
     serialEsp8266.print("DATA");
    serialEsp8266.print('\r');
    root.printTo(serialEsp8266);
    serialEsp8266.print('\r');
    if (IS_DEBUG) {
      Serial.print("Send to Esp8266: ");
      root.printTo(Serial);
      Serial.print('\n');
    }
}
// *******************************************
//     Bật tắt Atomat
//********************************************

void changeApt() {
  if (IS_DEBUG) Serial.println("Change Apt");
  unsigned long address = 0b0101010101010101;
  FanSwitch.send((address << 8) | 0b00000011, 24);
  delay(80);
  FanSwitch.send((address << 8) | 0b00000000, 24);

   int valueApt = !digitalRead(ATStatusPin);
   StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root = jsonBuffer2.createObject();
    root["apt"] = valueApt;
     serialEsp8266.print("DATA");
    serialEsp8266.print('\r');
    root.printTo(serialEsp8266);
    serialEsp8266.print('\r');
    if (IS_DEBUG) {
      Serial.print("Send to Esp8266: ");
      root.printTo(Serial);
      Serial.print('\n');
    }

}
// **************************************************
//     Tắt quạt trần
// **************************************************

void offFan() {
  if (IS_DEBUG) Serial.println("Off Fan");
  unsigned long address = 0b0101010101010100;
  FanSwitch.send((address << 8) | 0b00001100, 24);
  delay(80);
  FanSwitch.send((address << 8) | 0b00000000, 24);

  int valueApt = readValueFan();
   StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root = jsonBuffer2.createObject();
    root["fan"] = valueApt;
     serialEsp8266.print("DATA");
    serialEsp8266.print('\r');
    root.printTo(serialEsp8266);
    serialEsp8266.print('\r');
    if (IS_DEBUG) {
      Serial.print("Send to Esp8266: ");
      root.printTo(Serial);
      Serial.print('\n');
    }
}
//****************************************************
//      Bật quạt trần
// ***************************************************

void onFan() {
  if (IS_DEBUG) Serial.println("On Fan");

  unsigned long address = 0b0101010101010100;
  FanSwitch.send((address << 8) | 0b00000011, 24);
  delay(80);
  FanSwitch.send((address << 8) | 0b00000000, 24);

   int valueApt = readValueFan();
   StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root = jsonBuffer2.createObject();
    root["fan"] = valueApt;
     serialEsp8266.print("DATA");
    serialEsp8266.print('\r');
    root.printTo(serialEsp8266);
    serialEsp8266.print('\r');
    if (IS_DEBUG) {
      Serial.print("Send to Esp8266: ");
      root.printTo(Serial);
      Serial.print('\n');
    }

}


void sendData() {
  float h = dht.readHumidity(); //Đọc độ ẩm
  float t = dht.readTemperature(); //Đọc nhiệt độ
  Serial.println("temp: ");

  int valueLight = !digitalRead(lightStatusPin);
  int valueFan = readValueFan();
  int valueApt = !digitalRead(ATStatusPin);

  StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root = jsonBuffer2.createObject();
  root["temp"] = t;
  root["humi"] = h;
  root["co2"] = random(0, 100);
//  root["light"] = valueLight;
//  root["fan"] = valueFan;
//  root["apt"] = valueApt;

  //in ra cổng software serial để ESP8266 nhận
  serialEsp8266.print("DATA");   //gửi tên lệnh
  serialEsp8266.print('\r');           // gửi \r
  root.printTo(serialEsp8266); //gửi chuỗi JSON
  serialEsp8266.print('\r');           // gửi \r

  if (IS_DEBUG) {
    Serial.print("Send to esp8266: ");
    root.printTo(Serial);
    Serial.print('\n');
  }
}

void sendValueDeviceControl() {

  int valueLight = !digitalRead(lightStatusPin);
  int valueFan = readValueFan();
  int valueApt = !digitalRead(ATStatusPin);

  StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root = jsonBuffer2.createObject();
  root["light"] = valueLight;
  root["fan"] = valueFan;
  root["apt"] = valueApt;

  //in ra cổng software serial để ESP8266 nhận
  serialEsp8266.print("DATA");   //gửi tên lệnh
  serialEsp8266.print('\r');           // gửi \r
  root.printTo(serialEsp8266); //gửi chuỗi JSON
  serialEsp8266.print('\r');           // gửi \r

  if (IS_DEBUG) {
    Serial.print("Send to esp8266: ");
    root.printTo(Serial);
    Serial.print('\n');
  }
}



int readValueFan() {
  int speed1Status = !digitalRead(speed1Pin);
  int speed2Status = !digitalRead(speed2Pin);
  int speed3Status = !digitalRead(speed3Pin);
  if (speed1Status == 0 and speed2Status == 0 and speed3Status == 0) {
    return 0;
  }
  if (speed1Status == 1) {
    return 3;
  }
  if (speed2Status == 1) {
    return 2;
  }
  if (speed3Status == 1) {
    return 1;
  }
}

