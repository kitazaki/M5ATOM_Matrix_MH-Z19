#include <M5Atom.h>
#include <MHZ19_uart.h>

bool IMU6886Flag = false;

MHZ19_uart mhz19;

void setup() {
  M5.begin(true, false, true);
  delay(50);
  for (int i = 0; i < 25; i++) {
    M5.dis.drawpix(i, 0xffffff);  // #GGRRBB の順
  }
  
  Serial.begin(115200);  // for debug log
  mhz19.begin(25, 21);  // Grove RX:26, TX:32 | GPIO RX:25, TX:21
  mhz19.setAutoCalibration(false);
  delay(3000);
}

void loop() {
  int co2 = mhz19.getPPM();
  int temp = mhz19.getTemperature();
 
  Serial.print(co2);
  Serial.print(" ppm, ");
  Serial.print(temp);
  Serial.println(" temp");

  if (co2 < 450) {
      for (int i = 0; i < 25; i++) {
        M5.dis.drawpix(i, 0x0000ff);  // blue
      }
  }
  else if (co2 < 700) {
      for (int i = 0; i < 25; i++) {
        M5.dis.drawpix(i, 0xff0000);  // green
      }
  }
  else if (co2 < 1000) {
      for (int i = 0; i < 25; i++) {
        M5.dis.drawpix(i, 0xffff00);  // yellow
      }
  }
  else {
    for (int i = 0; i < 25; i++) {
      M5.dis.drawpix(i, 0x00ff00);  // red
    }
  }
  delay(5000);
}
