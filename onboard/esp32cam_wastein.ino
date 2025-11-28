#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_camera.h"
#include <ESP32Servo.h>

// ==========================================
// 1. NETWORK CONFIGURATION 
// ==========================================
const char* ssid = "Principal";
const char* password = "Digi1234";

// REPLACE '192.168.1.XX' with your laptop's IP address.
// Keep :8000/predict_raw at the end.
const char* serverUrl = "http://192.168.100.207:8000/predict_raw";

// ==========================================
// 2. SERVO CONFIGURATION (360 CONTINUOUS)
// ==========================================
Servo myServo;
#define SERVO_PIN 14  // GPIO 14 is a common free pin on ESP32-CAM

// For 360 Servos:
// 1500 = STOP
// 1000 = Full Speed Clockwise (approx)
// 2000 = Full Speed Counter-Clockwise (approx)
// We control position by "Time Spinning"
int STOP_SIGNAL = 1500;
int SPEED_CW = 1300;  // Adjust speed (1000-1499)
int SPEED_CCW = 1700; // Adjust speed (1501-2000)

// ==========================================
// 3. CAMERA PIN DEFINITIONS (AI-THINKER MODEL)
// ==========================================
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void setup() {
  Serial.begin(115200);
  
  // --- SERVO SETUP ---
  myServo.setPeriodHertz(50); 
  myServo.attach(SERVO_PIN, 1000, 2000);
  myServo.writeMicroseconds(STOP_SIGNAL); // Ensure it starts stopped

  // --- WIFI SETUP ---
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  // --- CAMERA SETUP ---
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Low quality for faster transmission during tests
  config.frame_size = FRAMESIZE_QVGA; // 320x240
  config.jpeg_quality = 12; // 0-63 lower is higher quality
  config.fb_count = 1;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera Init Failed");
    return;
  }
  Serial.println("Camera Ready!");
}

void loop() {
  // Wait 5 seconds before next scan (Simulates waiting for trash)
  // In real life, you might use an Ultrasonic sensor to trigger this
  delay(5000); 

  Serial.println("Capturing image...");
  
  // 1. Capture Image
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // 2. Connect to Server
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "image/jpeg");

  // 3. Send Image (RAW BYTES)
  Serial.println("Sending to Server...");
  int httpResponseCode = http.POST(fb->buf, fb->len);

  // 4. Handle Response
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Server Response: " + response);
    
    // Process the JSON string simply using indexOf
    // Response format: {"class": "plastic"}
    
    if (response.indexOf("plastic_metal") > 0) {
       Serial.println("Action: PLASTIC/METAL BIN");
       sortWaste("plastic_metal");
    } 
    else if (response.indexOf("glass") > 0) {
       Serial.println("Action: GLASS BIN");
       sortWaste("glass");
    }
    else if (response.indexOf("paper") > 0) {
       Serial.println("Action: PAPER BIN");
       sortWaste("paper");
    }
    else {
       Serial.println("Action: OTHER/WASTE BIN");
       sortWaste("other");
    }
  } else {
    Serial.printf("Error on sending POST: %d\n", httpResponseCode);
  }

  // 5. Cleanup
  esp_camera_fb_return(fb); // Free memory
  http.end();
}

// ==========================================
// 4. SERVO LOGIC (THE PHYSICAL MOVEMENT)
// ==========================================
void sortWaste(String type) {
  // Logic for 360 Continuous Servo:
  // You cannot set angle. You set SPEED and TIME.
  // Example logic: Rotate turntable to specific bin position.
  
  // NOTE: You must calibrate the 'delay' times below based on your mechanism!
  
  if (type == "plastic") {
    // Example: Rotate Right for 1 second
    myServo.writeMicroseconds(SPEED_CW);
    delay(1000); 
    myServo.writeMicroseconds(STOP_SIGNAL);
  } 
  else if (type == "metal") {
    // Example: Rotate Right for 2 seconds
    myServo.writeMicroseconds(SPEED_CW);
    delay(2000); 
    myServo.writeMicroseconds(STOP_SIGNAL);
  }
  else if (type == "paper") {
    // Example: Rotate Right for 3 seconds
    myServo.writeMicroseconds(SPEED_CW);
    delay(3000); 
    myServo.writeMicroseconds(STOP_SIGNAL);
  }
  else {
    // Waste: Rotate Left for 1 second
    myServo.writeMicroseconds(SPEED_CCW);
    delay(1000); 
    myServo.writeMicroseconds(STOP_SIGNAL);
  }
  
  // Optional: Reset mechanism if needed
  delay(1000);
}