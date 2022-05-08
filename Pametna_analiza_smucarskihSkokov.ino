#include "I2Cdev.h"
#include "MPU6050.h"
#include "SPI.h"
#include "SD.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#define OUTPUT_READABLE_YAWPITCHROLL

uint8_t devStatus;

MPU6050 accelgyro;

int16_t ax , ay, az;
int16_t gx, gy, gz;

bool button = false;

//bool buttonOff = false;

bool isrunning = false;


//SD card modul
const int chipSelect = D8;
File dataFile;

#define OUTPUT_READABLE_ACCELGYRO

//spremenljivki za txt fajle
const int LIMIT = 1000;
char fileName[13];

//spremenljivka za stevec lajnov
int line_counter = 0;

int stevec = 0;

void setup() {
    pinMode(16, INPUT);
    pinMode(0, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
      
      #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
          Wire.begin();
      #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
          Fastwire::setup(400, true);
      #endif
  
      Serial.begin(38400);

      accelgyro.initialize();

        //kalibracija
        accelgyro.setXGyroOffset(220);
        accelgyro.setYGyroOffset(76);
        accelgyro.setZGyroOffset(-85);
        accelgyro.setZAccelOffset(1788); 
    
        // make sure it worked (returns 0 if so)
        if (devStatus == 0) {
            // Calibration Time: generate offsets and calibrate our MPU6050
            accelgyro.CalibrateAccel(6);
            accelgyro.CalibrateGyro(6);
            accelgyro.PrintActiveOffsets();
        }

      

      if (!SD.begin(chipSelect)) {
        Serial.println("SD card initialization failed!");
        while (1);
      }

          
  }
  
  void loop() {

      if(isrunning == false){
        //za pregled in ustvarjanje txt fajlov 
        for (int n = 0; n < LIMIT; n++) {
          sprintf(fileName, "file%.3d.txt", n);
          if (SD.exists(fileName)) continue;
          dataFile = SD.open(fileName, FILE_WRITE);
          break; 
        }
        isrunning = true;  
      }

      
      accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      
      #ifdef OUTPUT_READABLE_ACCELGYRO

      //preverjanje stanja button-a
      if (digitalRead(16) == HIGH){
        button = !button;
        delay(200);
        stevec++;
        Serial.print(stevec);
      }

      if(stevec == 2){
          isrunning = false;
          stevec = 0;
          line_counter = 0;
          Serial.print("Nov skok");
        }

      dataFile = SD.open(fileName, FILE_WRITE);

      if(button){
        Serial.print(line_counter);Serial.print(";");
        Serial.print("ax:"); Serial.print(ax); Serial.print(";");
        Serial.print("ay:");Serial.print(ay); Serial.print(";");
        Serial.print("az:");Serial.print(az); Serial.print(";");
        Serial.print("gx:");Serial.print(gx); Serial.print(";");
        Serial.print("gy:");Serial.print(gy); Serial.print(";");
        Serial.print("gz:");Serial.print(gz); Serial.println("");
        
        //zapis podatkov na sd card 
        if (dataFile) {
          dataFile.print(line_counter);dataFile.print(";");
          dataFile.print(ax);dataFile.print(";");
          dataFile.print(ay);dataFile.print(";");
          dataFile.print(az);dataFile.print(";");
          dataFile.print(gx);dataFile.print(";");
          dataFile.print(gy);dataFile.print("");dataFile.println("");
          dataFile.close();
          //Serial.println("Writing to SD card was DONE!");
         }
         else {
          Serial.println("error opening file.txt");
         }
         line_counter++;
      }

      #endif
  }
