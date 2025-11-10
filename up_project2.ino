// === Mess Seat Reservation System ===
// 10 Seat Buttons + 1 Summary/Reset Button (D12)
// Shows Seat Map only in Summary, Long Press on D12 resets all

const int seatPins[10] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; // Seat buttons
const int summaryPin = 12; // Summary + Reset button

bool seatStatus[10] = {0};      // 0 = Empty, 1 = Occupied
bool lastSeatState[10] = {1};
bool lastSummaryState = 1;

unsigned long pressStartTime = 0;
bool buttonHeld = false;

void setup() {
  Serial.begin(9600);
  Serial.println("=== Mess Seat Reservation System (10 Seats + Summary Button) ===");
  Serial.println("Press seat buttons to toggle occupancy.\nShort press D12 = summary, Long press = reset all.\n");

  for (int i = 0; i < 10; i++) {
    pinMode(seatPins[i], INPUT_PULLUP);
  }
  pinMode(summaryPin, INPUT_PULLUP);

  printStatus();
}

void loop() {
  // --- Check Summary Button ---
  bool summaryState = digitalRead(summaryPin);

  // Button pressed down
  if (summaryState == LOW && lastSummaryState == HIGH) {
    pressStartTime = millis();
    buttonHeld = false;
  }

  // Button held down
  if (summaryState == LOW && (millis() - pressStartTime > 2000) && !buttonHeld) {
    resetSeats();
    buttonHeld = true;
  }

  // Button released
  if (summaryState == HIGH && lastSummaryState == LOW && !buttonHeld) {
    showSummary();
  }

  lastSummaryState = summaryState;

  // --- Check Each Seat Button ---
  for (int i = 0; i < 10; i++) {
    bool currentState = digitalRead(seatPins[i]);
    if (currentState == LOW && lastSeatState[i] == HIGH) {
      seatStatus[i] = !seatStatus[i];
      printStatus();
      delay(200);
    }
    lastSeatState[i] = currentState;
  }
}

// --- Display Full Seat Status ---
void printStatus() {
  int occupied = 0;
  for (int i = 0; i < 10; i++) if (seatStatus[i]) occupied++;
  int vacant = 10 - occupied;

  Serial.println("\n--- Current Seat Status ---");
  for (int i = 0; i < 10; i++) {
    Serial.print("Seat ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(seatStatus[i] ? "Occupied" : "Empty");
  }

  Serial.print("Occupied: ");
  Serial.print(occupied);
  Serial.print(" | Vacant: ");
  Serial.println(vacant);

  if (occupied == 10) {
    Serial.println("🚫 Mess Full!");
  } else if (occupied >= 9) {
    Serial.println("⚠️  Almost full!");
  }

  Serial.println("---------------------------");
}

// --- Summary Display ---
void showSummary() {
  int occupied = 0;
  for (int i = 0; i < 10; i++) if (seatStatus[i]) occupied++;
  int vacant = 10 - occupied;

  Serial.println("\n=== Seat Summary ===");
  Serial.print("Occupied: ");
  Serial.print(occupied);
  Serial.print(" | Vacant: ");
  Serial.println(vacant);

  Serial.print("[");
  for (int i = 0; i < 10; i++) Serial.print(seatStatus[i] ? "🟥" : "🟩");
  Serial.println("]");
  Serial.println("=====================");
}

// --- Reset All Seats ---
void resetSeats() {
  for (int i = 0; i < 10; i++) seatStatus[i] = 0;
  Serial.println("\n🔄 All seats reset manually by long press!");
  printStatus();
}
