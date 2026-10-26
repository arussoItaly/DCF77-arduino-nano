# DCF77-arduino-nano
Ricevitore DCF77 completo per Arduino nano

Componenti:
  - Arduino nano
  - Modulo ricevitore DCF77 RC8000
  - Schermo OLED da 0.96 pollici SSD1306

  - led per visualizzare la ricezione del segnale + resistenza da xxx ohm per il led (opzionale)
  - bottone per abilitare/disabilitare il led (opzionale)

  - filtro anti-disturbo da applicare all'entrata del modulo RC8000
    - 100 nF ceramico multistrato
    - 10 µF elettrolitico
```
         Ferrite
            |
          RC8000
        |        |
       GND      VCC
        |        |
        -  10µF  +
        |        !
        - 100nF  +
        |        |
        GND     VCC (3.3V)
          Arduino
  ```
  - RTC per mantenere l'ora corretta quando non è ricevuto il segnale di DCF77 (opzionale), in quanto il clock interno dell'Arduino nano perde normalmente circa 6 secondi ogni 15 minuti.
  
    - integrato RTC DS1302
    - quarzo da 32768
   
    <img width="1312" height="1199" alt="CollegamentoDS1302" src="https://github.com/user-attachments/assets/efcde857-33ab-4560-905c-e9934097a91d" />

  - resistenza da xxx ohm per aumentare la corrente assorbita (opzionale) da utilizzare caso in cui si alimenti il tutto con un power bank (che si spengono automaticamente quando il carico è troppo basso).

Nel codice è presente una sezione di configurazioni in modo da cambiare il comportamento e disabilitare/abilitare RTC, resistenza, led, ecc.
