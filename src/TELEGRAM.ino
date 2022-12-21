
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
// Замените на свои сетевые данные
const char* ssid = "WIFI_IOT";
const char* password = "1234567891";

// Инициализация Telegram бота
#define BOTtoken "5906286565:AAF71BxPYkX6sWpgz1wGTdtyKlnffROO3zE"  // Ваш Токен
#define CHAT_ID "-1001858191181"                                      // ID чата

#define SDA = 0
#define SCL = 2
LiquidCrystal_I2C lcd(0x27,16,2);

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

SoftwareSerial GSMport(1, 3);

// String URL = "http://api.telegram.org/bot5906286565:AAF71BxPYkX6sWpgz1wGTdtyKlnffROO3zE/sendMessage?chat_id=-1001858191181&text=Hi_Eweryone!";
// const int waterSensor = A0;                                       // Сенсор протечки
// bool waterDetected = false;
// int count = 3;                                                    // Количество сообщений
// const int drop = 250;                                             // Капля

int zone1 = 4;               // к выводу 4 подключён геркон 1 
int zone2 = 5;
int zone3 = 12;
int zone4 = 14;
int arrZones[] = {zone1, zone2, zone3, zone4};
int flag = 0;
int cost_of_try = 0;
bool wifi = true;

void setup() {
               
  lcd.init(); // initialize the lcd 
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hello World!");
  Serial.begin(115200);
  client.setInsecure();
  
  GSMport.begin(9600);
  gprs_init();

  // pinMode(waterSensor, INPUT_PULLUP);            // Внутренняя подтяжка сенсора  INPUT_PULLUP
  
  pinMode(zone1, INPUT_PULLUP); // задаём выводы в качестве входа (будем считывать с него)
  digitalWrite(zone1, HIGH); // активируем внутренний подтягивающий резистор вывода
  pinMode(zone2, INPUT_PULLUP);
  digitalWrite(zone2, HIGH);
  pinMode(zone3, INPUT_PULLUP);
  digitalWrite(zone3, HIGH);
  pinMode(zone4, INPUT_PULLUP);
  digitalWrite(zone4, HIGH);

  // Attempt to connect to Wifi network:
  Serial.print("Соединение с Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // cost_of_try += 1;
    // if (cost_of_try >= 20){
    //   start_modem();
    // }
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi соединение установлено");
  Serial.print("IP адрес: ");
  Serial.println(WiFi.localIP());

  Serial.println("Бот <Охрана> стартовал");
  bot.sendMessage(CHAT_ID, "WiFi соединение установлено", "");
  bot.sendMessage(CHAT_ID, "Бот <Охрана> стартовал", "");

  
  
}




void loop() {
  int i;
  for (i = 0; i<4; i=i+1){
    int gerkonValue1 = digitalRead(arrZones[i]);                                                // Считываем значение с A0   
    if(gerkonValue1 == LOW && flag == 0){
      bot.sendMessage(CHAT_ID, "Сработка!!!" + String(arrZones[i]), "");
      Serial.println("Сработка!!!" + String(arrZones[i]));
      flag = arrZones[i];
      Serial.println(String(flag));
      gerkonValue1 = HIGH;
      delay(100);
      
    }
    else if(flag != 0){int gv = digitalRead(flag);
         if (gv == HIGH){Serial.println("Vosstanovlenie datchika " + String(flag));
         flag = 0;
         }
    }
       // gprs_send(String(arrZones[i]));
       // count -= 1;                                                                             // Уменьшаем счётчик на 1  
    delay(1000);
  }
   

}  
  
void gprs_init() {  //Процедура начальной инициализации GSM модуля
  int d = 500;
  int ATsCount = 7;
  String ATs[] = {  //массив АТ команд
    "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"",  //Установка настроек подключения
    "AT+SAPBR=3,1,\"APN\",\"internet.tele2.ru\"",
    "AT+SAPBR=3,1,\"USER\",\"\"",
    "AT+SAPBR=3,1,\"PWD\",\"\"",
    "AT+SAPBR=1,1",  //Устанавливаем GPRS соединение
    "AT+HTTPINIT",  //Инициализация http сервиса
    "AT+HTTPPARA=\"CID\",1"  //Установка CID параметра для http сессии
  };
  int ATsDelays[] = {6, 1, 1, 1, 3, 3, 1}; //массив задержек
  Serial.println("GPRS init start");
  for (int i = 0; i < ATsCount; i++) {
    Serial.println(ATs[i]);  //посылаем в монитор порта
    GSMport.println(ATs[i]);  //посылаем в GSM модуль
    delay(d * ATsDelays[i]);
    Serial.println(ReadGSM());  //показываем ответ от GSM модуля
    delay(d);
  }
  Serial.println("GPRG init complete");
}

void gprs_send(String data) {  //Процедура отправки данных на сервер
  //отправка данных на сайт
  int d = 400;
  Serial.println("Send start");
  Serial.println("setup url");
  GSMport.println("AT+HTTPPARA=\"URL\",\"http://api.telegram.org/bot" + String(BOTtoken) + "/sendMessage?chat_id=" + String(CHAT_ID) + "&text=" + data + "\"");
  delay(d * 2);
  Serial.println(ReadGSM());
  delay(d);
  Serial.println("GET url");
  GSMport.println("AT+HTTPACTION=0");
  delay(d * 2);
  Serial.println(ReadGSM());
  delay(d);
  Serial.println("Send done");
}

String ReadGSM() {  //функция чтения данных от GSM модуля
  int c;
  String v;
  while (GSMport.available()) {  //сохраняем входную строку в переменную v
    c = GSMport.read();
    v += char(c);
    delay(10);
  }
  return v;
}  
  
// void start_modem(){
//     GSMport.begin(9600);
//   gprs_init();
//   }  
  
  
  
  
  
  
  
  
  
  
  // int gerkonValue1 = digitalRead(zone1);                                                // Считываем значение с A0   
  // if(gerkonValue1 == LOW){
  //   bot.sendMessage(CHAT_ID, "Сработка зона1!!!", "");
  //   Serial.println("Сработка зона1!!!");
  //   // count -= 1;                                                                             // Уменьшаем счётчик на 1  
  //   delay(1000);                                                                           // Пауза 5 секунд  
  // }
  //  int gerkonValue2 = digitalRead(zone2);                                                // Считываем значение с A0   
  // if(gerkonValue2 == LOW){
  //   bot.sendMessage(CHAT_ID, "Сработка зона2!!!", "");
  //   Serial.println("Сработка зона2!!!");
  //   // count -= 1;                                                                             // Уменьшаем счётчик на 1  
  //   delay(1000);                                                                           // Пауза 5 секунд  
  // }
  //  int gerkonValue3 = digitalRead(zone3);                                                // Считываем значение с A0   
  // if(gerkonValue3 == LOW){
  //   bot.sendMessage(CHAT_ID, "Сработка зона3!!!", "");
  //   Serial.println("Сработка зона3!!!");
  //   // count -= 1;                                                                             // Уменьшаем счётчик на 1  
  //   delay(1000);                                                                           // Пауза 5 секунд  
  // }
  //  int gerkonValue4 = digitalRead(zone4);                                                // Считываем значение с A0   
  // if(gerkonValue4 == LOW){
  //   bot.sendMessage(CHAT_ID, "Сработка зона4!!!", "");
  //   Serial.println("Сработка зона4!!!");
  //   // count -= 1;                                                                             // Уменьшаем счётчик на 1  
  //   delay(1000);                                                                           // Пауза 5 секунд  
  // }






  
  
  
  
  
  
  
  
  
  
  
  
  // int sensorValue = analogRead(waterSensor);                                                // Считываем значение с A0   
  // if(waterDetected && count){
  //   bot.sendMessage(CHAT_ID, "Сработка!!!", "");
  //   Serial.println("Сработка!!!");
  //   count -= 1;                                                                             // Уменьшаем счётчик на 1  
  //   delay(5000);                                                                           // Пауза 5 секунд  
     
  // }
     
  // if (sensorValue > drop) {
  //   waterDetected = true;                                                                   // Состояние датчика
  // }
  // else {
  //   waterDetected = false;                                                                  // Состояние датчика
  //   count = 3;                                                                              // Количество сообщений снова 3
  // }

