#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6I6L4nwQj"
#define BLYNK_TEMPLATE_NAME "dht11"
#define BLYNK_AUTH_TOKEN "MJ2xp0-cCSOMa9v_ELYkP1tpHlVnMh88"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

char ssid[] = "MT7628_AP";
char pass[] = "12345678";

#define DHTPIN 4
#define DHTTYPE DHT11
#define LED_PIN 27

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// ====== AUTO ======
float alpha = 0.2;     // hệ số lọc
float t_f = 0;         // nhiệt độ sau lọc
float setTemp = 30.0;  // ngưỡng bật LED

void sendDHT() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Loi doc DHT11");
    return;
  }

  // ---- LỌC NHIỄU ----
  t_f = alpha * t + (1 - alpha) * t_f;

  // ---- ĐIỀU KHIỂN TỰ ĐỘNG ----
  if (t_f > setTemp) {
    digitalWrite(LED_PIN, HIGH);
    Blynk.virtualWrite(V3, 1);
  } else {
    digitalWrite(LED_PIN, LOW);
    Blynk.virtualWrite(V3, 0);
  }

  // ---- GỬI LÊN BLYNK ----
  Blynk.virtualWrite(V1, t_f);
  Blynk.virtualWrite(V2, h);

  // ---- DEBUG ----
  Serial.print("T raw: ");
  Serial.print(t);
  Serial.print(" | T filter: ");
  Serial.print(t_f);
  Serial.print(" | H: ");
  Serial.println(h);
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // LẤY MẪU 2 GIÂY
  timer.setInterval(2000L, sendDHT);
}

void loop() {
  Blynk.run();
  timer.run();
}
