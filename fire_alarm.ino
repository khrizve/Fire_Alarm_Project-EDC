#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Display setup (Change I2C address if needed)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// DHT11 setup
#define DHTPIN 3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Pins
#define FLAME_PIN 2
#define SMOKE_PIN A0
#define BUZZER 4
#define RED_LED 5
#define GREEN_LED 6

// Smoke detection threshold (adjust after testing your sensor)
#define SMOKE_THRESHOLD 300
// Temperature threshold for fire alert (Celsius)
#define TEMP_THRESHOLD 50

void setup() {
  pinMode(FLAME_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  lcd.begin(16,2);
  lcd.backlight();
  dht.begin();

  Serial.begin(9600);
  lcd.print("Fire Alarm Ready");
  delay(2000);
  lcd.clear();
}

void loop() {
  int flame = digitalRead(FLAME_PIN);
  int smoke = analogRead(SMOKE_PIN);
  float temp = dht.readTemperature();

  if (isnan(temp)) {
    temp = 25.0; // Default room temperature if reading failed
  }

  Serial.print("Flame: "); Serial.print(flame);
  Serial.print(" | Smoke: "); Serial.print(smoke);
  Serial.print(" | Temp: "); Serial.println(temp);

  bool flameDetected = (flame == LOW); // Flame sensor active LOW (change if yours differs)
  bool smokeDetected = (smoke > SMOKE_THRESHOLD);
  bool highTemp = (temp > TEMP_THRESHOLD);

  if (flameDetected || smokeDetected || highTemp) {
    // Danger detected - alarm ON
    digitalWrite(BUZZER, HIGH);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("!!! FIRE ALERT !!!");
    lcd.setCursor(0, 1);
    if (flameDetected) {
      lcd.print("Flame Detected");
    } else if (smokeDetected) {
      lcd.print("Smoke: "); lcd.print(smoke);
    } else if (highTemp) {
      lcd.print("Temp: "); lcd.print(temp); lcd.print("C");
    }
  } else {
    // Safe state - alarm OFF
    digitalWrite(BUZZER, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Normal");
    lcd.setCursor(0, 1);
    lcd.print("Temp: "); lcd.print(temp); lcd.print("C");
  }

  delay(1000);
}
