bool geodinium_entered = false;
int ingress_ref = 0;
int egress_ref = 0;

bool geodinium_detected(int sensor_value, int ref) {
  int constexpr threshold = 6;
  return abs(sensor_value - ref) > threshold;
}

void motor_set(int pin, float duty) {
  // These were measured with an oscilloscope using the servo programmer
  // middle position: +duty = 7.45%
  // left position: +duty = 2.70%
  // right position +duty = 12.20%

  // this value is arbitrary, I was just trying to match the servo programmer as close as possible
  // weirdly, if I set the len any higher, the frequency will jump from ~60Hz to ~2KHz, but I'm not sure why
  float constexpr len = 16840.0f;

  // If I stop sending the signal, the motor will stop moving, so send it several times
  for (int i = 0; i < 10; ++i) {
    digitalWrite(pin, HIGH);
    delayMicroseconds((unsigned int)(len * duty));
    digitalWrite(pin, LOW);
    delayMicroseconds((unsigned int)(len  - (len * duty)));
  }
}

void direct_motor_geodinium() {
  motor_set(7, .065);
}

void direct_motor_nebulite() {
  motor_set(7, .087);
}

void setup() {
  Serial.begin(9600);
  // The servo will be plugged in here (white wire is signal)
  pinMode(7, OUTPUT);

  // the hall effect sensors aren't very noisy, so one measurement should be enough
  // The value with no magnetic field should be between 460 and 560 (it will vary between sensors b/c of tolerance)
  // https://www.diodes.com/assets/Datasheets/AH49E.pdf
  ingress_ref = analogRead(A0);
  egress_ref = analogRead(A1);

  Serial.print("Ingress ref: ");
  Serial.println(ingress_ref);
  Serial.print("Egress ref: ");
  Serial.println(egress_ref);

  motor_set(7, .0745);
  delay(500);
  direct_motor_nebulite();
}

void loop() {
  int ingress = analogRead(A0);
  int egress = analogRead(A1);

  Serial.print(ingress);
  Serial.print(", ");
  Serial.print(egress);
  Serial.println();

  if (geodinium_detected(egress, egress_ref) && geodinium_entered) {
    geodinium_entered = false;
    Serial.println("Exited");
    direct_motor_nebulite();
  }

  if (geodinium_detected(ingress, ingress_ref) && !geodinium_entered) {
    geodinium_entered = true;
    Serial.println("Entered");
    direct_motor_geodinium();
  }
}
