#include <LiquidCrystal.h>
#include <Keypad.h>

// Configuración de la pantalla LCD (RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(23, 22, 24, 25, 26, 27);

// Pines de botones
const int BTN_UP = A15;       
const int BTN_DOWN = A11;     
const int BTN_LEFT = A14;     
const int BTN_RIGHT = A12;    
const int BTN_CENTER = A13;   
const int BTN_SELECT_TONE = A10; 
const int BTN_STOP_ALARM = A9;

// Pines de LED y buzzer
const int LED_PINS[] = {A0, A1, A2, A3, A7};
const int BUZZER_PIN = A8;

// Tono y nombres de tonos
const int TONES[] = {262, 294, 330, 349, 392}; // Tonos: Do, Re, Mi, Fa, Sol
const char* TONE_NAMES[] = {"Do", "Re", "Mi", "Fa", "Sol"};

// Variables para control de tonos
int currentToneIndex = 0;
bool inToneSelection = false;
unsigned long toneStartTime = 0;
unsigned long lastToneChange = 0;
bool toneConfirmed = false;

// Variables para RTC y alarma
int hours = 12, minutes = 0, seconds = 0;             // Hora actual (0-23, 0-59, 0-59)
int alarmHours = 12, alarmMinutes = 0, alarmSeconds = 0; // Hora de la alarma
bool alarmActive = false;      // Alarma activa
bool showAlarm = false;        // Muestra RTC o alarma
bool countdownActive = false;  // Tiempo en que la alarma está activa

// Modo edición: el usuario presiona BTN_LEFT para alternar entre modo normal y modo edición  
bool editMode = false;         
// Si se edita la alarma, se usa esta variable; de lo contrario se edita el RTC
bool editingAlarm = false;     

/* 
Campos editables:
Campo 0: Horas (0-23)
Campo 1: Minutos (0-59)
Campo 2: Segundos (0-59)
Campo 3: Día (1 - max según mes; febrero = 28)
Campo 4: Mes (0-11 internamente; se muestra como mes+1)
Campo 5: Año
Campo 6: Día de la semana (0 a 6, donde 0=Domingo, 1=Lunes, …, 6=Sábado)
*/
int editField = 0;             
int selectedTone = 0;          // Tono asignado para la alarma
unsigned long lastUpdate = 0, alarmStartTime = 0;
int year = 2025;             
// Como ya no se utilizan los algoritmos de año bisiesto, fijamos febrero en 28 días.
int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
// Mes actual: se guarda de 0 a 11
int currentMonth = 0;        
int currentDayOfMonth = 1;   // Día del mes
// Día de la semana (editable manualmente, 0=Domingo, 1=Lunes,...,6=Sábado)
int currentDay = 0;          

// Arreglo para mostrar días de la semana
const char* daysOfWeek[] = {"Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"};

// Variables para parpadeo durante edición
bool blinkState = true;
unsigned long lastBlink = 0;

// ====================================================
// Funciones Auxiliares para Incremento/Decremento
// ====================================================
int incValue(int value, int maxVal, int minVal = 0) {
  int newVal = value + 1;
  return (newVal > maxVal) ? minVal : newVal;
}

int decValue(int value, int maxVal, int minVal = 0) {
  int newVal = value - 1;
  return (newVal < minVal) ? maxVal : newVal;
}

// ====================================================
// Funciones de Alarma y Tono
// ====================================================
void playTone(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
}

void stopTone() {
  noTone(BUZZER_PIN);
}

void handleToneSelection() {
  unsigned long currentMillis = millis();
  if (!inToneSelection) {
    inToneSelection = true;
    toneStartTime = currentMillis;
    currentToneIndex = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Selec. de Tono");
  }
  
  lcd.setCursor(0, 1);
  lcd.print("Tono: ");
  lcd.print(TONE_NAMES[currentToneIndex]);
  lcd.print("   ");
  
  playTone(TONES[currentToneIndex], 1000);
  
  if (currentMillis - toneStartTime >= 3000 && !toneConfirmed) {
    stopTone();
    selectedTone = currentToneIndex;
    toneConfirmed = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tono Conf.");
    delay(2000);
    inToneSelection = false;
    return;
  }
  
  if (digitalRead(BTN_SELECT_TONE) == LOW && currentMillis - lastToneChange >= 200) {
    lastToneChange = currentMillis;
    toneStartTime = currentMillis;
    stopTone();
    currentToneIndex = (currentToneIndex + 1) % 5;
    toneConfirmed = false;
  }
}

void startAlarm() {
  countdownActive = true;
  alarmStartTime = millis();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ALARMA ACTIVADA");
  lcd.setCursor(0, 1);
  lcd.print("Tono: ");
  lcd.print(TONE_NAMES[selectedTone]);
  
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PINS[i], (i == selectedTone) ? HIGH : LOW);
  }
  playTone(TONES[selectedTone], 120000 );
}

