#define TMP A0
#define BUZZER 3

void setup()
{
    pinMode(TMP, INPUT); // Temperature analog pin

    pinMode(BUZZER, OUTPUT); // Buzzer pin
    analogWrite(BUZZER, 0);


    Serial.begin(9600); // Serial port at 9600 kb/s
}

float temperature = 0;
float registerTmp=0;
float vout=0;


void loop()
{
    registerTmp = analogRead(TMP); // Read temperature
    vout = (5 * registerTmp) / 1023;
    temperature = (vout - 0.5) / 0.01;

    if (temperature > 36 && temperature < 37.5)
    {
        analogWrite(BUZZER, 67);
    }

    if (temperature > 37.5)
    {
        analogWrite(BUZZER, 127);
    }

    if (temperature < 36)
    {
        analogWrite(BUZZER, 0);
    }

    Serial.print("temperature: ");
    Serial.println(temperature);

    delay(1000);
}
