#ifndef MISSIONI_H
#define MISSIONI_H

#include <stdbool.h>
#include "eroe.h"

// Enumerazione per identificare le missioni disponibili
typedef enum {
    MISSIONE_PALUDE = 0,           // Palude Putrescente
    MISSIONE_MAGIONE = 1,          // Magione Infestata
    MISSIONE_GROTTA = 2,           // Grotta di Cristallo
    MISSIONE_CASTELLO = 3,         // Castello del Signore Oscuro (finale)
    MISSIONE_NESSUNA = -1          // Nessuna missione attiva
} TipoMissione;

// Struttura che rappresenta lo stato di una singola missione
typedef struct {
    TipoMissione tipo;             // Tipo di missione
    char nome[50];                 // Nome descrittivo della missione
    char descrizione[200];         // Descrizione dell'obiettivo
    bool completata;               // true se la missione è stata completata
    bool sbloccata;                // true se la missione è accessibile
    
    // Contatori per il progresso della missione
    int obiettiviCompletati;       // Numero di obiettivi raggiunti (es: 2 Generali Orco uccisi)
    int obiettiviTotali;           // Numero di obiettivi necessari (es: 3 Generali Orco)
    
    // Flag speciali per oggetti da recuperare
    bool oggettoRecuperato;        // true se l'oggetto della missione è stato trovato
} Missione;

// Struttura che gestisce tutte le missioni del gioco
typedef struct {
    Missione missioni[4];          // Array delle 4 missioni (3 normali + 1 finale)
    int missioniCompletate;        // Contatore delle missioni completate
    TipoMissione missioneCorrente; // Missione attualmente in corso
} GestoreMissioni;

// --- FUNZIONI DI INIZIALIZZAZIONE ---

/**
 * Inizializza il gestore delle missioni con i dati di default
 * Le prime 3 missioni sono sbloccate, la finale è bloccata
 */
void inizializzaGestoreMissioni(GestoreMissioni* gestore);

/**
 * Inizializza una singola missione con nome, descrizione e obiettivi
 */
void inizializzaMissione(Missione* m, TipoMissione tipo, const char* nome, 
                         const char* descrizione, int obiettiviTotali);

// --- FUNZIONI DI VISUALIZZAZIONE MENU ---

/**
 * Mostra il menu di selezione delle missioni disponibili
 * Ritorna il numero di missioni disponibili da scegliere
 */
int mostraMenuMissioni(const GestoreMissioni* gestore);

/**
 * Mostra il menu durante una missione in corso
 * Include: Esplora, Negozio, Inventario, Torna al Villaggio
 */
void mostraMenuDuranteMissione(const Missione* missione, const Eroe* eroe);

/**
 * Mostra lo stato di avanzamento della missione corrente
 */
void mostraStatoMissione(const Missione* missione);

// --- FUNZIONI DI GESTIONE MISSIONI ---

/**
 * Permette al giocatore di selezionare una missione dal menu
 * Ritorna il tipo di missione selezionata o MISSIONE_NESSUNA se annullato
 */
TipoMissione selezionaMissione(GestoreMissioni* gestore);

/**
 * Avvia l'esecuzione di una missione specifica
 * Gestisce tutto il gameplay della missione (dungeon, combattimenti, ecc.)
 * Ritorna true se la missione è stata completata
 */
bool eseguiMissione(GestoreMissioni* gestore, Eroe* eroe, TipoMissione tipo);

/**
 * Completa una missione, aggiorna i contatori e sblocca eventualmente la finale
 */
void completaMissione(GestoreMissioni* gestore, TipoMissione tipo);

/**
 * Incrementa il contatore degli obiettivi completati (es: nemico sconfitto)
 */
void incrementaObiettivi(Missione* missione);

/**
 * Segna un oggetto speciale come recuperato (es: chiave, spada dell'eroe)
 */
void segnaOggettoRecuperato(Missione* missione);

// --- FUNZIONI DI CONTROLLO STATO ---

/**
 * Verifica se una missione è completata
 */
bool missioneCompletata(const Missione* missione);

/**
 * Verifica se una missione è accessibile/sbloccata
 */
bool missioneSbloccata(const Missione* missione);

/**
 * Verifica se tutte le missioni preliminari sono completate
 * (necessario per sbloccare la missione finale)
 */
bool tutteLePreliminariCompletate(const GestoreMissioni* gestore);

/**
 * Verifica se gli obiettivi della missione sono stati raggiunti
 */
bool obiettiviRaggiunti(const Missione* missione);

/**
 * Sblocca la missione finale quando le 3 missioni base sono completate
 */
void sbloccaMissioneFinale(GestoreMissioni* gestore);

// --- FUNZIONI DI UTILITÀ ---

/**
 * Ottiene il nome di una missione dal suo tipo
 */
const char* getNomeMissione(TipoMissione tipo);

/**
 * Ottiene la descrizione dell'obiettivo di una missione
 */
const char* getDescrizioneMissione(TipoMissione tipo);

/**
 * Ritorna il puntatore alla missione specifica nel gestore
 */
Missione* getMissione(GestoreMissioni* gestore, TipoMissione tipo);

#endif // MISSIONI_H