/* Дмитрий Осипов. http://www.youtube.com/user/d36073?feature=watch
 
 v.01 Arduino NFC EEPROM электронный ключ RC522 Card Read Module RFID OLED LCD Display
 Version 0.1 2014/09/01
 ====================================
 Каждый электронный NFC (билет / карта / проездной / брелок), имеет свой уникальный номер "UID",
 на основе этой уникальности, организуем систему управления, например контроль доступа.
 
 Инструкция:
 Сначала записываем "вручную" в sketch № "Card UID" карты администратора, 
 для запуска режима записи в EEPROM (энергонезависимую память)"Card UID" пользователей, максимум 50 карт.
 В "режиме управления", поднеся к "Card Read" карту администратора, запускаем "режим записи" "Card UID" пользователей в 50 ячеек 
 памяти, если нужно пропустить какую-нибудь ячейку, в "режиме записи" поднеся карту администратора, перескочим к следующей ячейке.
 Для выхода из режима записи, надо пройти от 0 до 49 ячейки, или сделать перезагрузку.
 Все записанные "Card UID", сохраняются даже после отключения питания.
 
 В "режиме управления" подносим к "Card Read" записанную в память / EEPROM карту пользователя,
 в зависимости от "Card UID" поднесённой карты, выполняем любые действия которые сами пропишем в sketch.
 
 Параллельно выводим на дисплей информацию о всех действиях, и озвучиваем через Piezo / динамик.
 Иногда, могут возникать проблемы у библиотеки Serial, но она итак в коде не используется.
 *********************
 
 Что нам понадобится:
 
 1) Arduino.
 
 2) RC522 Card Read Module
 Mifare RC522 Card Read Module Tags SPI Interface Read and Write.
 http://www.ebay.com/itm/400447291624?ssPageName=STRK:MEWNX:IT&_trksid=p3984.m1439.l2649
 — Поддерживаемые типы карт: MIFARE S50, MIFARE S70, 
 MIFARE UltraLight, MIFARE Pro, MIFARE DESfire.
 ! Напряжение / питание: 3.3 v.
 
 
 3) Дисплей OLED LCD Display I2C 0.96 IIC Serial 128X64 
 http://www.ebay.com/itm/251550002026?ssPageName=STRK:MEWNX:IT&_trksid=p3984.m1439.l2649
 
 4) Piezo / Динамик.
 
 **********************************
 
 Скачать sketch.
 v.01 Arduino NFC EEPROM электронный ключ RC522 Card Read Module RFID OLED LCD Display
 
 ----------------------------
 Подробную видео инструкцию выложу здесь.
 v.01 Arduino NFC EEPROM электронный ключ RC522 Card Read Module RFID OLED LCD Display
 
 ============================================
 
 
 *************************************************************** 
 Вспомогательные видео инструкции, sketch, программы, библиотеки.
 ***************************************************************
 
 
 v.02 Arduino NFC билет Метро электронный ключ RC522 Card Read Module RFID
 Скачать sketch.  https://yadi.sk/d/zJpYsmLcTkaYV
 
 видео v.02 Arduino NFC билет Метро электронный ключ RC522 Card Read Module RFID 
 http://www.youtube.com/watch?v=TF8oMHsSfWU
 
 ----------------------------
 
 v.01 Arduino Метро Единый билет RC522 Card Read Module RFID.txt
 Скачать sketch. https://yadi.sk/d/L1YHkmcuSjdwy
 
 видео: Arduino Метро Единый билет RC522 Card Read Module RFID NFC 
 http://www.youtube.com/watch?v=z6-q_BS9LmQ&list=UU7aH7HVqDvwB1xNHfSl-fDw
 
 ----------------------------
 
 Самый лучший Arduino дисплей OLED LCD Display I2C 0.96 IIC Serial 128X64
 видео: http://www.youtube.com/watch?v=niA3aPu3-dQ&list=UU7aH7HVqDvwB1xNHfSl-fDw
 ============================================
 
 
 */




#include <EEPROM.h> // Подключаем библиотеку для записи в энергонезависимую память "Card UID" / уникальный номер NFC карты.
#include <SPI.h>
#include <Wire.h> // Подключаем библиотеку для шины I2C / для работы дисплея.
#include <LiquidCrystal_I2C.h>
// ---------------------------------
// Скачать библиотеки для дисплея. https://yadi.sk/d/9F_uW1wIZUDna
// #include <Adafruit_GFX.h> // Скачанная библиотека для дисплея.
// #include <Adafruit_SSD1306.h> // Скачанная библиотека для дисплея.
// ---------------------------------

