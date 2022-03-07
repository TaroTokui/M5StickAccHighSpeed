#include <ArduinoOSCWiFi.h>
#include <M5StickCPlus.h>

#define SAMPLE_PERIOD 1    // サンプリング間隔(ミリ秒)
#define BATCH_COUNT 10  // ひとまとめで送るデータ数、変更しないこと

const char *ssid = "";
const char *pass = "";
const int bind_port = 10001;
const int send_port = 10002;

IPAddress ip(192, 168, 11, 101);           // for fixed IP Address
IPAddress gateway(192,168, 11, 1);        //
IPAddress subnet(255, 255, 255, 0);      //
const String ipClient = "192.168.11.21";      // to PC

int flag;

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
bool btn_state = false;

void setup() {

  // initialize the M5StickC object
  M5.begin();
  M5.Axp.ScreenBreath(10); // 画面の明るさ7〜１2
  M5.Lcd.setRotation(3); // 画面を横向きにする
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(5, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Press M5 button");

  // button & led
  pinMode(BTN_A_PIN, INPUT_PULLUP);
  pinMode(BTN_B_PIN, INPUT_PULLUP);
  pinMode(LED_PIN,   OUTPUT);
  digitalWrite(LED_PIN, LED_OFF);

  // 加速度センサ
  M5.IMU.Init();
  M5.IMU.SetAccelFsr(M5.IMU.AFS_4G);
  
  // WiFi stuff
  WiFi.begin(ssid, pass);
  WiFi.config(ip, gateway, subnet);

  // 受信したOSCパケットを直接変数にバインドします
  flag = 0;
  OscWiFi.subscribe(bind_port, "/test", flag);

}


void loop() {
  OscWiFi.update(); // 自動的に送受信するために必須

  // check button
  btn_a = digitalRead(BTN_A_PIN);
  btn_b = digitalRead(BTN_B_PIN);
  
  if(prev_btn_a == BTN_OFF && btn_a == BTN_ON)
  {
    btn_state = !btn_state;
    delay(200); // チャタリング防止

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(5, 10);
    if(btn_state){
      digitalWrite(LED_PIN, LED_ON);
      M5.Lcd.println("Sending OSC Data...");
    }else{
      digitalWrite(LED_PIN, LED_OFF);
      M5.Lcd.println("Press M5 button");
    }

  }
  
  float ax[BATCH_COUNT], ay[BATCH_COUNT], az[BATCH_COUNT];  // 加速度データを読み出す変数
  
  if(btn_state)
  {
    for(int i=0; i<BATCH_COUNT; i++)
    {
      M5.IMU.getAccelData(&ax[i],&ay[i],&az[i]);
      delay(SAMPLE_PERIOD);    
    }

    OscWiFi.send(ipClient, send_port, "/x", ax[0], ax[1], ax[2], ax[3], ax[4], ax[5], ax[6], ax[7], ax[8], ax[9]);
    OscWiFi.send(ipClient, send_port, "/y", ay[0], ay[1], ay[2], ay[3], ay[4], ay[5], ay[6], ay[7], ay[8], ay[9]);
    OscWiFi.send(ipClient, send_port, "/z", az[0], az[1], az[2], az[3], az[4], az[5], az[6], az[7], az[8], az[9]);
  }else
  {
    delay(SAMPLE_PERIOD);
  }
}
