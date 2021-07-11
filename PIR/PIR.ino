#include <M5StickC.h>
#include "Mailer.h"
#include <ezTime.h>
#include <Espalexa.h>
Espalexa espalexa;
//create devices yourself
EspalexaDevice* epsilon;
//callback functions
//new callback type, contains device pointer
void motionChanged(EspalexaDevice* dev);

String alexaSkill = "Tmotion2";
String email_content1 = "S2:動体検知 ";

const char* wifi_ssid = "input your wifi ssid";
const char* wifi_pass = "input your wifi pass";
const char* smtp_username = "input your smtp username";
const char* smtp_password = "input your smtp pass";
const char* smtp_from_address = "input your smtp from address";
const int smtp_port = 465;
const char* smtp_hostname = "smtp.mail.yahoo.co.jp";

const char* to_address = "input your smtp to address";
const char* subject = "動体検知アラート";
const int minimum_email_interval_seconds = 6 * 1;  // 6sec

Mailer mail(smtp_username, smtp_password, smtp_from_address, smtp_port,
            smtp_hostname);
Timezone Tokyo;
time_t last_emailed_at;
void send_email(const String content);
bool isToggle = false;
bool isMotionDetect = true;
        
#define BTN_A_PIN 37
#define BTN_B_PIN 39
#define LED_PIN   10
// このLEDは、GPIO10の電位を下げることで発光するタイプ
#define LED_ON  LOW
#define LED_OFF HIGH

// INPUT_PULLUPが有効かは不明だが、有効という前提で定義
#define BTN_ON  LOW
#define BTN_OFF HIGH

uint8_t prev_btn_a = BTN_OFF;
uint8_t btn_a      = BTN_OFF;
uint8_t prev_btn_b = BTN_OFF;
uint8_t btn_b      = BTN_OFF;


//our callback functions
void motionChanged(EspalexaDevice* d) {
  if (d == nullptr) return; //this is good practice, but not required

  M5.Lcd.setCursor(60, 0, 1);
  M5.Lcd.println("");
  M5.Lcd.println("");
  M5.Lcd.println("");
  M5.Lcd.println("");
  M5.Lcd.println("");
  M5.Lcd.println("");
  //do what you need to do here
  //EXAMPLE
  Serial.print("A changed to ");
  if (d->getValue()){
    Serial.println("MotionDetect ON");
    M5.Lcd.println("MotionDetect ON    ");
    isMotionDetect = true;
  }
  else {
    Serial.println("MotionDetect OFF");
    M5.Lcd.println("MotionDetect OFF  ");
    isMotionDetect = false;
  }
}

void setup_mail(){
  String ssid = wifi_ssid;
  M5.Lcd.println("Connecting to "+ ssid);
  WiFi.begin(wifi_ssid, wifi_pass);
  M5.Lcd.println("waitForSync");
  Serial.println("waitForSync");
  waitForSync();
  M5.Lcd.println("WiFi connected");
  Serial.println("WiFi connected");
  M5.Lcd.println("IP Address: " + WiFi.localIP().toString());
  Serial.println("IP Address: " + WiFi.localIP().toString());
  Tokyo.setLocation("Asia/Tokyo");
  M5.Lcd.println("Asia/Tokyo time: " + Tokyo.dateTime());
  last_emailed_at = Tokyo.now();
  M5.Lcd.fillScreen(BLACK);
}

void setup() {
  Serial.begin(115200);
  M5.begin();
  // put your setup code here,  M  log_iion(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setRotation(3);
  M5.Lcd.println("PIR TEST");
  Serial.println("PIR TEST");
  pinMode(36,INPUT_PULLUP);
  pinMode(BTN_A_PIN, INPUT_PULLUP);
  pinMode(BTN_B_PIN, INPUT_PULLUP);
  pinMode(LED_PIN,   OUTPUT);
  digitalWrite(LED_PIN, LED_OFF);
  setup_mail();
  
  // Delog_ices here. 
  espalexa.addDevice(alexaSkill, motionChanged, EspalexaDeviceType::onoff); //non-dimmable device
  espalexa.begin();
  delay(5000);
}

