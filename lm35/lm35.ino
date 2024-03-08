#include <WiFi.h>
#include <TCS3200.h>
#include <ESP32_Servo.h>
#include <FirebaseESP32.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define S0 12
#define S1 13
#define S2 14 
#define S3 27
#define OUT 26

#define WIFI_SSID "Realme 2"
#define WIFI_PASSWORD "dekyongg"
#define API_KEY "0mXLGhlmX6XCi4QU5DbhFGj6W7XI9NRGefqhScC8"
#define DATABASE_URL "https://pisang-sale-project-default-rtdb.asia-southeast1.firebasedatabase.app/"

Servo servo1;
Servo servo2;

int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

const int sensorPin = A0;                         // Pin tempat sensor LM35 terhubung
const int sensorMq = A3;                          // Pin tempat sensor MQ2 terhubung

float temperature;                                // Variable untuk menyimpan nilai suhu
float asap;                                       // Variabel untuk menyimpan nilai ppm asap



//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

String mainFirebase = "";

int a, b, c, d, e; 

int readFrequency() {
  // Read the frequency from the sensor
  return pulseIn(OUT, LOW);
}

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
    
  // Menginisialisasi objek servo1 dan servo2 dengan pin yang tepat
  servo1.attach(2);
  servo2.attach(4);
  servo1.write(0);
  servo2.write(0);
  Serial.begin(9600);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  // Assign the api key (required)
  config.api_key = API_KEY;

  config.database_url = DATABASE_URL;

  Firebase.begin(&config, &auth);

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);
}

void loop() {
  int sensorValue = analogRead(sensorPin);              // Membaca tegangan dari sensor LM35
  float temperature = (sensorValue * 3.5 / 4095) * 200;   // Mengubah tegangan dari sensor LM35 ke nilai Suhu
  float asapValue = analogRead(asap);                     // Membaca tegangan dari sensor MQ2
  float ppm = (sensorValue / 4095.0 * 3.3) * 1000;      // Mengubah tegangan dari sensor LM35 ke nilai Suhu


  
  // Select RED filtered photodiodes and start reading frequency
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  redFrequency = readFrequency();

  // Select GREEN filtered photodiodes and start reading frequency
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  greenFrequency = readFrequency();

  // Select BLUE filtered photodiodes and start reading frequency
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blueFrequency = readFrequency();

  // Inisiasi servo berdasarkan nilai ppm dan suhu
  if(temperature >= 30){
    servo1.write(180);
    servo2.write(180);
    delay(500);
  }
  if (ppm >= 130){
    servo1.write(180);
    servo2.write(180);
    delay(500);    
  }
  else{
    servo1.write(0);
    servo2.write(0);
    delay(500);
  }

  // Tunggu beberapa saat sebelum membaca kembali sensor
  delay(1000);
  
  a=temperature;
  b=ppm;
  c=redFrequency;
  d=greenFrequency;
  e=blueFrequency;
  
  
  if (Firebase.ready()) { 
    //Firebase.setInt(fbdo, main, 5);
    Firebase.setInt(fbdo, "/Asap/", a);
    Firebase.setInt(fbdo, "/Suhu/", b);
    Firebase.setInt(fbdo, "/Warna-B/", e);
    Firebase.setInt(fbdo, "/Warna-G/", d);
    Firebase.setInt(fbdo, "/Warna-R/", c);
    delay(200);
  }

  // Menampilkan nilai suhu dan ppm di serial monitor
  Serial.print("Suhu saat ini: ");
  Serial.print(temperature);
  Serial.println("Derajat Celsius");
  Serial.print("Kepulan Asap");
  Serial.println(ppm);
  Serial.print("Red: ");
  Serial.print(redFrequency);
  Serial.print(" Green: ");
  Serial.print(greenFrequency);
  Serial.print(" Blue: ");
  Serial.println(blueFrequency);

  delay(5000); // Adjust delay as needed
}


 
