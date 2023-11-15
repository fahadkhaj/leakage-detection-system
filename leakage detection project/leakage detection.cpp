#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <CayenneMQTTESP8266.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
char ssid[] = "Kemmy";
char password[] = "2626seaton";
char username[] = "2c6b9cb0-09b3-11ee-8485-5b7d3ef089d0";
char mqtt_password[] = "dbe5289c171babde09f0ff852c243ac1d440a89b";
char client_id[] = "34e67ae0-09b3-11ee-8485-5b7d3ef089d0";
byte sensorOnePin = 14;
byte sensorOnePin1 = 12;
float calibrationFactor = 4.5;
volatile byte pulseCount;
volatile byte pulseCount1;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
float flowRate1;
unsigned int flowMilliLitres1;
unsigned long totalMilliLitres1;
float difference;
unsigned long oldTime;
unsigned long oldTime1;

void setup()
{
    Cayenne.begin(username, mqtt_password, client_id, ssid, password);
    Serial.begin(9600);
    pinMode(sensorOnePin, INPUT);
    digitalWrite(sensorOnePin, HIGH);
    pinMode(sensorOnePin1, INPUT);
    digitalWrite(sensorOnePin1, HIGH);
    pulseCount = 0;
    flowRate = 0.0;
    flowMilliLitres = 0;
    totalMilliLitres = 0;
    oldTime = 0;
    pulseCount1 = 0;
    flowRate1 = 0.0;
    flowMilliLitres1 = 0;
    totalMilliLitres1 = 0;
    oldTime1 = 0;
    difference = 0;
    lcd.init();
    lcd.clear();
    lcd.backlight(); // Make sure backlight is on
    lcd.setCursor(0, 0);
    lcd.print("LEAK SYSTEM");
    attachInterrupt(digitalPinToInterrupt(sensorOnePin), pulseCounter, FALLING);
    attachInterrupt(digitalPinToInterrupt(sensorOnePin1), pulseCounter1, FALLING);
}

void loop()
{
    Cayenne.loop();
    delay(1000);

    if ((millis() - oldTime) > 1000 && (millis() - oldTime1) > 1000)
    {
        detachInterrupt(digitalPinToInterrupt(sensorOnePin));
        detachInterrupt(digitalPinToInterrupt(sensorOnePin1));

        flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
        flowRate1 = ((1000.0 / (millis() - oldTime1)) * pulseCount1) / calibrationFactor;
        difference = flowRate - flowRate1;
        Cayenne.virtualWrite(1, flowRate);
        Cayenne.virtualWrite(2, flowRate1);
        Cayenne.virtualWrite(3, difference);
        oldTime = millis();
        oldTime1 = millis();

        Serial.print("Flow rate1: ");
        Serial.print(int(flowRate)); // Print the integer part of the variable
        Serial.print("L/min");
        Serial.print("\n"); // Print tab space
        Serial.print("Flow rate2: ");
        Serial.print(int(flowRate1)); // Print the integer part of the variable
        Serial.print("L/min");
        Serial.print("Differance ");
        Serial.print(int(difference)); // Print the integer part of the variable

        if (difference == 0)
        {
            delay(2000);
            lcd.clear();
            lcd.setCursor(0, 1); // Set cursor to character 2 on line 0
            lcd.print("NO LEAKAGE");
        }
        else if (difference > 1)
        {
            lcd.clear();
            lcd.setCursor(0, 1); // Set cursor to character 2 on line 0
            lcd.print(" LEAKAGE");
        }
        pulseCount = 0;
        pulseCount1 = 0;
        attachInterrupt(digitalPinToInterrupt(sensorOnePin), pulseCounter, FALLING);
        attachInterrupt(digitalPinToInterrupt(sensorOnePin1), pulseCounter1, FALLING);
    }
}
ICACHE_RAM_ATTR void pulseCounter()
{
    pulseCount++;
}
ICACHE_RAM_ATTR void pulseCounter1()
{
    pulseCount1++;
}
