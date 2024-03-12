//10:b5:88:56:31:08

#include <Arduino.h>
#include <PS4Controller.h>  //PS4コントローラーとの通信用ライブラリ

#include <vector>

// put function declarations here:

//GPIO 0,1,3,14,15,34,35,36,39 は使用しない
/*
motor: 6
move: 2(left, right)
high: 1
tools: 3(low,low, high)
*/

/*
  #defineでESPのピンに仮の名前を付ける
  プログラム中での可読性が上がり、デバッグ(いわゆる動作確認)

  MOTORとついてるやつは速度制御
  DIRとついてるやつは回転方向
*/

// 走行用モーター
#define RMOTOR A4
#define RDIR 35
#define LMOTOR A19
#define LDIR 33

// 低い方の機構(展開用)
#define LOWMOTOR A17
#define LOWMTDIR 25
// 低い方の機構(蓋開ける用)
#define LOWOPEN A15
#define LOWOPDIR 14

// 高い方の機構
#define SHOOTMOTOR A13
#define SHOOTDIR 13

// 機構類を上に持ち上げる用
#define UPDWNMOTOR A12
#define UPDWNDIR 4

void setup() {
  Serial.begin(115200);// PCとの通信速度

  PS4.begin("10:b5:88:56:31:08");// コントローラーとペアリング
  Serial.println("connected");

/*
  pinMode(): 特定のピンを入力用にするか出力用にするかの設定
  ledcSetup(): PWM制御をどのように行うかの設定
  ledcAttachPin(): ledcSetup()の設定をピンに割り当てる
*/

  pinMode(RMOTOR, OUTPUT);// pinMode(ピン番号, 入出力);
  pinMode(RDIR, OUTPUT);
  pinMode(LMOTOR, OUTPUT);
  pinMode(LDIR, OUTPUT);

  pinMode(LOWMOTOR, OUTPUT);
  pinMode(LOWMTDIR, OUTPUT);
  pinMode(LOWOPEN, OUTPUT);
  pinMode(LOWOPDIR, OUTPUT);

  pinMode(SHOOTMOTOR, OUTPUT);
  pinMode(SHOOTDIR, OUTPUT);

  pinMode(UPDWNMOTOR, OUTPUT);
  pinMode(UPDWNDIR, OUTPUT);
  
  ledcSetup(0, 490, 8);// ledcSetup(チャンネル, 周波数, 分解能);
  ledcSetup(1, 490, 8);
  ledcSetup(2, 490, 8);
  ledcSetup(3, 490, 8);
  ledcSetup(4, 490, 8);
  ledcSetup(5, 490, 8);

  ledcAttachPin(RMOTOR,0);// ledcAttachPin(ピン番号, チャンネル);
  ledcAttachPin(LMOTOR,1);

  ledcAttachPin(LOWMOTOR,2);
  ledcAttachPin(LOWOPEN,3);

  ledcAttachPin(SHOOTMOTOR,4);

  ledcAttachPin(UPDWNMOTOR,5);
}

// 反転するかの判定
bool isDIR(int value) {
  if (value < 0) {
    return true;
  } else {
    return false;
  }
}

// 受け取ったモーターを止める
void stopMotor(std::vector<int> motor) {
  for (auto& m:motor) {
    ledcWrite(m, 0);
  }
  for (auto& m:motor) {
    motor.pop_back();
  }
}

// 前後移動
void forward(int stick) {
  int speed = map(abs(stick), 0, 127, 0, 255);// 数値の調整
  if (isDIR(stick)) {// isDIRがtrueだったら反転(後退)
    digitalWrite(LDIR, HIGH);
    digitalWrite(RDIR, HIGH);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
  } else {// 前進
    digitalWrite(LDIR, LOW);
    digitalWrite(RDIR, LOW);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
  }
}

//回転
void rotation(int stick) {
  int speed = map(abs(stick), 0, 127, 0, 255);
  if (isDIR(stick)) {// isDIRがtrueだったら反時計回りに回転
    digitalWrite(LDIR, HIGH);
    digitalWrite(RDIR, LOW);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
  } else {// 時計回りに回転
    digitalWrite(LDIR, LOW);
    digitalWrite(RDIR, HIGH);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
  }
}

//機構の上下
void upDown(bool dir) {
  if (dir) {// dirがtrueだったら下がる
    digitalWrite(UPDWNDIR, HIGH);
    ledcWrite(5, 127);
  } else {// 上がる
    digitalWrite(UPDWNDIR, LOW);
    ledcWrite(5, 127);
  }
}

// 低い方の機構の展開、格納
void lowToolDeploy(bool dir) {
  if (dir) {// dirがtrueだったら格納する
    digitalWrite(LOWMTDIR, HIGH);
    ledcWrite(2, 127);
  } else {// 展開する
    digitalWrite(LOWMTDIR, LOW);
    ledcWrite(2, 127);
  }
}

// 低い方の機構の玉放出
void lowToolOpen(bool dir) {
  if (dir) {//蓋を閉じる
    digitalWrite(LOWOPDIR, HIGH);
    ledcWrite(3, 127);
  } else {//玉放出
    digitalWrite(LOWOPDIR, LOW);
    ledcWrite(3, 127);
  }
}

