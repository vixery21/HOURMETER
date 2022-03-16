
  //========================================================\\
  //------------------------Libraries-----------------------\\
  //========================================================\\

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <RTClib.h>
#include <SD.h>
#include <SPI.h>
//#include <ESP8266WiFiMulti.h>

int chipSelect = 10;

  //========================================================\\
  //------------------Hardware being used-------------------\\
  //========================================================\\

RTC_DS3231 rtc;
DateTime now;
File file1;
hd44780_I2Cexp lcd;
File file2;
//ESP8266WiFiMulti wifiMulti;

  //========================================================\\
  //--------Button that will be used on Arduino Pins--------\\
  //========================================================\\

int P0 = 2; // Button Reset Data
int P1 = 3; // Button SET MENU
int lmp1 = 6; // Green
int lmp2 = 7; // Red

  //========================================================\\
  //-----Variables Used Including New Debounce Variable-----\\
  //========================================================\\

int menu = 0;
const int debounce = 35;
byte hitung;
unsigned long waktu_semua;
const unsigned long eventInterval = 1000;
//const unsigned long Interval_Notification = 1800000000;
const unsigned long Interval_Notification = 20000;
unsigned long previousTime = 0;
unsigned long previousTime2 = 0;
unsigned long currentMillis;
unsigned long seconds;
unsigned long minutes;
unsigned long hours ;
unsigned long seconds2;
unsigned long minutes2;
unsigned long hours2;
unsigned long detiktersimpan;

