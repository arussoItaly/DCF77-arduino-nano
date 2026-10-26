#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TimeLib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long inizioLow = 0;
unsigned long fineLow = 0;
unsigned long durataLow = 0;

bool segnalePresente = false;
bool nuovoMinuto = false;

uint8_t indiceBit = 0;

uint8_t datiTempo[59];

uint8_t oraDCF;
uint8_t minutoDCF;
uint8_t giornoDCF;
uint8_t meseDCF;
unsigned int annoDCF;

uint8_t ora;
uint8_t minuto;
uint8_t giorno;
uint8_t mese;
unsigned int anno;
uint8_t giornoSettimana;

bool oraSolare = false;

const __FlashStringHelper *msg[5];
uint8_t n = 0;

uint8_t paritaOra;
uint8_t paritaMinuto;
uint8_t paritaData;

uint8_t bitInizioTempo;

unsigned long timeAcquisizione = 0;
bool ricevitoreAttivo = false;
unsigned long startDCF = 0;

uint8_t segnaleDcf = LOW;

bool bottone_StatoeStabile = HIGH;      // Stato stabile del pulsante
bool bottone_ultimaLettura = HIGH;      // Ultima lettura effettuata

unsigned long bottone_tempoCambio = 0;
const unsigned long bottone_debounce = 30;   // millisecondi

static uint8_t ultimoSecondoMostrato = 255;
  
//### Configurazioni
#define RTC_ON 1 //usa oppure no l'RTC

#define PIN_DCF 2
#define PON 8

#define LED_DCF 3 //LED_BUILTIN
#define PIN_BOTTONE 9
bool blinkLedSyncDCF = true;

bool gestioneStandByRicevitore = false;

const uint8_t numeroMinutiRefreshDCF = 60;

bool mostraInfoAggiuntive = false;
bool mostraFlagOraSolare = true;
bool mostraGiornoMeseAnnoSyncDCF = false;
bool mostraNomeMeseCorrente = true;

uint8_t luminositaDisplay = 1;  //Default 127 MAX 255

//Configurazione resistenza di carico per power bank
#define PIN_RESISTENZA 7
const unsigned long ON_TIME = 400;   //ms: duranta impulso di carico
const unsigned long OFF_TIME = 600; //ms: pausa tra un impulso e l'altro
unsigned long lastToggle = 0;
bool loadState = false;
bool usaResistenza = false;

bool centraOra = true;
bool centraData = true;
bool centraGiornoSettimana = true;

const int RTC_DAT = 5;
const int RTC_CLK = 4;
const int RTC_RST = 6;

//###

#if RTC_ON
  #include <ThreeWire.h>
  #include <RtcDS1302.h>

  ThreeWire myWire(RTC_DAT, RTC_CLK, RTC_RST);
  RtcDS1302<ThreeWire> Rtc(myWire);
#endif

void setup() {

  pinMode(PIN_DCF, INPUT);

  if (blinkLedSyncDCF) pinMode(LED_DCF, OUTPUT);

  if (PON > 0) pinMode(PON, OUTPUT);

  Wire.begin();
  Wire.setClock(400000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1);
  }

  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(luminositaDisplay);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.println("Sincronizzazione");
  display.println();
  display.println("DCF77");

  display.display();

  oraDCF = 0;
  minutoDCF = 0;
  giornoDCF = 0;
  meseDCF = 0;
  annoDCF = 0;

#if RTC_ON
  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

  if (!Rtc.IsDateTimeValid()) Rtc.SetDateTime(compiled);

  if (Rtc.GetIsWriteProtected()) Rtc.SetIsWriteProtected(false);

  if (!Rtc.GetIsRunning()) Rtc.SetIsRunning(true);
#endif
  
  accendiRicevitore();

  if (usaResistenza) {
    pinMode(PIN_RESISTENZA, OUTPUT);
    digitalWrite(PIN_RESISTENZA, LOW);
  }

  //SE RESISTENZA SEMPRE ACCESA
  //pinMode(PIN_RESISTENZA, OUTPUT);
  //digitalWrite(PIN_RESISTENZA, HIGH);

  if (PIN_BOTTONE > 0) pinMode(PIN_BOTTONE, INPUT_PULLUP);
}