// Библиотека "RFID", для RC522 Card Read. Скачать http://yadi.sk/d/6XLMLCuxSjdGa
#include <MFRC522.h> // скачанная библиотека "RFID" для RC522 Card Read Module.
// ****************************************

// Для дисплея.
// Дисплей подключаем по 2 проводам, на Arduino Uno (A4 = SDA) и (A5 = SCL).  
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
// --------------------------------------

/*
Для RC522 Card Read Module. подключение для Arduino Uno и Mega, производится к разным Pin!
 ----------------------------------------------------- Nicola Coppola
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno      Arduino Mega      MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                5                 RST
 * SPI SS     10               53                SDA
 * SPI MOSI   11               51                MOSI
 * SPI MISO   12               50                MISO
 * SPI SCK    13               52                SCK
 
 */

// Два Pin (SS и RST) допускают произвольное подключение и конфигурируются в коде.
// !(SS - он же - SDA).
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Объект MFRC522C / reate MFRC522 instance.
unsigned long uidDec, uidDecTemp;  // Для отображения номера карточки в десятичном формате.
// **************************************************************

// Массив для хранения 50 "Card UID" / уникальных номеров NFC карт, считанных из EEPROM. 
unsigned long CardUIDeEPROMread[] = {
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,
  30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49};

int ARRAYindexUIDcard; // Индекс для массива CardUIDeEPROMread.

int EEPROMstartAddr; // Стартовая ячейка памяти для записи / чтения EEPROM "Card UID".
// -----------------------------

int LockSwitch; // Замок / Переключатель / Блокиратор.

int PiezoPin = 14; // Piezo / Динамик.

void setup(){   
  // Serial.begin(9600); 

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C (for the  0.96" 128X64 OLED LCD Display)I2C АДРЕС.

  SPI.begin(); //  инициализация SPI / Init SPI bus.
  mfrc522.PCD_Init(); // инициализация MFRC522 / Init MFRC522 card.

  pinMode(PiezoPin, OUTPUT);
  digitalWrite(PiezoPin, HIGH),delay(100),digitalWrite(PiezoPin, LOW); // Подадим писк при запуске.

  DisplayWAiT_CARD(); // Запускаем функцию, заставка дисплея.

  EEPROMreadUIDcard(); // Запускаем функцию, для перезаписи массива CardUIDeEPROMread, данными из EEPROM.

  // Если хотите увидеть в Serial мониторе все записанные № Card-UID, раскомментируйте строку.
  // for(int i = 0; i <= 49; i++)Serial.print(i),Serial.print(" ---- "),Serial.println(CardUIDeEPROMread[i]);

  //  for (int i = 0; i < 512; i++)EEPROM.write(i, 0); // EEPROM Clear / Очистить / стереть EEPROM. 
}

void loop() {

  // Поиск новой NFC карты / Look for new cards.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Выбор карточки / Select one of the cards.
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Выдача серийного номера карточки "UID".
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    uidDecTemp = mfrc522.uid.uidByte[i];
    uidDec = uidDec*256+uidDecTemp;  
  }  
  // ----------------------------------------

  // Каждый раз подготавливаем дисплей для записи. 
  display.clearDisplay(); // Clear the buffer. / Очистить буфер дисплея.
  display.setTextColor(WHITE); // Цвет текста.
  display.setTextSize(2); // Размер текста (2).
  display.setCursor(0,0); // Устанавливаем курсор в колонку / Pixel 0, строку / Pixel 0. 

  // ----------------------------------------

  // uidDec - Это полученный "Card UID" / уникальный номер NFC карты.

  // Записываем "вручную" в sketch № "Card UID" карты администратора, для запуска "режима записи" в EEPROM "Card UID" пользователей.
  // Проверяем если "Card UID" совпадёт с заранее записанным "Card UID" администратора, или LockSwitch больше 0.
  // Внимание!, замените "Card UID" администратора № 4198836864, на свой "Card UID". 
  if (uidDec == 4198836864 || LockSwitch > 0)EEPROMwriteUIDcard(); // Запускаем функцию записи в EEPROM "Card UID" пользователей.
  // ----------------------------------------


  // Для режима управления.
  if (LockSwitch == 0) // только если находимся в "режиме управления".
  {
    // Подготавливаемся для перебора массива CardUIDeEPROMread.
    for(ARRAYindexUIDcard = 0; ARRAYindexUIDcard <= 49; ARRAYindexUIDcard++) 
    {
      // фильтр, на тот случай если мы не запишим все 50 переменных массива, в не записанных переменных массива будут значение 0.    
      if (CardUIDeEPROMread[ARRAYindexUIDcard] > 0) // Перебираем весь массив CardUIDeEPROMread, фильтруем от 0.
      {
        //  Перебираем весь массив CardUIDeEPROMread, Если поднесённая карта совпадёт с одной из 50  переменных массива.       
        if (CardUIDeEPROMread[ARRAYindexUIDcard] == uidDec) // Если обнаружено совпадение с поднесенной картой.
        {
          CommandsCARD(); // Запускаем функцию для выполнения любых действий, в зависимости от "Card UID" поднесённой карты.
          break; // Выходим, останавливаем проверку / перебор / поиск, в массиве CardUIDeEPROMread.
          // Полезная вещь! Итак, в момент перебора массива, в случае совпадения с поднесенным "Card UID", мы останавливаем 
          // цикл  for, с помощью break, и в значении ARRAYindexUIDcard сохранится индекс массива, в котором находился "Card UID".
          // Знания значения индекса массива, упростит выполнение других задач. 
        }
      }
    }   
    // ARRAYindexUIDcard == 50 Означает, что мы перебрали весь массив от 0 до 49, и не обнаружили совпадений с поднесённой "Card UID".
    // То есть поднесенная карта отсутствует в базе, выводим на дисплей "не найдена карта" и № "Card UID" карты.
    if (ARRAYindexUIDcard == 50)display.println("NOT  Found CARD-UID"),display.println(uidDec),display.display();
    delay(2000);    
    ARRAYindexUIDcard = 0;
    DisplayWAiT_CARD(); // Запускаем функцию, заставка дисплея.
  }
} 