enum Mode
{
  modeWaktuSekarang,
  modeWaktuSemua,
  modeDisplayDateTime,
  modeInformasi,
  jumlahMode,
};

  //========================================================\\
  //-----------------------Viod Setup-----------------------\\
  //========================================================\\

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  SD.begin(chipSelect);

  pinMode(P0, INPUT);
  pinMode(P1, INPUT);
  pinMode(lmp2, OUTPUT);
  pinMode(lmp1,OUTPUT);
  //pinMode(P2,INPUT);
  //pinMode(P3,INPUT);

  //WiFi.persistent(false);
  //WiFi.mode(WIFI_STA);
  //wifiMulti.addAP("Waskita Wifi", "");
  //wifiMulti.addAP("Waskita Wifi 2", "");

  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("Halo,");
  lcd.setCursor(0, 1);
  lcd.print("Selamat Datang!");
  delay(3000);

  lcd.setCursor(0, 0);
  lcd.print("Waskita Beton");
  lcd.setCursor(0, 1);
  lcd.print("Precast Klaten");
  delay(3000);

  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");


  if (! rtc.begin()) {
    //Serial.print("rtc is NOT running!");
    lcd.setCursor(0, 1);
    lcd.print("rtc Tidak Terbaca");
    // Set the date and time at compile time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    delay(1000);
    lcd.setCursor(0, 0);
    lcd.print("                 ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }

  if(!SD.begin(chipSelect)){
    //Serial.print("SDCARD GAGAL");
    lcd.setCursor(0, 1);
    lcd.print("SDCARD GAGAL");
    delay(1000);
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }else{
    lcd.setCursor(0, 0);
    lcd.print("SDCARD BERHASIL");
    Serial.println("Mencoba Read...");

    file2 = SD.open("Data_Log.txt");
    if(file2){
      while(file2.available()){
        long value_log =file2.parseInt();
      }
    file2.close();
    }


    file1 = SD.open("LOG.txt");
    if (file1) {
      while (file1.available()) {
        long value = file1.parseInt();
        waktu_semua = value;
        Serial.println(value);
      }
      file1.close();
      lcd.setCursor(0, 1);
      lcd.print("Read Berhasil");
      delay(2000);
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Read Gagal");
      delay(2000);
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
    }
  }
  if (digitalRead(P0) == HIGH) {
    SD.remove("LOG.txt");
    Serial.print("RESET");
  }
}

  //========================================================\\
  //---------------------Viod Loop--------------------------\\
  //========================================================\\

void loop() {
  DateTime now = rtc.now();

  switch (hitung) {
    case modeWaktuSekarang:
      WaktuSekarang();
      break;

    case modeWaktuSemua:
      WaktuSemua();
      break;

    case modeDisplayDateTime:
      DisplayDateTime();
      break;

    case modeInformasi:
      informasi();
      break;
  }

  if (digitalRead(P1))
  {
    delay(100);
    if (digitalRead(P1))
    {
      hitung++;
      hitung %= jumlahMode;
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      while (digitalRead(P1));
    }
  }

  currentMillis = millis();
  seconds = currentMillis / 1000;
  minutes = seconds / 60;
  hours = minutes / 60;

  currentMillis %= 1000;
  seconds %= 60;
  minutes %= 60;
  //hours %= 24;

  
  unsigned long waktu_notif = millis(); // For Notification if the time has passed
  unsigned long currentTime = millis(); //Make a Delay
  unsigned long currentMillis1 = (waktu_semua * 1000) +  millis();
  seconds2 = currentMillis1 / 1000;
  minutes2 = seconds2 / 60;
  hours2 = minutes2 / 60;
  currentMillis1 %= 1000;
  int(seconds2 %= 60);
  int(minutes2 %= 60);
  int(hours2 %= 10000);
  //hours %= 24;
  detiktersimpan = seconds2 + (minutes2 * 60) + (hours2 * 3600);

  //========================================================
  //Write data SD CARD
  //========================================================
  
  if (currentTime - previousTime >= eventInterval) {
    SD.remove("LOG.txt");
    file1 = SD.open("LOG.txt", FILE_WRITE);
    if (file1) {
      file1.print(detiktersimpan);
      Serial.println(detiktersimpan);
      Serial.println("Berhasil Write Data 1");
      file1.close();
    }
    else {
      Serial.println("Gagal Write Data 1");
    }
    file2 = SD.open("Data_Log.txt", FILE_WRITE);
    if (file2) {
      file2.print(hours2);
      file2.print(":");
      file2.print(minutes2);
      file2.print(":");
      file2.println(seconds2);
      Serial.println("Berhasil Write Data LOG");
      file2.close();
    } else {
      Serial.println("Gagal Write Data LOG");
    }
    previousTime = currentTime;
  }

  if (digitalRead(P0)) {
    delay(100);
    if (digitalRead(P0)) {
      SD.remove("LOG.txt");//Serial.println("Berhasil Menghapus");
      while (digitalRead(P0));
    }
  }
}

  //========================================================\\
  //--------------------Display Date Time-------------------\\
  //========================================================\\

void DisplayDateTime ()
{
  // We show the current date and time
  DateTime now = rtc.now();
  if (digitalRead(P1)) {
    menu = 1;
    delay(debounce);
  }
  lcd.setCursor(0, 1);
  lcd.print("Hour: ");
  if (now.hour() <= 9)
  {
    lcd.print("0");
  }
  lcd.print(now.hour(), DEC);
  lcd.print(":");
  if (now.minute() <= 9)
  {
    lcd.print("0");
  }
  lcd.print(now.minute(), DEC);
  lcd.print(":");
  if (digitalRead(P1)) {
    menu = 1;
    delay(debounce);
  }
  if (now.second() <= 9)
  {
    lcd.print("0");
  }
  lcd.print(now.second(), DEC);

  lcd.setCursor(0, 0);
  lcd.print("Date: ");
  if (now.day() <= 9)
  {
    lcd.print("0");
  }
  lcd.print(now.day(), DEC);
  lcd.print("/");
  if (digitalRead(P1)) {
    menu = 1;
    delay(debounce);
  }
  if (now.month() <= 9)
  {
    lcd.print("0");
  }
  lcd.print(now.month(), DEC);
  lcd.print("/");
  lcd.print(now.year(), DEC);

  delay(100);
}

  //========================================================\\
  //----------------------Informasi Alat--------------------\\
  //========================================================\\

void informasi() {
  lcd.setCursor(0, 0);
  lcd.print("Informasi Alat :");
  lcd.setCursor(0, 1);
  lcd.print("Wheel Loader");
}

  //========================================================\\
  //---------------Display Waktu Lama Berjalan--------------\\
  //========================================================\\

void WaktuSekarang() {
  digitalWrite(lmp1, HIGH);
  lcd.setCursor(0, 0);
  lcd.print("Running Time");
  lcd.setCursor(0, 1);
  if (hours <= 9) {
    lcd.print("0");
  }
  lcd.print(hours);
  lcd.setCursor(2, 1);
  lcd.print(":");
  lcd.setCursor(3, 1);
  if (minutes <= 9) {
    lcd.print("0");
  }
  lcd.print(minutes);
  lcd.setCursor(5, 1);
  lcd.print(":");
  lcd.setCursor(6, 1);
  if (seconds <= 9) {
    lcd.print("0");
  }
  lcd.print(seconds);

  delay(100);
}

  //========================================================\\
  //--------------Display Total Waktu Berjalan--------------\\
  //========================================================\\

void WaktuSemua() {
  digitalWrite(lmp1, HIGH);
  lcd.setCursor(0, 0);
  lcd.print("Total Time");
  lcd.setCursor(0, 1);
  if (hours2 <= 9) {
    lcd.print("0");
  }
  lcd.print(hours2);
  lcd.setCursor(2, 1);
  lcd.print(":");
  lcd.setCursor(3, 1);
  if (minutes2 <= 9) {
    lcd.print("0");
  }
  lcd.print(minutes2);
  lcd.setCursor(5, 1);
  lcd.print(":");
  lcd.setCursor(6, 1);
  if (seconds2 <= 9) {
    lcd.print("0");
  }
  lcd.print(seconds2);
  delay(100);
}