void loop() {
  
  segnaleDcf = digitalRead(PIN_DCF);

  if (indiceBit >= 59) {
    nuovoMinuto = false;
  }

  static unsigned long ultimoCambio = 0;

  unsigned long adesso = millis();

  if (usaResistenza) {
    if (loadState && (adesso - lastToggle > ON_TIME)) {
      digitalWrite(PIN_RESISTENZA, LOW);
      loadState = false;
      lastToggle = adesso;
    }
    else if (!loadState && (adesso - lastToggle >= OFF_TIME)) {
      digitalWrite(PIN_RESISTENZA, HIGH);
      loadState = true;
      lastToggle = adesso;
    }
  }

  if (gestioneStandByRicevitore && !ricevitoreAttivo &&
      adesso - timeAcquisizione >= (unsigned long)numeroMinutiRefreshDCF * 60UL * 1000UL)
  {
    accendiRicevitore();
  }

  if (ricevitoreAttivo && adesso - startDCF >= 2000) // attendi la stabilizzazione
  {
    bool skip = ((segnaleDcf != segnalePresente) && (adesso - ultimoCambio < 20));

    if (!skip)
    {
      if (segnaleDcf != segnalePresente)
        ultimoCambio = adesso;

      if (blinkLedSyncDCF)
      {
        if (segnaleDcf > 0) {
          digitalWrite(LED_DCF, HIGH);
        }
        else {
          digitalWrite(LED_DCF, LOW);
        }
      }

      // Fronte di salita
      if (segnaleDcf == HIGH && !segnalePresente) {

        segnalePresente = true;

        fineLow = adesso;
        durataLow = fineLow - inizioLow;

        if (nuovoMinuto) {

          int bit = bitDaDurata(durataLow);

          if (bit != -1 && indiceBit < 59) {
            datiTempo[indiceBit++] = bit;
          }
        }
      }

      // Fronte di discesa
      if (segnaleDcf == LOW && segnalePresente) {

        segnalePresente = false;

        inizioLow = adesso;

        if (rilevaNuovoMinuto(durataLow))
        {
          ora = 0;
          minuto = 0;
          giorno = 0;
          mese = 0;
          anno = 0;
          oraSolare = false;
          memset(datiTempo, 0, sizeof(datiTempo));
        }
      }
    }
  }

  uint8_t secondoCorrente = second();

  if (secondoCorrente != ultimoSecondoMostrato)
  {
    ultimoSecondoMostrato = secondoCorrente;
    visualizzaDati();
  }

  if (PIN_BOTTONE > 0)
  {
    bool lettura = digitalRead(PIN_BOTTONE);

    // Se la lettura cambia, riparte il timer
    if (lettura != bottone_ultimaLettura)
    {
      bottone_tempoCambio = adesso;
      bottone_ultimaLettura = lettura;
    }

    // Se la lettura è stabile da almeno bottone_debounce ms
    if (adesso - bottone_tempoCambio >= bottone_debounce)
    {
      if (lettura != bottone_StatoeStabile)
      {
        bottone_StatoeStabile = lettura;

        // È stata rilevata una pressione
        if (bottone_StatoeStabile == LOW)
        {
          blinkLedSyncDCF = !blinkLedSyncDCF;

          if (!blinkLedSyncDCF) digitalWrite(LED_DCF, LOW);
        }
      }
    }
  }
}

int bitDaDurata(unsigned long durataLow) {

  //if (durataLow >= 851 && durataLow <= 950) //A
  //if (durataLow >= 851 && durataLow <= 1050) //B
  if (durataLow >= 870 && durataLow <= 980) //C
    return 0;

  //if (durataLow >= 750 && durataLow <= 850) //A
  //if (durataLow >= 650 && durataLow <= 850) //B
  if (durataLow >= 720 && durataLow <= 860) //C
    return 1;

  return -1;
}

