#ifndef SALVATAGGI_H
#define SALVATAGGI_H

#include "eroe.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

#define MAX_NOME_FILE 256
#define INPUT_BACK 'b'

/**
 * Struttura che rappresenta i dati salvati per una partita
 * Include checksum CRC32 per controllo integrità dati
 */
typedef struct {
    time_t dataSalvataggio;          // Timestamp Unix del salvataggio (aggiornato automaticamente)
    char nome[MAX_NOME_EROE];        // Nome del giocatore (usato come identificatore univoco)
    int vita;                        // Punti vita correnti
    int monete;                      // Monete possedute
    int oggettiPosseduti;            // Numero di oggetti nell'inventario
    int missioniCompletate;          // Numero di missioni completate
    
    uint32_t crc32;                  // Checksum CRC32 per controllo integrità (calcolato automaticamente)
} Salvataggio;

// --- FUNZIONI DI CONVERSIONE ---

/**
 * Crea una struttura Salvataggio dai dati di un Eroe
 * NOTA: Il timestamp NON viene impostato qui, ma in salvaGioco()
 * 
 * @param eroe Puntatore all'eroe da salvare
 * @return Struttura Salvataggio con i dati dell'eroe (senza timestamp)
 */
Salvataggio creaSalvataggioDaEroe(const Eroe* eroe);

/**
 * Crea una struttura Eroe dai dati di un Salvataggio
 * 
 * @param salvataggio Puntatore al salvataggio da convertire
 * @return Struttura Eroe con i dati del salvataggio
 */
Eroe creaEroeDaSalvataggio(const Salvataggio* salvataggio);

// --- FUNZIONI DI GESTIONE FILE ---

/**
 * Salva o aggiorna un salvataggio su file
 * - Se esiste già un salvataggio con lo stesso nome, lo AGGIORNA
 * - Altrimenti crea un nuovo file di salvataggio
 * - Imposta automaticamente il timestamp al momento del salvataggio
 * - Calcola e salva il CRC32 per integrità
 * 
 * @param s Puntatore al salvataggio da salvare
 * @return true se il salvataggio è riuscito, false altrimenti
 */
bool salvaGioco(const Salvataggio* s);

/**
 * Conta il numero di file di salvataggio presenti
 * 
 * @return Numero di salvataggi disponibili
 */
int contaSalvataggi(void);

/**
 * Legge un salvataggio dal file specificato dall'indice
 * Verifica l'integrità tramite CRC32
 * Se il file è nel vecchio formato (senza CRC), lo aggiorna automaticamente
 * 
 * @param idx Indice del salvataggio (1-based: 1, 2, 3, ...)
 * @param s Puntatore dove memorizzare i dati letti
 * @return true se la lettura è riuscita e il file è valido, false se corrotto o non esistente
 */
bool leggiSalvataggioIndice(int idx, Salvataggio* s);

/**
 * Elimina un salvataggio e ricompatta gli altri
 * Esempio: se elimini save2.dat, save3.dat diventa save2.dat
 * 
 * @param idx Indice del salvataggio da eliminare (1-based)
 * @return true se l'eliminazione è riuscita, false altrimenti
 */
bool eliminaSalvataggio(int idx);

// --- FUNZIONI DI INTERFACCIA UTENTE ---

/**
 * Mostra a schermo la lista di tutti i salvataggi disponibili
 * con informazioni dettagliate (data, vita, monete, ecc.)
 */
void mostraMenuSalvataggi(void);

/**
 * Chiede all'utente di selezionare un salvataggio dalla lista
 * Permette di annullare premendo 'b'
 * 
 * @return Indice del salvataggio selezionato (1-based), o -1 se annullato
 */
int chiediSalvataggioDaCaricare(void);

/**
 * Mostra il menu di gestione per un salvataggio specifico
 * Opzioni: Carica, Elimina, Annulla
 * 
 * @param sceltaSalvataggio Indice del salvataggio da gestire
 * @return 1 se è stata eseguita un'azione, 0 se annullato
 */
int gestioneSalvataggioScelto(int sceltaSalvataggio);

// --- NOTA IMPORTANTE ---
// Il sistema di salvataggio funziona così:
// 1. Ogni eroe ha un NOME univoco che lo identifica
// 2. Quando salvi un eroe, il sistema:
//    - Cerca se esiste già un salvataggio con quel nome
//    - Se SÌ: AGGIORNA il file esistente con i nuovi dati
//    - Se NO: CREA un nuovo file di salvataggio
// 3. Questo significa che NON puoi avere due salvataggi con lo stesso nome
// 4. Per avere più salvataggi, usa nomi diversi per gli eroi

#endif // SALVATAGGI_H