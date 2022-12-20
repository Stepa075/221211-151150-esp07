
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
// Замените на свои сетевые данные
  const char* ssid = "WIFI_IOT";
  const char* password = "1234567891";

// Инициализация Telegram бота
#define BOTtoken "5906286565:AAF71BxPYkX6sWpgz1wGTdtyKlnffROO3zE"  // Ваш Токен
#define CHAT_ID "-1001858191181"                                      // ID чата

SoftwareSerial SerialAT(2, 3);

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// const int waterSensor = A0;                                       // Сенсор протечки
// bool waterDetected = false;
// int count = 3;                                                    // Количество сообщений
// const int drop = 250;                                             // Капля

int zone1 = 4;               // к выводу 4 подключён геркон 1 
int zone2 = 5;
int zone3 = 9;
int zone4 = 10;
int arrZones[] = {zone1, zone2, zone3, zone4};

void setup() {
  Serial.begin(115200);
  client.setInsecure();
  
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
  
  for (int i = 0; i<sizeof(arrZones)/sizeof(int); i++){
    int gerkonValue1 = digitalRead(arrZones[i]);                                                // Считываем значение с A0   
  if(gerkonValue1 == LOW){
    bot.sendMessage(CHAT_ID, "Сработка зона1!!!" + arrZones[i], "");
    Serial.println("Сработка зона1!!!" + arrZones[i]);
    // count -= 1;                                                                             // Уменьшаем счётчик на 1  
    delay(500);                                                                           // Пауза 5 секунд  
  }
  }


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




}

  
  
  
  
  
  
  
  
  
  
  
  
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