bool rilevaNuovoMinuto(unsigned long durataLow) {

  if (durataLow >= 1750) {
    uint8_t bitRicevuti = indiceBit;

    // Il bit 58 non genera un fronte "normale": la sua pausa si fonde
    // con il secondo 59 mancante. La recuperiamo sottraendo la durata
    // nominale del secondo assente.
    if (nuovoMinuto && bitRicevuti == 58) {
      int bit58 = bitDaDurata(durataLow - 1000);
      if (bit58 != -1) {
        datiTempo[58] = bit58;
        bitRicevuti = 59;
      }
    }

    indiceBit = 0;
    nuovoMinuto = true;

    if (bitRicevuti < 59) return true;

    if (datiTempo[0] != 0)
      return true;

    if (datiTempo[17] == datiTempo[18]) return true;

    bitInizioTempo = datiTempo[20];

    if (bitInizioTempo != 1) return true;

    minuto =
      datiTempo[21] * 1 +
      datiTempo[22] * 2 +
      datiTempo[23] * 4 +
      datiTempo[24] * 8 +
      datiTempo[25] * 10 +
      datiTempo[26] * 20 +
      datiTempo[27] * 40;

    ora =
      datiTempo[29] * 1 +
      datiTempo[30] * 2 +
      datiTempo[31] * 4 +
      datiTempo[32] * 8 +
      datiTempo[33] * 10 +
      datiTempo[34] * 20;

    paritaOra = datiTempo[35];
    paritaMinuto = datiTempo[28];
    paritaData = datiTempo[58];

    giorno =
      datiTempo[36] * 1 +
      datiTempo[37] * 2 +
      datiTempo[38] * 4 +
      datiTempo[39] * 8 +
      datiTempo[40] * 10 +
      datiTempo[41] * 20;

    giornoSettimana =
      datiTempo[42] * 1 +
      datiTempo[43] * 2 +
      datiTempo[44] * 4;

    mese =
      datiTempo[45] * 1 +
      datiTempo[46] * 2 +
      datiTempo[47] * 4 +
      datiTempo[48] * 8 +
      datiTempo[49] * 10;

    anno =
      2000 +
      datiTempo[50] * 1 +
      datiTempo[51] * 2 +
      datiTempo[52] * 4 +
      datiTempo[53] * 8 +
      datiTempo[54] * 10 +
      datiTempo[55] * 20 +
      datiTempo[56] * 40 +
      datiTempo[57] * 80;

    if (checkParity(21, 27) != paritaMinuto) return true;

    if (checkParity(29, 34) != paritaOra) return true;

    if (checkParity(36, 57) != paritaData) return true;

    if (ora > 23) return true;
    if (minuto > 59) return true;
    if (giorno == 0 || giorno > 31) return true;
    if (mese == 0 || mese > 12) return true;
    if (giornoSettimana < 1 || giornoSettimana > 7) return true;

    oraSolare = false;

    n = 0;

    if (datiTempo[15] > 0)
      msg[n++] = F("Antenna di backup");

    if (datiTempo[16] > 0)
      msg[n++] = F("Cambio ora");

    if (datiTempo[17] > 0)
      msg[n++] = F("Ora legale UTC+2");

    if (datiTempo[18] > 0)
    {
      msg[n++] = F("Ora solare UTC+1");
      oraSolare = true;
    }

    if (datiTempo[19] > 0)
      msg[n++] = F("Secondo intercalare");

    oraDCF = ora;
    minutoDCF = minuto;
    giornoDCF = giorno;
    meseDCF = mese;
    annoDCF = anno;

    setTime(ora, minuto, 0, giorno, mese, anno);
    
#if RTC_ON
    aggiornaRTC(giorno, mese, anno, ora, minuto, 0);
#endif
    
    timeAcquisizione = millis();

    if (gestioneStandByRicevitore)
    {
      spegniRicevitore();
    }

    return true;
  }

  return false;
}

uint8_t checkParity(uint8_t a, uint8_t b)
{
  uint8_t parity = 0;

  for (uint8_t i = a; i <= b; i++)
    parity ^= datiTempo[i];

  return parity;
}

