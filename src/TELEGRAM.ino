
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Замените на свои сетевые данные
  const char* ssid = "WIFI_IOT";
  const char* password = "1234567891";

// Инициализация Telegram бота
#define BOTtoken "5906286565:AAF71BxPYkX6sWpgz1wGTdtyKlnffROO3zE"  // Ваш Токен
#define CHAT_ID "-1001858191181"                                      // ID чата

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const int waterSensor = A0;                                       // Сенсор протечки
bool waterDetected = false;
int count = 3;                                                    // Количество сообщений
const int drop = 250;                                             // Капля

int alarmPin1 = 4; // к выводу 4 подключён геркон 1 

void setup() {
  Serial.begin(115200);
  client.setInsecure();
  
  pinMode(waterSensor, INPUT_PULLUP);            // Внутренняя подтяжка сенсора  INPUT_PULLUP
  
  pinMode(alarmPin1, INPUT_PULLUP); // задаём вывод 4 в качестве входа (будем считывать с него)
  digitalWrite(alarmPin1, HIGH); // активируем внутренний подтягивающий резистор вывода

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
  bot.sendMessage(CHAT_ID, "Бот <Охрана> стартовал", "");
}

void loop() {
  
  int gerkonValue = digitalRead(alarmPin1);                                                // Считываем значение с A0   
  if(gerkonValue == LOW){
    bot.sendMessage(CHAT_ID, "Сработка!!!", "");
    Serial.println("Сработка!!!");
    // count -= 1;                                                                             // Уменьшаем счётчик на 1  
    delay(5000);                                                                           // Пауза 5 секунд  
     
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
}