// ******************************************
// ******************************************



// Делаем функцию, записи в EEPROM "Card UID" пользователей, максимум 50 "Card UID".
void EEPROMwriteUIDcard() {

  if (LockSwitch == 0) // Если находились в режиме управления.
  {
    display.println("   START    RECORD   Card UID   CLIENT"); 
    display.display(); 
    for(int i = 0; i <= 2; i++)delay(500),digitalWrite(PiezoPin, HIGH),delay(500),digitalWrite(PiezoPin, LOW); // 3 раза пискнем.
  } 

  // -------

  // Для пропуска записи в ячейку памяти.
  if (LockSwitch > 0) // Если находимся в "режиме записи".
  {
    // Внимание!, замените "Card UID" администратора № 4198836864, на свой "Card UID". 
    if (uidDec == 4198836864) // Если поднесена карта администратора. 
    {
      display.println("   SKIP     RECORD   "); 
      display.setTextSize(3);
      display.setCursor(40,40);
      display.println(EEPROMstartAddr/5); // Выводим № пропущенной ячейки памяти.
      display.display();

      EEPROMstartAddr += 5; // Пропускаем запись в ячейку памяти, если не хотим записывать туда "Card UID".

      digitalWrite(PiezoPin, HIGH),delay(200),digitalWrite(PiezoPin, LOW); // 1 раз пискнем.
    }

    else // Иначе, то есть поднесена карта для записи.

    // "Card UID" / № карты это "длинное число", которое не поместится в одну ячейку памяти EEPROM. 
    // Разрубим "длинное число" на 4 части, и кусками, запишем его в 4 ячейки EEPROM. Начинаем запись с адреса EEPROMstartAddr.

    {
      EEPROM.write(EEPROMstartAddr, uidDec & 0xFF); 
      EEPROM.write(EEPROMstartAddr + 1, (uidDec & 0xFF00) >> 8); 
      EEPROM.write(EEPROMstartAddr + 2, (uidDec & 0xFF0000) >> 16); 
      EEPROM.write(EEPROMstartAddr + 3, (uidDec & 0xFF000000) >> 24);
      // Записали!.
      delay(10);
      // --
      display.println("RECORD OK! IN MEMORY "); 
      display.setTextSize(3);
      display.setCursor(50,40);
      display.println(EEPROMstartAddr/5); // Выводим № записанной ячейки памяти.
      display.display();

      EEPROMstartAddr += 5; // Прибавляем 5 к стартовой ячейки записи.
      for(int i = 0; i <= 40; i++)delay(5),digitalWrite(PiezoPin, HIGH),delay(5),digitalWrite(PiezoPin, LOW); // пискнем.
    }   
  }

  LockSwitch++; // Разблокируем режим записи, и заблокируем режим управления. 

  if (EEPROMstartAddr/5 == 50) // если дошли до 50. 
  {
    delay(2000);
    display.clearDisplay(); // Clear the buffer. / Очистить буфер.
    display.setTextColor(WHITE); // Цвет текста.
    display.setTextSize(3); // Размер текста (3).
    display.setCursor(0,0); 
    display.println("RECORD FINISH"); 
    display.display();


    digitalWrite(PiezoPin, HIGH),delay(3000),digitalWrite(PiezoPin, LOW); // Длинный писк.

    EEPROMstartAddr = 0;   
    uidDec = 0;
    ARRAYindexUIDcard = 0;

    EEPROMreadUIDcard(); // Запускаем функцию, для перезаписи массива CardUIDeEPROMread, данными из EEPROM.
  } 
}