void visualizzaDati()
{
const __FlashStringHelper *mesi[] = {
  F("Gennaio"), F("Febbraio"), F("Marzo"), F("Aprile"),
  F("Maggio"), F("Giugno"), F("Luglio"), F("Agosto"),
  F("Settembre"), F("Ottobre"), F("Novembre"), F("Dicembre")
};

  uint8_t textSize = 1;

  if (year() == 1970) return;

  display.clearDisplay();
  display.setCursor(0, 0);

  display.print(F("DCF "));

  displayZero(oraDCF);
  display.print(F(":"));

  displayZero(minutoDCF);

  if (mostraGiornoMeseAnnoSyncDCF)
  {
    display.print(F(" "));
    displayZero(giornoDCF);
    display.print(F("/"));
    displayZero(meseDCF);
    display.print(F("/"));
    display.print(annoDCF);
  }

  display.println();
  display.println();

  display.setTextSize(2);
  textSize = 2;

  if(centraOra)
  {
    //Per centrare l'ora
    int16_t y = display.getCursorY();
    display.setCursor(sizeLen(8, textSize), y);
  }

#if RTC_ON
  time_t t = getRTCTime();
#else
  time_t t = now();
#endif
  
  displayZero(hour(t));
  display.print(F(":"));

  displayZero(minute(t));
  display.print(F(":"));

  displayZero(second(t));

  if (mostraFlagOraSolare && oraSolare)
  {
    display.setTextSize(1);
    display.print(F(" *"));
  }

  display.println();

  display.setTextSize(1);
  textSize = 1;

  display.println();

  //calcolo numero caratteri per centrare la scritta
  uint8_t numeroCaratteri = 2; //numero di spazi

  if (mostraNomeMeseCorrente)
  {
    numeroCaratteri += 1; //giorno

    if(day(t) >= 10) numeroCaratteri += 1;
    
    numeroCaratteri += strlen_P((const char *)mesi[month() - 1]); //mese
  }
  else
  {
    numeroCaratteri += 4; //giorno e mese
  }

  numeroCaratteri += 4; //anno

  if(centraData)
  {
    //Per centrare la data
    uint8_t y = display.getCursorY();
    display.setCursor(sizeLen(numeroCaratteri, textSize), y);
  }
  
  if (mostraNomeMeseCorrente)
  {
    display.print(day(t));
    display.print(F(" "));
    display.print(mesi[month() - 1]);
    display.print(F(" "));
  }
  else
  {
    displayZero(day(t));
    display.print(F("/"));
    displayZero(month());
    display.print(F("/"));
  }

  display.print(year(t));

  if (mostraNomeMeseCorrente)
  {
    display.println();

    if (!mostraInfoAggiuntive)
    {
      display.println();
    }
  }
  else
    display.print(F(" "));

  displayGiornoSettimana();

  if (mostraInfoAggiuntive) displayInfo();

  display.display();
}

void displayGiornoSettimana()
{
const __FlashStringHelper *giorni[] = {
  F("Domenica"),
  F("Luned\x8D"),
  F("Marted\x8D"),
  F("Mercoled\x8D"),
  F("Gioved\x8D"),
  F("Venerd\x8D"),
  F("Sabato")
};

  if(centraGiornoSettimana)
  {
    //Per centrare il giorno della settimana
    int16_t y = display.getCursorY();
    display.setCursor(sizeLen(strlen_P((const char *)giorni[weekday() - 1]) , 1), y);
  }
  
  //display.println();
  display.println(giorni[weekday() - 1]);
}

void displayInfo()
{
  for (uint8_t i = 0; i < n; i++)
  {
    if (i > 0) display.print(F(" "));
    display.print(msg[i]);
  }
}

void displayZero(uint8_t v)
{
  if (v < 10) {
    display.print(F("0"));
  }
  display.print(v);
}

void accendiRicevitore()
{
  inizioLow = 0;
  fineLow = 0;
  durataLow = 0;
  segnalePresente = false;
  indiceBit = 0;
  nuovoMinuto = false;

  if (PON > 0) digitalWrite(PON, LOW);
  startDCF = millis();
  ricevitoreAttivo = true;
}

void spegniRicevitore()
{
  if (PON > 0) digitalWrite(PON, HIGH); //standby ricevitore
  ricevitoreAttivo = false;
  startDCF = 0;
  if (blinkLedSyncDCF) digitalWrite(LED_DCF, LOW);
}

uint8_t sizeLen(uint8_t chars, uint8_t textSize)
{
  return (SCREEN_WIDTH - (chars * 6 * textSize)) / 2; 
}

#if RTC_ON
  void aggiornaRTC(uint8_t giorno, uint8_t mese, unsigned int anno, uint8_t ora, uint8_t minuto, uint8_t secondo)
  {
    RtcDateTime adesso = RtcDateTime(anno, mese, giorno, ora, minuto, secondo);
  
    Rtc.SetIsWriteProtected(false);
    Rtc.SetDateTime(adesso);
    Rtc.SetIsWriteProtected(true);
  }

  time_t getRTCTime()
  {
    RtcDateTime dt = Rtc.GetDateTime();
  
    tmElements_t tm;
  
    tm.Year = CalendarYrToTm(dt.Year());
    tm.Month = dt.Month();
    tm.Day = dt.Day();
    tm.Hour = dt.Hour();
    tm.Minute = dt.Minute();
    tm.Second = dt.Second();
  
    return makeTime(tm);
  }
#endif
