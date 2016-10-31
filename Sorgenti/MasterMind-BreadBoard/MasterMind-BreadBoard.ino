// simboli per combinazione
#define NUM_SIMBOLI 4
// numero di caratteri per combinazione
#define NUM_CARATTERI 4
// tentativi totali (difficoltà)
#define TOT_TENTATIVI 10

/* INPUT OUTPUT */

/* DISPLAY */
#include <LiquidCrystal.h>
// Dimensioni del display
#define ROW 2
#define COL 16
// Inizializzazione display
// RS ENABLE D4~D7
LiquidCrystal lcd(2, 3 ,4, 5, 6, 7);

/* BOTTONI */
const byte PIN_bottoni[NUM_SIMBOLI] = {9, 10, 11, 12};

/* EFFETTI SONORI */
const byte PIN_buzzer = 13;
#define VINTO 1
#define PERSO 2
#define BIP 3

void setup() {
  for (short a = 0; a < NUM_SIMBOLI; a++)
    pinMode(PIN_bottoni[a], INPUT_PULLUP);

  lcd.begin(COL, ROW);

  lcd.print("   Mastermind!  ");
  lcd.setCursor(0, 1);
  lcd.print(" by GLGPRograms");

  randomSeed(analogRead(A0));

  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print(" premi un tasto ");

  // Premi un tasto per iniziare
}

void loop() {
  byte sequenza[NUM_CARATTERI], sequenza_ins[NUM_CARATTERI];

  while ( leggiBottone() == 255);

  // Genera combinazione
  lcd.clear();
  lcd.print("  sto pensando  ");
  
  lcd.setCursor(4, 1);
  
  for (int c = 0; c < NUM_CARATTERI; c++) {
    sequenza[c] = random(NUM_SIMBOLI);
    lcd.print(". ");
    delay(600);
  }

  for (byte tentativi = TOT_TENTATIVI; tentativi > 0; tentativi--) {
    lcd.clear();
    lcd.print("tentativo ");
    lcd.print(tentativi);
    lcd.print("/");
    lcd.print(TOT_TENTATIVI);
    lcd.setCursor(0, 1);

    for (byte c = 0; c < NUM_CARATTERI; c++) {

      // Lettura carattere inserito
      do {
        sequenza_ins[c] = leggiBottone();
      }
      while ( sequenza_ins[c] == 255);

      // Stampa su LCD il carattere
      lcd.print(assegna_carattere(sequenza_ins[c]));
      lcd.print(" ");
    }

    lcd.print(" ");
    
    if ( comparaSequenza(sequenza_ins, sequenza) == true) {
      // Hai vinto
      lcd.setCursor(0, 0);
      lcd.print("  Hai vinto!!!  ");
      riproduciEffetto(VINTO);
      delay(2000);
      lcd.setCursor(0, 1);
      lcd.print(" premi un tasto ");
      break;
    }
    else if (tentativi == 1) {
      lcd.clear();
      lcd.print("  Hai perso!!!  ");
      lcd.setCursor(0, 1);
      lcd.print("  Era ");
      for(byte c = 0; c < NUM_CARATTERI; c++) {
        lcd.print(assegna_carattere(sequenza[c]));
      }
      
      riproduciEffetto(PERSO);
      delay(2000);
      lcd.setCursor(0, 1);
      lcd.print(" premi un tasto ");
      break;
    }
    else 
      while ( leggiBottone() == 255);

  }

}

byte leggiBottone() {
  byte letturaAttuale = 0x00;
  static byte letturaPrecedente;

  delay(30); // Pausa debouncing

  for (byte l = 0; l < NUM_SIMBOLI; l++) {
    // PULL-UP, letture invertite!
    letturaAttuale |= (!digitalRead(PIN_bottoni[l]) << l);
  }

  for (byte l = 0; l < NUM_SIMBOLI; l++) {
    if ( (letturaAttuale & _BV(l)) && (~letturaPrecedente & _BV(l)) ) {
      letturaPrecedente = letturaAttuale;
      riproduciEffetto(BIP);
      return l;
    }
  }

  letturaPrecedente = letturaAttuale;
  return 255;
}

bool comparaSequenza(byte *sequenza_ins, byte *sequenza) {
  byte corrispondenza = 0x00;
  byte elementi = 0;

  // Elementi giusti al posto giusto
  for (byte c = 0; c < NUM_CARATTERI; c++) {
    if (sequenza_ins[c] == sequenza[c]) {
      elementi++;
      lcd.print("O ");
      corrispondenza |= _BV(c);
    }
  }

  if (elementi == NUM_CARATTERI)
    return true;

  // Elementi giusti al posto sbagliato
  for (byte m = 0; m < NUM_CARATTERI; m++) {
    
    if (sequenza[m] == sequenza_ins[m])
      continue;
      
    for (byte n = 0; n < NUM_CARATTERI; n++) {
      // Se è già stato controllato, salta
      if (corrispondenza & _BV(n))  continue;
      else if (sequenza_ins[m] == sequenza[n]) {
        elementi++;
        lcd.print("X ");
        corrispondenza |= _BV(n);
        break;
      }
    }
  }

  return false;
}

void riproduciEffetto(byte effetto){
  switch (effetto) {
    case BIP:
    tone(PIN_buzzer, 440, 50);
    break;
    
    /*
    // Al momento nessun altro effetto sonoro
    case VINTO:
    
    break;
    
    case PERSO:
    
    break;
    */
  }
}

char assegna_carattere(byte premuto)
{
  return (65 + premuto);
}
