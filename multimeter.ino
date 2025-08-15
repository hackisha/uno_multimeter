// Arduino UNO continuity tester with LED indication
const byte PIN_SRC = 3;     // 소스 출력 (HIGH로 켜기)
const byte PIN_SENSE = A0;  // 측정 노드 (전압 읽기)
const byte PIN_LED = 8;     // LED (330~1kΩ 직렬저항으로 GND 연결)

const float VS = 5.0;       // 기준 전압(UNO)
const float RS = 2200.0;    // 전류제한 저항(2.2kΩ)
const float THRESH_OHM = 50.0; // 통전 판정 저항값

float readResistanceOnce() {
  int adc = analogRead(PIN_SENSE);
  float vnode = (adc * VS) / 1023.0;
  float denom = VS - vnode;
  if (denom <= 0.005) return 1e9; // 거의 개방으로 간주
  float rdut = (RS * vnode) / denom;
  return rdut;
}

float readResistanceFiltered(uint8_t n = 10) {
  float sum = 0;
  for (uint8_t i = 0; i < n; i++) {
    sum += readResistanceOnce();
    delay(2);
  }
  return sum / n;
}

void setup() {
  pinMode(PIN_SRC, OUTPUT);
  digitalWrite(PIN_SRC, HIGH); // 측정 전압 인가
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  analogReference(DEFAULT); // 5V 기준
  Serial.begin(115200);
}

void loop() {
  float r = readResistanceFiltered(16);

  static bool ledOn = false;
  const float onThresh = THRESH_OHM * 0.9;   // 켜짐 임계
  const float offThresh = THRESH_OHM * 1.1;  // 꺼짐 임계

  if (!ledOn && r <= onThresh) ledOn = true;
  else if (ledOn && r >= offThresh) ledOn = false;

  digitalWrite(PIN_LED, ledOn ? HIGH : LOW);

  // 시리얼 출력
  Serial.print("R = ");
  if (r > 9e8) Serial.println("OPEN");
  else {
    Serial.print(r, 1);
    Serial.println(" ohm");
  }
  delay(20);
}
