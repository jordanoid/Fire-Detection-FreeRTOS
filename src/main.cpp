#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <LiquidCrystal_I2C.h>

#define buzzPin 6
#define IRpin A0

int IRsens;

LiquidCrystal_I2C lcd (0x27, 16, 2);

void readFire(void *pvParameters);
void showLCD(void *pvParameters);
void buzzTrig(void *pvParameters);

SemaphoreHandle_t xBinarySemaphore;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(buzzPin, OUTPUT);
  pinMode(IRpin, INPUT);

  xBinarySemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(xBinarySemaphore); // Set binary semaphore = 0 agar scheduling dapat berjalan di awal

  // Pembuatan Task RTOS dengan prioritas dimana angka terbesar memiliki prioritas lebih dahulu
  xTaskCreate(readFire, "Deteksi Api", 128, NULL, 2, NULL);
  xTaskCreate(showLCD, "Show LCD", 128, NULL, 2, NULL);
  xTaskCreate(buzzTrig, "Trigger Buzzer", 128, NULL, 1, NULL);

  vTaskStartScheduler();
}

// Task mengambil data dari fire sensor
void readFire(void *pvParameters){
  for(;;){
    IRsens = analogRead(IRpin); // mengambil data dari sensor
    vTaskDelay(1);
  }
}

// Task menampilkan tulisan ke LCD
void showLCD(void *pvParameters){
  for(;;){

    xSemaphoreTake(xBinarySemaphore, portMAX_DELAY); // Mengambil binary semaphore sehingga akan menjalankan program pada task ini tanpa interupsi sampai give selanjutnya
    lcd.init();
    lcd.backlight();
    
    if(IRsens < 1000){ // Pengkondisian untuk tampilan LCD
      lcd.setCursor(0,0);
      lcd.print("Api");
      lcd.setCursor(0,1);
      lcd.print("Terdeteksi!");
    }else{
      lcd.setCursor(0,0);
      lcd.print("Tidak Ada Api");
      lcd.setCursor(0,1);
      lcd.print("Terdeteksi");
    }
    xSemaphoreGive(xBinarySemaphore); // give binary semaphore
    vTaskDelay(1);
  }
}

// Task membunyikan buzzer
void buzzTrig(void *pvParameters){
  for(;;){
    xSemaphoreTake(xBinarySemaphore, portMAX_DELAY); // Mengambil binary semaphore sehingga akan menjalankan program pada task ini tanpa interupsi sampai give selanjutnya
    if(IRsens < 1000){
      digitalWrite(buzzPin, HIGH);
      delay(200);
      digitalWrite(buzzPin, LOW);
      delay(200);
    }else{
      digitalWrite(buzzPin, LOW);
    }
    xSemaphoreGive(xBinarySemaphore); // give binary semaphore
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}