#define TMP A0
#define SPO2_POT A1
#define BPM_POT A2
#define BUZZER 3

void setup()
{
    pinMode(TMP, INPUT); // Temperature analog pin
    pinMode(SPO2_POT, INPUT); // Oximetry analog pin
    pinMode(BPM_POT, INPUT); // Heart beat analog pin

    pinMode(BUZZER, OUTPUT); // Buzzer pin
    analogWrite(BUZZER, 0);


    Serial.begin(9600); // Serial port at 9600 kb/s
}

float temperature = 0;
float registerTmp=0;
float voutTmp=0;

float registerSpo2=0;
float voutOxi=0;
float spo2 = 0;

float registerBPM=0;
float voutBpm=0;
float bpm = 0;

void loop()
{
    // Temperature
    registerTmp = analogRead(TMP); // Read temperature register
    voutTmp = (5 * registerTmp) / 1023;
    temperature = (voutTmp - 0.5) / 0.01;

    Serial.print("TEMPERATURE: ");
    Serial.println(temperature);

    // Oximetry
    registerSpo2 = analogRead(SPO2_POT); // Read spo2 register
    voutOxi = (5 * registerSpo2) / 1023;
    spo2 = 20 * voutOxi;

    Serial.print("SPO2 %: ");
    Serial.println(spo2);

    // BPM
    registerBPM = analogRead(BPM_POT); // Read heart rate register
    voutBpm = (5 * registerBPM) / 1023;
    bpm = 44 * voutBpm;

    Serial.print("BPM: ");
    Serial.println(bpm);

    buzz(bpm, spo2, temperature, 10000); // Sound for 10 seconds

    delay(2000);
}

const float MIN_BPM = 60;
const float MAX_BPM = 100;
const float MAX_TMP = 37.5;
const float MIN_SPO2 = 95;

const unsigned int MAX_FREQ = 1000;
const int power = 70;

void buzz(float bpm, float spo2, float tmp, unsigned int span)
{
    byte abnormalBpm = bpm < MIN_BPM || bpm > MAX_BPM;
    byte lowSpo2 = spo2 < MIN_SPO2;
    byte highTmp = tmp > MAX_TMP;
    byte normalIndicators = !abnormalBpm && !lowSpo2 && !highTmp;

    unsigned long start = millis();
    unsigned long current = start;

    unsigned int  freq = MAX_FREQ;

    while (current - start <= span)
    {
        if (normalIndicators)
        {
            analogWrite(BUZZER, 0);
            break;
        }

        if (abnormalBpm)
        {
            analogWrite(BUZZER, power);
            freq -= 250;
        }

        if (lowSpo2)
        {
            analogWrite(BUZZER, power);
            freq -= 250;
        }

        if (highTmp)
        {
            analogWrite(BUZZER, power);
            freq -= 250;
        }

        current = millis();
        delay(freq);
        analogWrite(BUZZER, 0);
        delay(freq);
        freq = MAX_FREQ;
    }
}