// 高い方の機構の玉発射
// TODO
void highToolShoot() {
  digitalWrite(SHOOTDIR, LOW);
  ledcWrite(4, 200);
}

std::vector<int> motor;

void loop() {
  if (PS4.isConnected()) {// コントローラーとの接続を確認
    if (PS4.RStickY() >= 10 || PS4.RStickY() <= -10) { // 右スティックが上下に倒されたら
      Serial.printf("Right Stick y at  %d\n", PS4.RStickY());
      forward(PS4.RStickY());// 前後移動
    } else {// 倒されなかったら
      motor = {RMOTOR, LMOTOR};// 左右モーターを止める
      stopMotor(motor);
    };
    if (PS4.RStickX() >= 10 || PS4.RStickX() <= -10) { // 右スティックが左右に倒されたら
      Serial.printf("Right Stick x at  %d\n", PS4.RStickX());
      rotation(PS4.RStickX());// 回転
    } else {
      motor = {RMOTOR, LMOTOR};// 左右モーターを止める
      stopMotor(motor);
    };
    if (PS4.Left()) { // 左ボタンが押されたら
      Serial.println("Left Button");
      lowToolDeploy(false);// 低い方の機構の展開
    } else {
      motor = {LOWMOTOR};// 展開用モーターを止める
      stopMotor(motor);
    };
    if (PS4.Right()) { // 右ボタンが押されたら
      Serial.println("Right Button");
      lowToolDeploy(true);// 低い方の機構の格納
    } else {
      motor = {LOWMOTOR};// 展開用モーターを止める
      stopMotor(motor);
    };
    if (PS4.Up()) { // 上ボタンが押されたら
      Serial.println("Up Button");
      upDown(false);// 上げる
    } else {
      motor = {UPDWNMOTOR};// 上下用モーターを止める
      stopMotor(motor);
    };
    if (PS4.Down()) { // 下ボタンが押されたら
      Serial.println("Down Button");
      upDown(true);// 下げる
    } else {
      motor = {UPDWNMOTOR};// 上下用モーターを止める
      stopMotor(motor);
    };
    if (PS4.Triangle()) {// 三角ボタンが押されたら
      Serial.println("Triangle Button");
      highToolShoot();// 高い方の機構の玉発射
    } else {
      motor = {SHOOTMOTOR};// 発射用モーターを止める
      stopMotor(motor);
    }
  }
}

  // put your main code here, to run repeatedly:
/*  if (PS4.isConnected()) {
    if (PS4.Right()) Serial.println("Right Button");
    if (PS4.Down()) Serial.println("Down Button");
    if (PS4.Left()) Serial.println("Left Button");
    if (PS4.Up()) Serial.println("Up Button");

    if (PS4.Circle()) Serial.println("Circle Button");
    if (PS4.Cross()) Serial.println("Cross Button");
    if (PS4.Square()) Serial.println("Square Button");
    if (PS4.Triangle()) Serial.println("Triangle Button");

    if (PS4.UpRight()) Serial.println("Up Right");
    if (PS4.DownRight()) Serial.println("Down Right");
    if (PS4.DownLeft()) Serial.println("Down Left");
    if (PS4.UpLeft()) Serial.println("Up Left");

    if (PS4.L1()) Serial.println("L1 Button");
    if (PS4.R1()) Serial.println("R1 Button");

    if (PS4.L2()) { 
      Serial.printf("L2 Button at %d\n", PS4.L2Value());
    };
    if (PS4.R2()) { 
      Serial.printf("R2 Button at %d\n", PS4.R2Value());
    };

    if (PS4.L3()) Serial.println("L3 Button");
    if (PS4.R3()) Serial.println("R3 Button");

    if (PS4.Share()) Serial.println("Share Button");
    if (PS4.Options()) Serial.println("Options Button");

    if (PS4.PSButton()) Serial.println("PS Button");
    if (PS4.Touchpad()) Serial.println("Touch Pad Button");

    if (PS4.LStickX() >= 10 || PS4.LStickX() <= -10) { 
      Serial.printf("Left Stick x at  %d\n", PS4.LStickX());
    };

    if (PS4.LStickY() >= 10 || PS4.LStickY() <= -10) { 
      //forward(PS4.LStickY());
      Serial.printf("Left Stick y at  %d\n", PS4.LStickY());
    };

    if (PS4.RStickX() >= 10 || PS4.RStickX() <= -10) { 
      Serial.printf("Right Stick x at  %d\n", PS4.RStickX());
    };

    if (PS4.RStickY() >= 10 || PS4.RStickY() <= -10) { 
      Serial.printf("Right Stick y at  %d\n", PS4.RStickY());
      forward(PS4.RStickY());
    };

    if (PS4.Charging()) Serial.println("The controller is charging.");
    if (PS4.Audio()) Serial.println("The controller has headphones attached.");
    if (PS4.Mic()) Serial.println("The controller has mic attached.");

    Serial.printf("Battery Level : %d\n", PS4.Battery());

    Serial.println();

    delay(1000);
  }
}
*/