#include <M5Atom.h>
#include "Ambient.h"
#include <MHZ19_uart.h>
#include <Adafruit_BME280.h>

bool IMU6886Flag = false;
MHZ19_uart mhz19;  // CO2 sensor

Adafruit_BME280 bme;  // I2C Instance

WiFiClient client;
const char* ssid = "";  // Wi-FiのSSID
const char* password = "";  // Wi-Fiのパスワード

Ambient ambient;
unsigned int channelId = XXXXX; // AmbientのチャネルID
const char* writeKey = ""; // Ambientのライトキー

void setup() {
    M5.begin(true, true, true);  // UART, I2C, LED
    Wire.begin(26,32);  // init Grove I2C 
    M5.dis.setBrightness(10);  // >= 3
    M5.dis.drawpix(0, 0x000000);  // 初期LED: 黒
    Serial.begin(115200);
    ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化
    mhz19.begin(25, 21);  // UART GPIO RX:25, TX:21
    mhz19.setAutoCalibration(false);
    bme.begin(0x76, &Wire);  // Grove I2C
    // delay(3000);
}

uint8_t seq = 0; // remember number of boots in RTC Memory

void loop() {
    int lpcnt=0 ;
    int lpcnt2=0 ;

    Serial.print("\r\nWi-Fi scan start.\r\n");
    WiFi.begin(ssid, password);  //  Wi-Fi APに接続
    while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
      delay(500);
      lpcnt += 1 ;                           //
      if (lpcnt > 6) {                       // 6回目(3秒) で切断/再接続
        WiFi.disconnect(true,true);          //
        WiFi.begin(ssid, password);    //
        lpcnt = 0 ;                          //
        lpcnt2 += 1 ;                        // 再接続の回数をカウント
      }                                      //
      if (lpcnt2 > 3) {                      // 3回 接続できなければ、
        ESP.restart() ;                      // ソフトウェアリセット
      }                  
      Serial.print(".");
    }

    M5.dis.drawpix(0, 0x00f000);  // Wi-Fi接続LED: 赤
    Serial.print("\nWiFi connected. \nIP address: ");
    Serial.println(WiFi.localIP());

    // ONフラグとカウンタ値をAmbientに送信する
    seq++;  // カウンタを更新
    Serial.print("Seq: ");
    Serial.println(seq);

    // CO2
    int co2 = mhz19.getPPM();
    int co2temp = mhz19.getTemperature();
    Serial.print("CO2: ");
    Serial.println(co2);
    Serial.print("CO2-Temp: ");
    Serial.println(co2temp);

    // BME280
    float temp = bme.readTemperature();
    float pressure = bme.readPressure()/100.0F;
    float humidity = bme.readHumidity();
    Serial.print("Temp: ");
    Serial.println(temp);
    Serial.print("Pressure: ");
    Serial.println(pressure);
    Serial.print("Humidity: ");
    Serial.println(humidity);
    
    // send data to ambient
    ambient.set(1, 1);
    ambient.set(2, seq);
    ambient.set(3, co2);
    ambient.set(4, co2temp);
    ambient.set(5, temp);
    ambient.set(6, pressure);
    ambient.set(7, humidity);
    ambient.send();
    Serial.print("ambient: data sent. \n");

    WiFi.disconnect(true,true);
    M5.dis.drawpix(0, 0xf00000);  // 初期LED: 緑
    Serial.print("WiFi disconnected. \n");

    // delay(10000);  // 10秒毎に更新
    delay(59000);  // 1分毎に更新
}
