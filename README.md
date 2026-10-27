# DCF77-arduino-nano
Ricevitore DCF77 completo per Arduino nano

#### Componenti:
  - Arduino nano
  - Modulo ricevitore DCF77 RC8000
  - Schermo OLED da 0.96 pollici SSD1306

  - (*opzionale*) led per visualizzare la ricezione del segnale + resistenza da 220 ohm per il led
  - (*opzionale*) bottone per abilitare/disabilitare il led

  - filtro anti-disturbo da applicare all'entrata del modulo RC8000
    - 100 nF ceramico multistrato
    - 10 µF elettrolitico
```
Arduino Nano (3.3V) --------+--------------------+--------> VCC (RC8000)
                            |                    |
                           [+]                  ---  100 nF (Ceramico Multistrato)
                      10 µF (Elettrolitico)     ---  (Filtra disturbi ad alta freq.)
                           [-]                   |
                            |                    |
Arduino Nano (GND)   -------+--------------------+--------> GND (RC8000)
  ```
  - (*opzionale ma consigliato*) RTC per mantenere l'ora corretta quando non è ricevuto il segnale di DCF77, in quanto il clock interno dell'Arduino nano perde normalmente circa 6 secondi ogni 15 minuti.
  
    - integrato RTC DS1302
    - quarzo da 32768
   
      <img width="350" height="319" alt="CollegamentoDS1302" src="https://github.com/user-attachments/assets/e1a32c78-3b7e-4f96-a0c2-3fb90175dbb8" />

  - (*opzionale*) resistenza da 330 ohm per aumentare la corrente assorbita da utilizzare caso in cui si alimenti il tutto con un power bank (che si spengono automaticamente quando il carico è troppo basso).

Nel codice è presente una sezione di configurazioni in modo da cambiare il comportamento e disabilitare/abilitare RTC, resistenza, led, ecc.

### Collegamenti con Arduino nano

#### Modulo DCF77 
  - GND => GND Arduino
  - VCC => +3.3 V Arduino
  - OUT => D2 Arduino
  - PON => D8 Arduino

#### Schermo OLED

  - GND => GND Arduino
  - SDA => A4 Arduino
  - SCL => A5 Arduino
  - VCC => 5V Arduino

#### Bottone (opzionale)
  - GND
  - D9 Arduino

#### Led (opzionale)
  - GND
  - Resistenza 220 ohm => D3 Arduino

#### RTC (opzionale ma consigliato)
  - DAT (pin 6) => D5 Arduino
  - RST (pin 5) => D6 Arduino 
  - CLK (pin 7) => D4 Arduino
  - 
  - VCC (pin 1) => 5V Arduino
  - GND (pin 4) => GND Arduino
    
  - X1 (pin 2) => piedino quarzo
  - X2 (pin 3) => piedino quarzo

#### Resistenza di carico 330 ohm (opzionale)
  - GND
  - D7 Arduino

**Consigli**

Se si alimenta l'Arduino nano con un alimentatore per evitare disturbi è consigliato mettere lungo il cavetto di alimentazione una ferrite.
Non mettere il modulo DCF77 vicino all'Arduino o allo schermo fanno interferenza, io ho messo il modulo con l'antenna ad almeno 10 cm di distanza dalla scatola con Arduino e OLED.

La scatola

Io ho comprato il progetto di stampa 3D di questa scatola (costa pochi euro):

[Universal ESP32/Arduino nano holder with display OLED 0.96"](https://cults3d.com/en/3d-model/gadget/universal-esp32-arduino-nano-holder-with-display-oled-0-96)

Per il led ed il bottone che sono opzionali ho praticato due buchi nella scatola, ed ho praticato un foro di uscita per i cavi di collegamento del modulo di ricezione del segnale di DCF77.

La scatola è piccola ed è molto precisa quindi ho scelto di utilizzare per i collegamenti interni dei fili piuttosto piccoli da 30AWG (diametro del conduttore circa 0,25 mm).

Per centrare l'uscita USBC dell'Arduino con il foro ho messo sotto l'Arduino uno piccolo spessore di gomma e fissato ai lati l'Arduino con degli spessori in legno.