void send_mail_wrap(){
  int now_hour = Tokyo.hour();
  M5.Lcd.setCursor(0, 0, 1);
  M5.Lcd.println("now hour = "+ String(now_hour) + "    ");
  
  // 経過時間を計算しそれが一定以上、かつ温度が閾値を超えていたらメール送信
  auto elapsed_seconds = difftime(Tokyo.now(), last_emailed_at);
  
  String email_content = email_content1 ;
  email_content = email_content + "動くものを検知しました。";
  email_content = email_content + Tokyo.dateTime();
  Serial.println(email_content);

  log_d("経過時間：%.1f秒", elapsed_seconds);
  if (elapsed_seconds > minimum_email_interval_seconds) {
      send_email(email_content);
  }
}

void send_email(const String content) {
  Serial.println("send_email");
  mail.send(to_address, subject, content);

  M5.Lcd.setCursor(0, 0, 1);
  M5.Lcd.println("");
  M5.Lcd.println("");
  M5.Lcd.println("");
  M5.Lcd.println("");
  M5.Lcd.println("");
  M5.Lcd.println("");
  M5.Lcd.println("");
  M5.Lcd.println("");
  M5.Lcd.println(Tokyo.dateTime());

  last_emailed_at = Tokyo.now();
  delay(1500);
}

void loop() {
 espalexa.loop();
 delay(1);
  
  int isMotion = digitalRead(36);
  M5.Lcd.setCursor(60, 20, 4);
  M5.Lcd.println(isMotion);

  if( isMotionDetect ) {
    if(isMotion){
      if( isToggle == false ){
        send_mail_wrap();
      }
      isToggle = true;
    } else {
      isToggle = false;
    }
  }
  
  btn_a = digitalRead(BTN_A_PIN);
  
  if(prev_btn_a == BTN_OFF && btn_a == BTN_ON){
    prev_btn_a = BTN_ON;
    // ボタンAが押されたとき。今回は1回発光
    digitalWrite(LED_PIN, LED_ON);
    delay(500);
    digitalWrite(LED_PIN, LED_OFF);
    // LCDを消灯
    M5.Axp.ScreenBreath(0);
  }else if(prev_btn_a == BTN_ON && btn_a == BTN_ON){
    prev_btn_a = BTN_OFF;
    // ボタンAが押されたとき。今回は1回発光
    digitalWrite(LED_PIN, LED_ON);
    delay(500);
    digitalWrite(LED_PIN, LED_OFF);
    // LCDを点灯
    M5.Axp.ScreenBreath(200);
  }
  btn_b = digitalRead(BTN_B_PIN);
  if(prev_btn_b == BTN_OFF && btn_b == BTN_ON){
    prev_btn_b = BTN_ON;
    M5.Lcd.setCursor(60, 0, 1);
    M5.Lcd.println("");
    M5.Lcd.println("");
    M5.Lcd.println("");
    M5.Lcd.println("");
    M5.Lcd.println("");
    M5.Lcd.println("");
    Serial.println("MotionDetect ON");
    M5.Lcd.println("MotionDetect ON    ");
    isMotionDetect = true;
    // ボタンBが押されたとき。今回は1回発光
    digitalWrite(LED_PIN, LED_ON);
    delay(500);
    digitalWrite(LED_PIN, LED_OFF);
  } else if(prev_btn_b == BTN_ON && btn_b == BTN_ON){
    prev_btn_b = BTN_OFF;
    M5.Lcd.setCursor(60, 0, 1);
    M5.Lcd.println("");
    M5.Lcd.println("");
    M5.Lcd.println("");
    M5.Lcd.println("");
    M5.Lcd.println("");
    M5.Lcd.println("");
    Serial.println("MotionDetect OFF");
    M5.Lcd.println("MotionDetect OFF");
    isMotionDetect = false;
    delay(500);
  } 
}
