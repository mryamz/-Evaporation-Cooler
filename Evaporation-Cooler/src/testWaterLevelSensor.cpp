

int water_level_value;
int water_level_pin = A5;

void setup() {
	Serial.begin(9600);
}

void loop() {
	water_level_value = analogRead(water_level_pin);
	Serial.println(water_level_value);
	delay(1000);
}