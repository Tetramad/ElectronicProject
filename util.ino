//uint32_t SetAngle(uint8_t servno, int16_t angle) {
//  servo.setPWM(servno, 0, map(angle,
//                              kServoAngleMin,
//                              kServoAngleMax,
//                              kServoCycleMin,
//                              kServoCycleMax));
//
//  return 500;
//}
//
//void Touring() {
//  if (millis() - elapsed_time > kTouringInterval) {
//    elapsed_time = millis();
//
//    angle += tour_direction;
//    if (angle > kServoAngleMax) {
//      angle = kServoAngleMax;
//      tour_direction = -tour_direction;
//    } else if (angle < kServoAngleMin) {
//      angle = kServoAngleMin;
//      tour_direction = -tour_direction;
//    }
//    SetAngle(kRotateServo, angle);
//  }
//}
//
//void Aiming() {
//  aimed = true;
//  angle += map(imagex, 0, kImageWidth, kFOV / 2, -kFOV / 2);
//  if (angle > kServoAngleMax) {
//    angle = kServoAngleMax;
//    aimed = false;
//  } else if (angle < kServoAngleMin) {
//    angle = kServoAngleMin;
//    aimed = false;
//  }
//  delay(SetAngle(kRotateServo, angle));
//}
//
//void ActionFar() {
//  if (imagex < kLeftDeadzone && imagex > kRightDeadzone) {
//    Serial.println("Hit deadzone");
//    return;
//  }
//  uint32_t wait = SetAngle(kTriggerServo, kTriggerForwardAngle);
//  delay(max(wait, uint32_t(kShootDelay)));
//  SetAngle(kTriggerServo, kTriggerBackAngle);
//}
//
//void ActionClose() {
//  for (int i = 0; i < 10; ++i) {
//    digitalWrite(kLED, LOW);
//    delay(100);
//    digitalWrite(kLED, HIGH);
//    delay(50);
//  }
//}