void stopAlarm() {
  countdownActive = false;
  stopTone();
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PINS[i], LOW);
  }
  lcd.clear();
}

// ====================================================
// Funciones de Cambio de Vista y Edición
// ====================================================
void toggleView() {
  showAlarm = !showAlarm;
  digitalWrite(A7, showAlarm ? HIGH : LOW);
  if (!editMode)
    editingAlarm = showAlarm;
}

void toggleEditMode() {
  editMode = !editMode;
  if (!editMode) editingAlarm = false;
}

void changeEditField() { 
  editField = (editField + 1) % 7; // 7 campos: 0-hora,1-min,2-sec,3-día,4-mes,5-año,6-día de la semana
}

void incrementTime() {
  if (!editMode) return;
  
  if (editingAlarm) {
    switch (editField) {
      case 0: alarmHours = incValue(alarmHours, 23); break;
      case 1: alarmMinutes = incValue(alarmMinutes, 59); break;
      case 2: alarmSeconds = incValue(alarmSeconds, 59); break;
      case 3: {
          int maxDays = daysInMonth[currentMonth];
          alarmSeconds = 0; // Opcional: reiniciar segundos de alarma
          currentDayOfMonth = incValue(currentDayOfMonth, maxDays, 1);
        }
        break;
      case 4: 
        currentMonth = (currentMonth + 1) % 12;
        if (currentDayOfMonth > daysInMonth[currentMonth])
          currentDayOfMonth = daysInMonth[currentMonth];
        break;
      case 5: 
        year++;
        break;
      case 6: 
        currentDay = incValue(currentDay, 6, 0);
        break;
    }
  } else { // Edición del RTC
    switch(editField) {
      case 0: hours = incValue(hours, 23); break;
      case 1: minutes = incValue(minutes, 59); break;
      case 2: seconds = incValue(seconds, 59); break;
      case 3: {
          int maxDays = daysInMonth[currentMonth];
          currentDayOfMonth = incValue(currentDayOfMonth, maxDays, 1);
        }
        break;
      case 4: 
        currentMonth = (currentMonth + 1) % 12;
        if (currentDayOfMonth > daysInMonth[currentMonth])
          currentDayOfMonth = daysInMonth[currentMonth];
        break;
      case 5: 
        year++;
        break;
      case 6: 
        currentDay = incValue(currentDay, 6, 0);
        break;
    }
  }
}

void decrementTime() {
  if (!editMode) return;
  
  if (editingAlarm) {
    switch (editField) {
      case 0: alarmHours = decValue(alarmHours, 23); break;
      case 1: alarmMinutes = decValue(alarmMinutes, 59); break;
      case 2: alarmSeconds = decValue(alarmSeconds, 59); break;
      case 3: {
          int maxDays = daysInMonth[currentMonth];
          currentDayOfMonth = decValue(currentDayOfMonth, maxDays, 1);
        }
        break;
      case 4: 
        currentMonth = (currentMonth - 1 + 12) % 12;
        if (currentDayOfMonth > daysInMonth[currentMonth])
          currentDayOfMonth = daysInMonth[currentMonth];
        break;
      case 5: 
        year--;
        break;
      case 6: 
        currentDay = decValue(currentDay, 6, 0);
        break;
    }
  } else {
    switch (editField) {
      case 0: hours = decValue(hours, 23); break;
      case 1: minutes = decValue(minutes, 59); break;
      case 2: seconds = decValue(seconds, 59); break;
      case 3: {
          int maxDays = daysInMonth[currentMonth];
          currentDayOfMonth = decValue(currentDayOfMonth, maxDays, 1);
        }
        break;
      case 4: 
        currentMonth = (currentMonth - 1 + 12) % 12;
        if (currentDayOfMonth > daysInMonth[currentMonth])
          currentDayOfMonth = daysInMonth[currentMonth];
        break;
      case 5: 
        year--;
        break;
      case 6: 
        currentDay = decValue(currentDay, 6, 0);
        break;
    }
  }
}

