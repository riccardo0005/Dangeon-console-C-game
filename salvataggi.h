#ifndef SALVATAGGI_H
#define SALVATAGGI_H

#include "eroe.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include "menu.h"

#define MAX_NOME_FILE 256
#define INPUT_BACK 'b'

// Struttura che rappresenta i dati salvati per una partita, con checksum CRC32
typedef struct {

    time_t dataSalvataggio;          // Timestamp Unix del salvataggio
    char nome[MAX_NOME_EROE];        // Nome del giocatore
    int vita;                        // Punti vita
    int monete;                      // Monete
    int oggettiPosseduti;            // Oggetti posseduti
    int missioniCompletate;          // Missioni completate

    uint32_t crc32;                  // Checksum CRC32 per controllo integrità dati
} Salvataggio;

// Funzioni di conversione tra Eroe e Salvataggio
Salvataggio creaSalvataggioDaEroe(const Eroe* eroe);
Eroe creaEroeDaSalvataggio(const Salvataggio* salvataggio);

// Funzioni di gestione salvataggi
bool salvaGioco(const Salvataggio* s);
int contaSalvataggi(void);
bool leggiSalvataggioIndice(int idx, Salvataggio* s);
bool eliminaSalvataggio(int idx);

void mostraMenuSalvataggi();
int chiediSalvataggioDaCaricare();
int gestioneSalvataggioScelto(int sceltaSalvataggio);

// Funzione per calcolare CRC32 su un buffer di dati
uint32_t calcola_crc32(const void* dati, size_t lunghezza);

#endif // SALVATAGGI_H
