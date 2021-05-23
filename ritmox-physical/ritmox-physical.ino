#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

#define TMP A0
#define BUZZER D5

MAX30105 particleSensor;

void setup()
{
    pinMode(TMP, INPUT); // Temperature analog pin
    Serial.begin(9600); // Serial port at 9600 kb/s

    pinMode(BUZZER, OUTPUT); // Buzzer pin
    noTone(BUZZER);

    // Initialize sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
    {
        Serial.println("MAX30105 was not found. Please check wiring/power. ");
        while (1);
    }

    particleSensor.setup(); //Configure sensor with default settings
    particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
    particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

void loop()
{
    // Check finger presence
    long irValue = particleSensor.getIR();

    if (irValue < 5000)
    {
        Serial.println(" No finger?");
    }
    else
    {
        float avgBPM = readBPM(20000); // Read BPM for 20 seconds.

        if (avgBPM < 0)
        {
            Serial.println(" No finger?");
        }
        else
        {
            Serial.print("average-BPM: ");
            Serial.print(avgBPM);

            float averageTmp = readTemperature(10000); // Read temperature for 10 seconds.

            Serial.print("average-temperature: ");
            Serial.println(averageTmp);

            buzz(avgBPM, averageTmp, 5000); // Sound for 5 seconds
        }
    }

    delay(2000);
}

float readBPM(unsigned int span)
{
    const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
    byte rates[RATE_SIZE]; //Array of heart rates
    byte rateSpot = 0;
    long lastBeat = 0; //Time at which the last beat occurred
    float beatsPerMinute;
    long lastIRvalue;
    float beatAvg;

    unsigned long start = millis();
    unsigned long current = start;

    while (current - start <= span)
    {
        long irValue = particleSensor.getIR();
        lastIRvalue = irValue;

        if (checkForBeat(irValue) == true)
        {
            long delta = millis() - lastBeat;
            lastBeat = millis();

            beatsPerMinute = 60 / (delta / 1000.0);

            if (beatsPerMinute < 255 && beatsPerMinute > 20)
            {
                rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
                rateSpot %= RATE_SIZE; //Wrap variable

                //Take average of readings
                beatAvg = 0;
                for (byte x = 0 ; x < RATE_SIZE ; x++)
                {
                    beatAvg += rates[x];
                }

                beatAvg /= RATE_SIZE;
            }
        }

        current = millis();
    }

    if (lastIRvalue < 5000)
    {
        return -1;
    }
    else
    {
        return beatAvg;
    }
}

const float WEMOS_VOLTAGE = 3.3;

// Read temperature for a given span of time and return
// average temperature.
float readTemperature(unsigned int span)
{
    float registerTmp;
    float vout;
    float temperature;
    float avgTemperature = -1;

    unsigned long start = millis();
    unsigned long current = start;

    while (current - start <= span)
    {
        registerTmp = analogRead(TMP); // Read temperature
        vout = (WEMOS_VOLTAGE * registerTmp) / 1023;

        temperature = vout / 0.01;

        if (avgTemperature < 0)
        {
            avgTemperature = temperature;
        }
        else
        {
            avgTemperature = (avgTemperature + temperature) / 2;
        }

        current = millis();
        delay(100); // Delay between readings for performance purposes
    }

    return avgTemperature;
}

const float MIN_BPM = 60;
const float MAX_BPM = 100;
//const float MAX_TMP = 37.5;
const float MAX_TMP = 34.5;

const unsigned int MAX_FREQ = 1100;

void buzz(float bpm, float tmp, unsigned int span)
{
    byte abnormalBpm = bpm < MIN_BPM || bpm > MAX_BPM;
    byte highTmp = tmp > MAX_TMP;
    byte normalIndicators = !abnormalBpm && !highTmp;

    unsigned long start = millis();
    unsigned long current = start;

    unsigned int  freq = MAX_FREQ;

    while (current - start <= span)
    {
        if (normalIndicators)
        {
            noTone(BUZZER);
            break;
        }

        if (abnormalBpm)
        {
            digitalWrite(BUZZER, HIGH);
            tone(BUZZER, 261);
            freq -= 300;
        }

        if (highTmp)
        {
            digitalWrite(BUZZER, HIGH);
            tone(BUZZER, 261);
            freq -= 300;
        }

        current = millis();
        delay(freq);
        noTone(BUZZER);
        delay(freq);
        freq = MAX_FREQ;
    }
}
