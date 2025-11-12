#ifndef MENU_H                       // Se MENU_H non è definito allora entriamo: inizio guardia di inclusione
#define MENU_H                       // Definisce MENU_H per evitare doppie inclusioni dello stesso header

#include <stdbool.h>                 // Include per il tipo bool
#include "eroe.h"                    // Include la definizione della struttura Eroe
#include "missioni.h"                // Include la definizione del GestoreMissioni

// --- MENU PRINCIPALE ---

/**
 * Funzione principale che gestisce il menu iniziale del gioco
 * Permette di: avviare nuova partita, caricare salvataggio, usare trucchi
 * Include la gestione del codice Konami per sbloccare i trucchi
 */
void menuPrincipale(void);

/**
 * Gestisce l'avvio di una nuova partita
 * Crea un nuovo eroe e avvia il menu del villaggio
 */
void gestisciNuovaPartita(void);

/**
 * Gestisce il caricamento di un salvataggio esistente
 * Mostra la lista dei salvataggi e permette di caricarne uno
 */
void gestisciCaricaSalvataggio(void);

/**
 * Gestisce il menu dei trucchi (Konami code)
 * Permette di modificare statistiche e sbloccare la missione finale
 */
void gestisciMenuTrucchi(void);

/**
 * Modifica un campo specifico di un salvataggio (vita, monete, missione finale)
 */
void modificaCampoSalvataggio(int sceltaSalvataggio);

// --- MENU DEL VILLAGGIO ---

/**
 * Menu principale durante il gioco, accessibile dopo aver iniziato una partita
 * Opzioni: intraprendi missione, riposati, inventario, salva, esci
 * Ritorna true se il giocatore vuole continuare a giocare
 */
bool menuDelVillaggio(Eroe* eroe, GestoreMissioni* gestore);

/**
 * Permette all'eroe di riposare, ripristinando tutti i punti vita a 20
 */
void riposatiAlVillaggio(Eroe* eroe);

/**
 * Mostra l'inventario completo dell'eroe con tutti i suoi dati
 */
void mostraInventario(const Eroe* eroe);

/**
 * Gestisce il salvataggio della partita corrente
 * Salva lo stato dell'eroe e delle missioni completate
 */
void salvaPartitaCorrente(const Eroe* eroe);

/**
 * Gestisce l'uscita dal gioco con conferma
 * Avvisa il giocatore di salvare prima di uscire
 */
bool gestisciUscita(void);

// --- UTILITÀ ---

/**
 * Legge un singolo carattere dall'input e pulisce il buffer
 * Previene problemi con input multipli
 */
char leggiCaratterePulito(void);

#endif // MENU_H                      // Fine della guardia di inclusione: chiude il blocco #ifndef/#define