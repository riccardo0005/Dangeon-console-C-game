// ...existing code...
#ifndef SALVATAGGI_H        // Inizio guardia di inclusione: se SALVATAGGI_H non è definito entriamo
#define SALVATAGGI_H        // Definisce SALVATAGGI_H per evitare doppie inclusioni dello stesso header
#include "eroe.h"          // Include la definizione della struttura Eroe e le sue funzioni/prototipi
#include <stdio.h>         // Include le definizioni standard I/O (FILE, snprintf, ecc.)
#include <stdbool.h>       // Include il tipo bool e i valori true/false
#include <time.h>          // Include time_t e funzioni/data/time per timestamp dei salvataggi
#include "menu.h"          // Include prototipi relativi al menu (es. mostraMenuSalvataggi)

#define MAX_NOME_FILE 256  // Definisce la dimensione massima del buffer per i nomi dei file (in caratteri)
#define INPUT_BACK 'b'     // Carattere usato per indicare "torna indietro" nei menu

// Struttura che rappresenta i dati salvati per una partita
typedef struct {          // Inizio dichiarazione della struct Salvataggio
    time_t dataSalvataggio; // Timestamp Unix del momento in cui è stato creato il salvataggio
    char nome[MAX_NOME_EROE]; // Nome del giocatore (usa la costante MAX_NOME_EROE definita in eroe.h)
    int vita;              // Punti vita salvati del giocatore
    int monete;            // Quantità di monete salvate
    int oggettiPosseduti;  // Numero di oggetti nell'inventario salvato
    int missioniCompletate;// Conteggio delle missioni completate al momento del salvataggio
} Salvataggio;            // Fine della definizione della struct e typedef con nome 'Salvataggio'



// Prototipi helper per conversione tra Eroe e Salvataggio:
// crea una struttura Salvataggio a partire dallo stato di un Eroe
Salvataggio creaSalvataggioDaEroe(const Eroe* eroe);
// crea una struttura Eroe a partire dai dati di un Salvataggio
Eroe creaEroeDaSalvataggio(const Salvataggio* salvataggio);



// Prototipo: salva la struttura Salvataggio in un nuovo file; ritorna true se ha successo
bool salvaGioco(const Salvataggio* s);

// Prototipo: conta quanti file di salvataggio ci sono nella cartella "salvataggi"
int contaSalvataggi(void);

// Prototipo: legge il salvataggio con indice idx (1-based) nella struttura s; true se ok
bool leggiSalvataggioIndice(int idx, Salvataggio* s);

// Prototipo: elimina il salvataggio con indice idx (1-based); true se rimosso con successo
bool eliminaSalvataggio(int idx);

// Mostra il menu che elenca i salvataggi disponibili (interfaccia utente)
void mostraMenuSalvataggi();


// Chiede all'utente quale salvataggio caricare e ritorna l'indice scelto (o -1 per annullare)
int chiediSalvataggioDaCaricare();

// Gestisce le azioni possibili su un salvataggio scelto (carica/modifica/elimina ...)
// Riceve l'indice della scelta e ritorna un codice di risultato/azione
int gestioneSalvataggioScelto(int sceltaSalvataggio);


#endif // SALVATAGGI_H     // Fine guardia di inclusione: chiude il blocco #ifndef/#define