// ====================================================
// Actualización de la Pantalla LCD
// ====================================================
void updateLCD() {
  lcd.clear();
  // Primera fila: Mensaje de visualización
  if (showAlarm)
    lcd.print("Alarma Proyecto Final");
  else
    lcd.print("RTC Proyecto Final");
    
  // Si estamos en modo edición, mostramos el mensaje de modo edición en la primera fila
  if (editMode) {
    lcd.setCursor(12, 0);
    lcd.print("Edicion");
  }
  
  // Segunda fila: Hora (se muestra en formato 24h)
  lcd.setCursor(0, 1);
  if (showAlarm)
    lcd.print((alarmHours < 10)?"0":""),
    lcd.print(alarmHours),
    lcd.print(":"),
    lcd.print((alarmMinutes < 10)?"0":""),
    lcd.print(alarmMinutes),
    lcd.print(":"),
    lcd.print((alarmSeconds < 10)?"0":""),
    lcd.print(alarmSeconds);
  else
    lcd.print((hours < 10)?"0":""),
    lcd.print(hours),
    lcd.print(":"),
    lcd.print((minutes < 10)?"0":""),
    lcd.print(minutes),
    lcd.print(":"),
    lcd.print((seconds < 10)?"0":""),
    lcd.print(seconds);

  // Tercera fila: Fecha (día/mes/año)
  lcd.setCursor(0, 2);
  lcd.print("Fecha: ");
  if (currentDayOfMonth < 10) lcd.print("0");
  lcd.print(currentDayOfMonth);
  lcd.print("/");
  if (currentMonth+1 < 10) lcd.print("0");
  lcd.print(currentMonth+1);
  lcd.print("/");
  lcd.print(year);
  
  // Cuarta fila: Día de la semana
  lcd.setCursor(0, 3);
  lcd.print("Dia: ");
  lcd.print(daysOfWeek[currentDay]);
}

// ====================================================
// Configuración Inicial
// ====================================================
void setup() {
  Serial.begin(9600);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_CENTER, INPUT_PULLUP);
  pinMode(BTN_SELECT_TONE, INPUT_PULLUP);
  pinMode(BTN_STOP_ALARM, INPUT_PULLUP );
  
  // Configurar LED A7 para indicar modo alarma
  pinMode(A7, OUTPUT);
  digitalWrite(A7, LOW);

  // Configurar LEDs individuales
  for (int i = 0; i < 5; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }

  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("   Proyecto Final  ");
  lcd.setCursor(0, 1);
  lcd.print(" RTC & Alarma");
  delay(2000);
  lcd.clear();

  // Día de la semana inicial (editable manualmente)
  currentDay = 0; // Domingo
  
  // Configuración del buzzer
  pinMode(BUZZER_PIN, OUTPUT);
}
  
// ====================================================
// Bucle Principal
// ====================================================
void loop() {
  unsigned long currentMillis = millis();
  bool needLCDUpdate = false;

  // Lectura de botones físicos
  if (digitalRead(BTN_CENTER) == LOW) {
    delay(200);
    toggleView();
    needLCDUpdate = true;
  }
  if (digitalRead(BTN_LEFT) == LOW) {
    delay(200);
    toggleEditMode();
    needLCDUpdate = true;
  }
  if (digitalRead(BTN_RIGHT) == LOW) {
    delay(200);
    changeEditField();
    needLCDUpdate = true;
  }
  if (digitalRead(BTN_UP) == LOW) {
    delay(200);
    incrementTime();
    needLCDUpdate = true;
  }
  if (digitalRead(BTN_DOWN) == LOW) {
    delay(200);
    decrementTime();
    needLCDUpdate = true;
  }
  if(digitalRead(BTN_STOP_ALARM) == LOW){
    delay(200);
    stopTone();
  }

  // Parpadeo en modo edición (cada 500 ms)
  if (currentMillis - lastBlink >= 500) {
    lastBlink = currentMillis;
    blinkState = !blinkState;
    if (editMode) needLCDUpdate = true;
  }

  // Actualización del RTC cada segundo (solo en modo normal)
  if (!editMode && currentMillis - lastUpdate >= 1000) {
    lastUpdate = currentMillis;
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours++;
        if (hours >= 24) {
          hours = 0;
          // Al cambiar de fecha automáticamente (se puede dejar al criterio del usuario)
          currentDayOfMonth++;
          if (currentDayOfMonth > daysInMonth[currentMonth]) {
            currentDayOfMonth = 1;
            currentMonth = (currentMonth + 1) % 12;
            // No se actualiza año bisiesto; siempre febrero = 28
            if (currentMonth == 0) year++;
          }
        }
      }
    }
    // Se podría actualizar el día de la semana automáticamente, pero se requiere edición manual.
    needLCDUpdate = true;
    
    // Verificar si la alarma debe sonar
    if (!editMode && !countdownActive &&
        hours == alarmHours &&
        minutes == alarmMinutes &&
        seconds == alarmSeconds) {
      startAlarm();
    }
  }
  
  // Actualizar LCD si es necesario
  if (needLCDUpdate) {
    updateLCD();
  }
  
  // Detener la alarma después de 60 segundos
  if (countdownActive && currentMillis - alarmStartTime >= 100000) {
    stopAlarm();
    updateLCD();
  }
  
  // Manejo del botón de selección de tono (si no hay cuenta regresiva)
  if (digitalRead(BTN_SELECT_TONE) == LOW && !countdownActive) {
    delay(200);
    handleToneSelection();
    updateLCD();
  }
  
  if (inToneSelection) {
    handleToneSelection();
    return;
  }
  
}
