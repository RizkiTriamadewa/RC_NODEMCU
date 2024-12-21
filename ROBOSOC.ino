#define ENA   14          // Enable/speed motors Right        GPIO14(D5)
#define ENB   12          // Enable/speed motors Left         GPIO12(D6)
#define IN_1  15          // L298N in1 motors Right           GPIO15(D8)
#define IN_2  13          // L298N in2 motors Right           GPIO13(D7)
#define IN_3  2           // L298N in3 motors Left            GPIO2(D4)
#define IN_4  0           // L298N in4 motors Left            GPIO0(D3)
#define SERVO_PIN 4       // GPIO4 (D2) untuk kontrol servo

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Servo.h>         // Use the standard Servo library

Servo myServo;           // Deklarasi objek servo

String command;          // String to store app command state.
int speedCar = 800;      // 400 - 1023.
int speed_Coeff = 3;

const char* ssid = "NodeMCU Car";
ESP8266WebServer server(80);

unsigned long lastPushTime = 0;  // Menyimpan waktu terakhir push
const unsigned long pushInterval = 500;  // Interval push servo 500ms

void pushRobot() {
    unsigned long currentMillis = millis();
    
    if (currentMillis - lastPushTime >= pushInterval) {
        myServo.write(70);  // Servo ke 70 derajat
        delay(200);         // Tunggu selama 200ms
        myServo.write(0);   // Servo kembali ke 0 derajat
        lastPushTime = currentMillis;  // Update waktu terakhir push
    }
}

void setup() {
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);  
    pinMode(IN_1, OUTPUT);
    pinMode(IN_2, OUTPUT);
    pinMode(IN_3, OUTPUT);
    pinMode(IN_4, OUTPUT); 

    // Inisialisasi servo
    myServo.attach(SERVO_PIN);  // Pasangkan servo pada pin D2
    myServo.write(0);           // Set awal ke posisi 0 derajat

    Serial.begin(115200);

    // Mengatur WiFi Access Point
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid);

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    // Memulai server web
    server.on("/", HTTP_handleRoot);
    server.onNotFound(HTTP_handleRoot);
    server.begin();    
}

void goAhead(){ 
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
    analogWrite(ENA, speedCar);

    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, HIGH);
    analogWrite(ENB, speedCar);
}

void goBack(){ 
    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW);
    analogWrite(ENA, speedCar);

    digitalWrite(IN_3, HIGH);
    digitalWrite(IN_4, LOW);
    analogWrite(ENB, speedCar);
}

void goRight(){ 
    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW);
    analogWrite(ENA, speedCar);

    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, HIGH);
    analogWrite(ENB, speedCar);
}

void goLeft(){
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
    analogWrite(ENA, speedCar);

    digitalWrite(IN_3, HIGH);
    digitalWrite(IN_4, LOW);
    analogWrite(ENB, speedCar);
}

void stopRobot(){  
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, LOW);
    analogWrite(ENA, 0);

    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, LOW);
    analogWrite(ENB, 0);
}

void loop() {
    server.handleClient();
    
    command = server.arg("State");
    Serial.print("Received Command: ");
    Serial.println(command); // Debug: Log perintah yang diterima

    if (command == "F") {
        goAhead();
        Serial.println("Forward");
    }
    else if (command == "B") {
        goBack();
        Serial.println("Backward");
    }
    else if (command == "L") {
        goLeft();
        Serial.println("Left");
    }
    else if (command == "R") {
        goRight();
        Serial.println("Right");
    }
    else if (command == "0") {
        speedCar = 400;
        Serial.print("Speed set to: ");
        Serial.println(speedCar);
    }
    else if (command == "9") {
        speedCar = 1023;
        Serial.print("Speed set to: ");
        Serial.println(speedCar);
    }
    else if (command == "S") {
        stopRobot();
        Serial.println("Stop");
    }
    else if (command == "P") {
        pushRobot();
        Serial.println("Push");
    }
}

void HTTP_handleRoot(void) {
    if (server.hasArg("State")) {
        String state = server.arg("State");
        Serial.print("Received State: ");
        Serial.println(state);
    }
    server.send(200, "text/html", "Command received.");
}