// -------------------------------------------


// Делаем функцию, перезаписи массива CardUIDeEPROMread, данными из EEPROM.
void EEPROMreadUIDcard()
{
  for(int i = 0; i <= 49; i++)
  {
    byte val = EEPROM.read(EEPROMstartAddr+3);   
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val; 
    val = EEPROM.read(EEPROMstartAddr+2); 
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val; 
    val = EEPROM.read(EEPROMstartAddr+1); // увеличиваем EEPROMstartAddr на 1.
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val; 
    val = EEPROM.read(EEPROMstartAddr); 
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val;

    ARRAYindexUIDcard++; // увеличиваем на 1.
    EEPROMstartAddr +=5; // увеличиваем на 5.
  }

  ARRAYindexUIDcard = 0;
  EEPROMstartAddr = 0; 
  uidDec = 0;
  LockSwitch = 0;
  DisplayWAiT_CARD();        
}


// -------------------------------------------



// Делаем функцию, заставка дисплея.
void DisplayWAiT_CARD()
{
  display.clearDisplay(); // Clear the buffer. / Очистить буфер. 
  display.setTextColor(WHITE); // Цвет текста.
  display.setTextSize(4); // Размер текста (4).
  display.setCursor(0,0);
  display.println("WAIT");
  display.setCursor(25,35);
  display.println("CARD");
  display.display();
}

// -------------------------------------------

// Делаем функцию, для выполнения любых действий, в зависимости от "Card UID" поднесённой карты.
void CommandsCARD()
{
  display.print("Hi "); // Выводим на дисплей "привет".

  // В "ARRAYindexUIDcard" будет временно хранится номер индекса массива, совпавшего с поднесенной Card-UID.
  // Например, если поднесённый Card-UID совпал, с записанным  Card-UID в ячейке № 0 / индексом массива ARRAYindexUIDcard № 0. 
  if (ARRAYindexUIDcard == 0)
  {
    display.println("Dmitry"); // Выводим на дисплей "Dmitry". 
    // ! Здесь, можно дописать любые другие действия. 
  }  

  else if (ARRAYindexUIDcard == 1) // Если Card-UID получен из ячейки № 1 / индекс массива № 1. 
  {
    display.println("Sergey"); 
    // ! Здесь, можно дописать любые другие действия. 
  }

  else if (ARRAYindexUIDcard == 2) // Если Card-UID получен из ячейки № 2 / индекс массива № 2.
  {
    display.println("Peter");
    // ! Здесь, можно дописать любые другие действия. 
  }

  else if (ARRAYindexUIDcard == 3) 
  {
    display.println("Vasily");
  }
  //....................... 
  else if (ARRAYindexUIDcard == 49) 
  {
    display.println("OSIPOV");
  }
  //---
  // Далее, самостоятельно пропишите ножное количество "else if (ARRAYindexUIDcard == ", максимум 50 условий.

  //---------------------------------------

  display.setCursor(0,20); // Спускаем курсор на 20 Pixel.
  display.println(CardUIDeEPROMread[ARRAYindexUIDcard]); // Выводим на дисплей обнаруженный Card-UID.
  display.setCursor(40,40); // Спускаем курсор на 40 Pixel.
  display.setTextSize(3); // увеличиваем шрифт.
  display.println(ARRAYindexUIDcard); // Выводим на дисплей  № индекса массива / ячейки, где хранился обнаруженный Card-UID
  display.display(); // Чтобы сделать символы видимыми на дисплее !.
  // -------------

  // Подаём звуковой сигнал через PiezoPin, 
  // количество раз зависит от  № индекса массива / ячейки, где хранился обнаруженный Card-UID.
  for(int i = 0; i <= ARRAYindexUIDcard; i++)delay(20),digitalWrite(PiezoPin, HIGH),delay(20),digitalWrite(PiezoPin, LOW);
  delay(1); // Дёргаем микроконтроллер.
}





















