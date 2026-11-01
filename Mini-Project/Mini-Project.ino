#include <Arduino.h>

/*
  Proyek: Sistem Kendali Motor DC & Blinking LED dengan Emergency Stop
  Deskripsi: 
    - Emergency Stop via Push Button (Pin 2, INPUT_PULLUP, Active-LOW)
    - Motor DC dikendalikan via Transistor NPN melalui PWM Pin 3
    - Blinking LED non-blocking menggunakan millis() pada Pin 13
*/

// ==========================================
// 1. DEFINISI KONSTANTA PIN & NILAI KONTROL
// ==========================================
const int PIN_EMERGENCY_BUTTON = 2;   // Tombol Emergency Stop (INPUT_PULLUP)
const int PIN_MOTOR_PWM        = 3;   // Basis Transistor Motor DC (Pin PWM ~3)
const int PIN_LED_STATUS       = 13;  // LED indikator onboard / eksternal

const unsigned long LED_INTERVAL = 1000; // Interval kedip: 1000 ms (1 detik)
const int MOTOR_SPEED_NORMAL     = 150;  // Duty cycle PWM motor (rentang 0 - 255)

// ==========================================
// 2. VARIABEL GLOBAL (STATE MANAGEMENT)
// ==========================================
unsigned long previousMillisLED = 0; // Menyimpan timestamp terakhir kali LED berganti status
int ledState = LOW;                  // Status keluaran LED saat ini (LOW = Mati, HIGH = Nyala)

void setup() {
  // Inisialisasi pin tombol darurat dengan internal pull-up resistor
  pinMode(PIN_EMERGENCY_BUTTON, INPUT_PULLUP);

  // Inisialisasi pin output
  pinMode(PIN_MOTOR_PWM, OUTPUT);
  pinMode(PIN_LED_STATUS, OUTPUT);

  // Kondisi awal (Safe State): Semua aktuator mati saat startup
  digitalWrite(PIN_LED_STATUS, LOW);
  analogWrite(PIN_MOTOR_PWM, 0);
}

void loop() {
  // ========================================================
  // PRIORITAS UTAMA: PEMBACAAN EMERGENCY STOP DI AWAL LOOP
  // ========================================================
  int emergencyState = digitalRead(PIN_EMERGENCY_BUTTON);

  // Tombol ditekan = logika LOW (karena memakai INPUT_PULLUP)
  if (emergencyState == LOW) {
    // Matikan seluruh aktuator seketika (Failsafe Instant Cut-off)
    analogWrite(PIN_MOTOR_PWM, 0);
    digitalWrite(PIN_LED_STATUS, LOW);

    // Reset status LED agar siklus rapi saat emergency dilepas
    ledState = LOW;

    // 'return' langsung mengakhiri iterasi loop() saat ini.
    // Seluruh kode operasional normal di bawahnya DILEWATI seketika.
    return;
  }

  // ========================================================
  // OPERASI NORMAL: HANYA DIJALANKAN JIKA TOMBOL TIDAK DITEKAN
  // ========================================================

  // 1. Operasikan Motor DC pada kecepatan konstan (PWM 150)
  analogWrite(PIN_MOTOR_PWM, MOTOR_SPEED_NORMAL);

  // 2. Kontrol Kedip LED Non-Blocking (Wajib Menggunakan millis())
  unsigned long currentMillis = millis();

  // Evaluasi selisih waktu: apakah sudah lewat 1000 ms?
  if (currentMillis - previousMillisLED >= LED_INTERVAL) {
    // Catat waktu perpindahan status saat ini
    previousMillisLED = currentMillis;

    // Toggle status logika LED (Jika LOW ubah jadi HIGH, jika HIGH ubah jadi LOW)
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // Terapkan status baru ke pin fisik LED
    digitalWrite(PIN_LED_STATUS, ledState);
  }
}