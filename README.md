# DCF77-arduino-nano
Ricevitore DCF77 completo per Arduino nano

Componenti:
  - Arduino nano
  - Modulo ricevitore DCF77 RC8000
  - Schermo OLED da 0.96 pollici SSD1306

  - (opzionale) led per visualizzare la ricezione del segnale + resistenza da xxx ohm per il led
  - (opzionale) bottone per abilitare/disabilitare il led

  - filtro anti-disturbo da applicare all'entrata del modulo RC8000
    - 100 nF ceramico multistrato
    - 10 µF elettrolitico
```
Arduino Nano (3.3V) --------+-----------------------------------------+--------> VCC (RC8000)
                            |                                         |
                           [+]                                       ---  100 nF (Ceramico Multistrato)
                      10 µF (Elettrolitico)                          ---  (Filtra disturbi ad alta freq.)
                           [-]                                        |
                            |                                         |
Arduino Nano (GND)   -------+-----------------------------------------+--------> GND (RC8000)
  ```
  - (opzionale) RTC per mantenere l'ora corretta quando non è ricevuto il segnale di DCF77, in quanto il clock interno dell'Arduino nano perde normalmente circa 6 secondi ogni 15 minuti.
  
    - integrato RTC DS1302
    - quarzo da 32768
   
      <img width="350" height="319" alt="CollegamentoDS1302" src="https://github.com/user-attachments/assets/e1a32c78-3b7e-4f96-a0c2-3fb90175dbb8" />

  - (opzionale) resistenza da xxx ohm per aumentare la corrente assorbita da utilizzare caso in cui si alimenti il tutto con un power bank (che si spengono automaticamente quando il carico è troppo basso).

Nel codice è presente una sezione di configurazioni in modo da cambiare il comportamento e disabilitare/abilitare RTC, resistenza, led, ecc.
