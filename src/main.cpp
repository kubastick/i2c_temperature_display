#include <Arduino.h>
#include <Wire.h>
#include <time.h>
#include "../.pio/libdeps/uno/Adafruit MCP9808 Library/Adafruit_MCP9808.h"
#include "../.pio/libdeps/uno/U8g2/src/U8g2lib.h"

#define LINE_SPACER 13
Adafruit_MCP9808 tempSensor = Adafruit_MCP9808();
U8G2_SH1106_128X64_NONAME_1_HW_I2C oledDisplay = U8G2_SH1106_128X64_NONAME_1_HW_I2C(U8G2_R0);

bool tempSensorInitialized = false;
long startTime = millis();

void setup() {
    pinMode(4,OUTPUT);

    // Init logging serial
    Serial.begin(9800);
    // while (!Serial);

    Serial.println("Initialized NilPointer temperature display");

    oledDisplay.begin();

    tempSensorInitialized = tempSensor.begin(0x18);
    if (tempSensorInitialized) {
        tempSensor.setResolution(2);
    }

}

void draw(float temp) {
    oledDisplay.setFont(u8g2_font_6x13_tf);

    if(tempSensorInitialized) {
        char textBuf[20];

        dtostrf(temp,2,2,textBuf);
        strcat(textBuf," °C");

#define Y_OFFSET 9
        oledDisplay.drawStr(0,Y_OFFSET,"Current temperature:");
        oledDisplay.drawStr(0,Y_OFFSET+LINE_SPACER,textBuf);

        if(temp>24) {
            oledDisplay.drawStr(0,Y_OFFSET+LINE_SPACER*2,"Very hot!");
        } else {
            oledDisplay.drawStr(0,Y_OFFSET+LINE_SPACER*2,"Temp is nice!");
        }

        long currentTime = millis();
        long timeDiff = currentTime - startTime;
        char timeBuf[30];
        ltoa(timeDiff/1000,timeBuf,10);

        char timeFinal[30+11] = "Uptime: ";
        strcat(timeFinal,timeBuf);
        strcat(timeFinal," s.");


        oledDisplay.drawStr(0,Y_OFFSET+LINE_SPACER*3,timeFinal);

    } else {
        oledDisplay.drawStr(0,Y_OFFSET,"Temp sensor error");
    }
}

void loop() {
    float temp = 0;
    if(tempSensorInitialized) {
        // Sensor is consuming around 200mA while being waked, so shutdown it to decrease power usage
        tempSensor.wake();
        temp = tempSensor.readTempC();
        tempSensor.shutdown();

        Serial.println("Current temp:");
        Serial.println(temp);

        // Blink diode
        /* if(temp>10) {
            int longBlinks = temp/(float)10;
            int shortBlinks = (int)temp % 10;

            for(int i = 0;i<longBlinks;i++){
                digitalWrite(4,HIGH);
                delay(500);
                digitalWrite(4,LOW);
                delay(200);
            }

            for(int i = 0;i<shortBlinks;i++){
                digitalWrite(4,HIGH);
                delay(200);
                digitalWrite(4,LOW);
                delay(100);
            }
        } */
    }

    oledDisplay.firstPage();
    do {
        draw(temp);
    } while ( oledDisplay.nextPage() );


    delay(1000);
}
