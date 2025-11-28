// ======================================================
// === CONFIGURAÇÃO DO MOTOR ===
// ======================================================

#define enL 17
#define inLA 32
#define inLB 33

#define enR 16
#define inRA 25
#define inRB 26

int motorSpeed = 200; // velocidade base (0–255)

#define PWM_CH_L 0
#define PWM_CH_R 1

// ======================================================
// === SERVO E SENSORES ===
// ======================================================

#include <ESP32Servo.h>

Servo meuServo;

int pinoServo = 5;

// Sensores de chama
int sensor1 = 23; // esquerda
int sensor2 = 22; // centro
int sensor3 = 21; // direita

int posAtual = 90;

// ======================================================
// === BOMBA (RELÉ) ===
// ======================================================

int pinoRele = 18;

unsigned long tempoInicioBomba = 0;
int bombaLigada = 0;
unsigned long limiteTempoBomba = 10000; // 10s

// ======================================================
// === CONFIGURAÇÕES INICIAIS ===
// ======================================================

void setup() {
  Serial.begin(115200);

  pinMode(inLA, OUTPUT);
  pinMode(inLB, OUTPUT);
  pinMode(inRA, OUTPUT);
  pinMode(inRB, OUTPUT);

  // PWM motores
  ledcSetup(PWM_CH_L, 5000, 8);
  ledcAttachPin(enL, PWM_CH_L);

  ledcSetup(PWM_CH_R, 5000, 8);
  ledcAttachPin(enR, PWM_CH_R);

  ledcWrite(PWM_CH_L, 0);
  ledcWrite(PWM_CH_R, 0);

  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);

  pinMode(pinoRele, OUTPUT);
  digitalWrite(pinoRele, LOW);

  meuServo.attach(pinoServo, 500, 2400);
  meuServo.write(posAtual);
}

// ======================================================
// === FUNÇÕES DO MOTOR ===
// ======================================================

void freioSuave() {
  for (int i = motorSpeed; i >= 0; i -= 8) {
    ledcWrite(PWM_CH_L, i);
    ledcWrite(PWM_CH_R, i);
    delay(10);
  }
}

void stopMotors() {
  freioSuave();
  digitalWrite(inLA, LOW);
  digitalWrite(inLB, LOW);
  digitalWrite(inRA, LOW);
  digitalWrite(inRB, LOW);
}

void moveForward() {
  digitalWrite(inLA, HIGH);
  digitalWrite(inLB, LOW);
  digitalWrite(inRA, HIGH);
  digitalWrite(inRB, LOW);
}

void moveBackward() {
  digitalWrite(inLA, LOW);
  digitalWrite(inLB, HIGH);
  digitalWrite(inRA, LOW);
  digitalWrite(inRB, HIGH);
}

void turnLeftSmooth() {
  digitalWrite(inLA, HIGH);
  digitalWrite(inLB, LOW);
  digitalWrite(inRA, HIGH);
  digitalWrite(inRB, LOW);

  ledcWrite(PWM_CH_L, motorSpeed * 0.40);
  ledcWrite(PWM_CH_R, motorSpeed);
}

void turnRightSmooth() {
  digitalWrite(inLA, HIGH);
  digitalWrite(inLB, LOW);
  digitalWrite(inRA, HIGH);
  digitalWrite(inRB, LOW);

  ledcWrite(PWM_CH_L, motorSpeed);
  ledcWrite(PWM_CH_R, motorSpeed * 0.40);
}

void recuarRapido() {
  digitalWrite(inLA, LOW);
  digitalWrite(inLB, HIGH);
  digitalWrite(inRA, LOW);
  digitalWrite(inRB, HIGH);

  ledcWrite(PWM_CH_L, 180);
  ledcWrite(PWM_CH_R, 180);
}

// ======================================================
// === VELOCIDADE ADAPTATIVA ===
// ======================================================

void velocidadeAdaptativa(int sensoresAtivos) {
  if (sensoresAtivos == 1) {
    ledcWrite(PWM_CH_L, 200);
    ledcWrite(PWM_CH_R, 200);
  }
  else if (sensoresAtivos == 2) {
    ledcWrite(PWM_CH_L, 150);
    ledcWrite(PWM_CH_R, 150);
  }
  else if (sensoresAtivos == 3) {
    ledcWrite(PWM_CH_L, 100);
    ledcWrite(PWM_CH_R, 100);
  }
}

// ======================================================
// === SERVO ===
// ======================================================

void moverServoLento(int alvo) {
  if (posAtual < alvo) posAtual++;
  else if (posAtual > alvo) posAtual--;
  meuServo.write(posAtual);
  delay(5);
}

// ======================================================
// === LOOP PRINCIPAL ===
// ======================================================

void loop() {
  int S1 = digitalRead(sensor1);
  int S2 = digitalRead(sensor2);
  int S3 = digitalRead(sensor3);

  int sensoresAtivos = S1 + S2 + S3;

  // ============================================================
  // === BOMBA COM LIMITE DE TEMPO ===
  // ============================================================
  if (sensoresAtivos > 0) {
    if (!bombaLigada) {
      bombaLigada = 1;
      tempoInicioBomba = millis();
      digitalWrite(pinoRele, HIGH);
    }
    else {
      if (millis() - tempoInicioBomba > limiteTempoBomba) {
        Serial.println("⛔ Bomba desligada por limite de tempo!");
        digitalWrite(pinoRele, LOW);
      }
    }
  }
  else {
    bombaLigada = 0;
    digitalWrite(pinoRele, LOW);
  }

  // ============================================================
  // === SERVO DIRECIONANDO PARA O FOGO ===
  // ============================================================

  int soma = 0, contador = 0;

  if (S1) { soma += 0; contador++; }
  if (S2) { soma += 90; contador++; }
  if (S3) { soma += 180; contador++; }

  int alvo = (contador > 0) ? soma / contador : 90;
  moverServoLento(alvo);

  // ============================================================
  // === RECUAR SE MUITO PERTO (3 SENSORES) ===
  // ============================================================

  if (sensoresAtivos == 3) {
    Serial.println("🔥 MUITO PERTO! RECUANDO!");
    recuarRapido();
    delay(800);
    stopMotors();
    return;
  }

  // ============================================================
  // === VELOCIDADE ADAPTATIVA ===
  // ============================================================

  if (sensoresAtivos > 0) velocidadeAdaptativa(sensoresAtivos);

  // ============================================================
  // === LÓGICA DE MOVIMENTO ===
  // ============================================================

  // Nenhum fogo → procurar
  if (sensoresAtivos == 0) {
    turnLeftSmooth();
    return;
  }

  // Apenas esquerda
  if (S1 && !S2 && !S3) {
    turnLeftSmooth();
    return;
  }

  // Apenas direita
  if (!S1 && !S2 && S3) {
    turnRightSmooth();
    return;
  }

  // Apenas centro
  if (!S1 && S2 && !S3) {
    moveForward();
    return;
  }

  // Esquerda + centro
  if (S1 && S2 && !S3) {
    turnLeftSmooth();
    return;
  }

  // Direita + centro
  if (!S1 && S2 && S3) {
    turnRightSmooth();
    return;
  }
}
