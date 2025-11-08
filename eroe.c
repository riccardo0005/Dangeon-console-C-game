// ...existing code...
#include <stdio.h>                 // include per funzioni di input/output come printf, fgets
#include <stdbool.h>               // include per il tipo bool (non usato qui ma consistente con altri file)
#include <string.h>                // include per funzioni su stringhe come strcpy, strlen

#include "eroe.h"                  // include dell'header che definisce la struttura Eroe e costanti come MAX_NOME_EROE


void inizializzaEroe(Eroe* eroe, const char* nome) { // funzione che inizializza i campi di una struttura Eroe
       
    if (eroe == NULL) {            // verifica se il puntatore passato è NULL per evitare dereferenziazioni invalide
        return;                    // esce subito se il puntatore non è valido
    } else {                        // altrimenti procede con l'inizializzazione
    strcpy(eroe->nome, nome);      // copia la stringa 'nome' nel campo nome della struttura eroe (attenzione a overflow gestito da MAX_NOME_EROE)
    eroe->vita = 20;               // imposta il valore iniziale della vita a 20
    eroe->monete = 0;              // imposta il numero iniziale di monete a 0
    eroe->missioniCompletate = 0;  // imposta il conteggio iniziale delle missioni completate a 0
    eroe->oggettiPosseduti = 0;    // imposta il numero iniziale di oggetti posseduti a 0
    }                              // chiusura del ramo else
}                                  // fine della funzione inizializzaEroe

void mostraEroe(const Eroe* eroe) { // funzione che stampa i dati dell'eroe sul terminale
    if (eroe == NULL) {             // verifica di sicurezza sul puntatore
        printf("Eroe non valido.\n"); // avvisa se il puntatore è NULL
        return;                      // esce dalla funzione
    }

    printf("\033[1;36m//---- DATI EROE ----//\033[0m\n"); // stampa intestazione colorata (ANSI)

    // Dati eroe in colore normale
    printf("\033[1;31mNome:\033[0m %s\n", eroe->nome);                 // stampa il nome dell'eroe con etichetta colorata
    printf("\033[1;31mVita:\033[0m %d\n", eroe->vita);                 // stampa la vita corrente dell'eroe
    printf("\033[1;31mMonete:\033[0m %d\n", eroe->monete);             // stampa il numero di monete possedute
    printf("\033[1;31mMissioni Completate:\033[0m %d\n", eroe->missioniCompletate); // stampa le missioni completate
    printf("\033[1;31mOggetti Posseduti:\033[0m %d\n", eroe->oggettiPosseduti);     // stampa il numero di oggetti posseduti

    printf("\033[1;36m//---------------//\033[0m\n"); // stampa linea di chiusura colorata
}                                                  // fine della funzione mostraEroe


void dichiaraNomeEroe(char* nomeEroe) {             // funzione che legge da stdin il nome dell'eroe e lo pulisce
    printf("Inserisci il nome del tuo eroe: ");     // prompt per l'utente
    fgets(nomeEroe, MAX_NOME_EROE, stdin);          // legge al massimo MAX_NOME_EROE-1 caratteri da stdin incluse newline
    // Rimuovi il carattere di nuova linea se presente
    size_t len = strlen(nomeEroe);                  // calcola la lunghezza della stringa letta
    if (len > 0 && nomeEroe[len - 1] == '\n') {     // se l'ultimo carattere è '\n'
        nomeEroe[len - 1] = '\0';                   // sostituisci il '\n' con il terminatore di stringa
    }
}                                                   // fine della funzione dichiaraNomeEroe   

void modificaMonete(Eroe* eroe, int delta) {        // funzione che modifica il numero di monete dell'eroe di 'delta'
    if (eroe == NULL) {                             // controllo di sicurezza sul puntatore
        return;                                     // esce se puntatore non valido
    }
    eroe->monete += delta;                          // applica la variazione alle monete
    if (eroe->monete < 0) {                         // evita che il numero di monete diventi negativo
        eroe->monete = 0;                           // imposta monete a zero se risultato negativo
    }
}                                                   // fine della funzione modificaMonete  

//delta 
void modificaVita(Eroe* eroe, int delta){           // funzione che modifica la vita dell'eroe di 'delta'
    // delta indica di quanto modificare la vita, può essere positivo o negativo

    if (eroe == NULL) {                             // controllo di sicurezza sul puntatore
        return;                                     // esce se puntatore non valido
    }
    eroe->vita += delta;                            // applica la variazione alla vita

    if (eroe->vita < 0) {                           // evita che la vita diventi negativa
        eroe->vita = 0;                             // imposta vita a zero se risultato negativo
    }
}                                                   // fine della funzione modificaVita
// ...existing code...