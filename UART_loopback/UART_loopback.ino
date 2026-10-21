#define UART_TX 17
#define UART_RX 16

unsigned long lastSend = 0;

void setup() {
    Serial.begin(9600); // was 115200 baud 
  Serial2.begin(9600, SERIAL_8N1, UART_RX, UART_TX);
}

void loop() {
  if (millis() - lastSend >= 1000) {
    lastSend = millis();
    Serial2.println("PING");
  }
  while (Serial2.available()) {
    Serial.write(Serial2.read());
  }
}