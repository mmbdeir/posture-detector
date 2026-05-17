
#include <Wire.h>
#include "DFRobot_HuskylensV2.h"
#include <math.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SDA_PIN 8
#define SCL_PIN 9

const char* ssid = "";
const char* password = "";

HuskylensV2 huskylens;

#define PRINT_ITEM(item) \
  Serial.print("result->" #item "=("); \
  Serial.print(result->item##_x); \
  Serial.print(","); \
  Serial.print(result->item##_y); \
  Serial.println(")");

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);



  Serial.println("Connecting to HuskyLens via I2C...");
  // HTTPClient http;

  // http.begin("http://httpbin.org/post");

  // int httpResponseCode = http.POST();

  // if (httpResponseCode > 0) {
  //   Serial.printf("HTTP Response code: %d\n", httpResponseCode);
  //   // Get the response payload as a String
  //   String payyload = http.getString();
  //   Serial.println("Response payload:");
  //   Serial.println(payyload);
  // } else {
  //   Serial.printf("Error in HTTP GET request: %s\n", http.errorToString(httpResponseCode).c_str());
  // }

  // http.end();
  while (!huskylens.begin(Wire)) {
    Serial.println(F("Begin failed!"));
    delay(300);
  }

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    delay(500);
  }

  huskylens.switchAlgorithm(ALGORITHM_POSE_RECOGNITION);
  Serial.println("HuskyLens Pose Recognition Ready!");
}

void sendPost(String messasge) {
  HTTPClient http;

  String url = "URL";

  http.begin(url);
  // http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Type", "text/plain");

  int httpCode = http.POST(messasge);

  if (httpCode > 0) {
    Serial.print("HTTP code: ");
    Serial.println(httpCode);

    String response = http.getString();
    Serial.println("Response:");
    Serial.println(response);
  } else {
    Serial.print("POST failed, error: ");
    Serial.println(httpCode);
  }

  http.end();
}


void loop() {
  if (!huskylens.getResult(ALGORITHM_POSE_RECOGNITION)) {
    Serial.println("No pose detected...");
    delay(1000);
    return;
  }

  while (huskylens.available(ALGORITHM_POSE_RECOGNITION)) {
    PoseResult* result =
      (PoseResult*)huskylens.popCachedResult(ALGORITHM_POSE_RECOGNITION);

    // Serial.println("\n===== POSE RESULT =====");

    // Serial.print("ID: ");
    // Serial.println(result->ID);

    // Serial.print("Center: (");
    // Serial.print(result->xCenter);
    // Serial.print(",");
    // Serial.print(result->yCenter);
    // Serial.println(")");

    // Serial.print("Width: ");
    // Serial.println(result->width);

    // Serial.print("Height: ");
    // Serial.println(result->height);

    // Serial.print("Name: ");
    // Serial.println(result->name);

    // Serial.print("Content: ");
    // Serial.println(result->content);

    // Print all 17 keypoints
    // PRINT_ITEM(nose);
    // PRINT_ITEM(leye);
    // PRINT_ITEM(reye);
    // PRINT_ITEM(lear);
    // PRINT_ITEM(rear);
    // PRINT_ITEM(lshoulder);
    // PRINT_ITEM(rshoulder);
    // PRINT_ITEM(lelbow);
    // PRINT_ITEM(relbow);
    // PRINT_ITEM(lwrist);
    // PRINT_ITEM(rwrist);
    // PRINT_ITEM(lhip);
    // PRINT_ITEM(rhip);
    // PRINT_ITEM(lknee);
    // PRINT_ITEM(rknee);
    // PRINT_ITEM(lankle);
    // PRINT_ITEM(rankle);

    // Serial.println("========================");
    angleCalculator(result);
  }

  delay(300);
}

void angleCalculator(PoseResult* result) {
  int lshoulderX = result->lshoulder_x;
  int lshoulderY = result->lshoulder_y;
  int rshoulderX = result->rshoulder_x;
  int rshoulderY = result->rshoulder_y;

  int lhipX = result->lhip_x;
  int lhipY = result->lhip_y;
  int rhipX = result->rhip_x;
  int rhipY = result->rhip_y;

  int lankleX = result->lankle_x;
  int lankleY = result->lankle_y;
  int rankleX = result->rankle_x;
  int rankleY = result->rankle_y;

  int shoulderX = (lshoulderX + rshoulderX) / 2;
  int shoulderY = (lshoulderY + rshoulderY) / 2;

  int hipX = (lhipX + rhipX) / 2;
  int hipY = (lhipY + rhipY) / 2;

  int footX = (lankleX + rankleX) / 2;
  int footY = (lankleY + rankleY) / 2;

  double distanceX = abs(shoulderX - hipX);
  double distanceY = abs(shoulderY - hipY);

  // double distanceFX = abs(shoulderX - footX);
  // double distanceFY = abs(shoulderY - footY);

  double distanceLX = abs(hipX - footX);
  double distanceLY = abs(hipY - footY);
  float backAngle = atan2(distanceY, distanceX) * 180 / M_PI;

  float legAngle = atan2(distanceLY, distanceLX) * 180 / M_PI;

  if (backAngle < 15) {
    sendPost("Laying down");
  } else {
    if (legAngle > 70) {
      sendPost("Standing");
    } else {
      sendPost("Sitting");
    }
  }


  delay(200);
}
