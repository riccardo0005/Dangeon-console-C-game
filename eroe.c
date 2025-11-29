
#include <stdio.h>                 // include per funzioni di input/output come printf, fgets
#include <stdbool.h>               // include per il tipo bool (non usato qui ma consistente con altri file)
#include <string.h>                // include per funzioni su stringhe come strcpy, strlen

#include "eroe.h"                  // include dell'header che definisce la struttura Eroe e costanti come MAX_NOME_EROE


//funzione che initializza l'eroe e di conseguenza i campi della struttura Eroe
void inizializzaEroe(Eroe* eroe, const char* nome) { //parametri, puntatore dlla variabile eroe e il nome precedentemente svuotato di caratteri indesiderati
       
    if (eroe == NULL) {            //Verifica se il puntatore passato è NULL 
        return;                    //Esce subito se il puntatore non è valido
    } else {                        //Altrimenti procede con l'inizializzazione dell'eroe
    
    //Funzione che copia strinche da una sorgente a una (destinazione, ad una sorgente)
    strcpy(eroe->nome, nome);      //(attenzione a overflow gestito da MAX_NOME_EROE)
    
    eroe->vita = 20;               //Imposta il valore iniziale della vita a 20
    eroe->monete = 0;              //Imposta il numero iniziale di monete a 0
    eroe->missioniCompletate = 0;  //Imposta il conteggio iniziale delle missioni completate a 0
    eroe->oggettiPosseduti = 0;    //Imposta il numero iniziale di oggetti posseduti a 0
    }                              //Chiusura del ramo else
}                                  //Fine della funzione inizializzaEroe



//Funzione che mi mosta i dati del mio eroe
void mostraEroe(const Eroe* eroe) { 
    if (eroe == NULL) {               //Verifica di sicurezza sul puntatore
        printf("Eroe non valido.\n"); //Avvisa se il puntatore è NULL
        return;                       //Esce dalla funzione
    }

    printf("\033[1;36m//---- DATI EROE ----//\033[0m\n"); // stampa intestazione colorata (ANSI)

    //Dati eroe in colore normale
    printf("\033[1;31mNome:\033[0m %s\n", eroe->nome);                 //Stampa il nome dell'eroe con etichetta colorata
    printf("\033[1;31mVita:\033[0m %d\n", eroe->vita);                 //Stampa la vita corrente dell'eroe
    printf("\033[1;31mMonete:\033[0m %d\n", eroe->monete);             //Stampa il numero di monete possedute
    printf("\033[1;31mMissioni Completate:\033[0m %d\n", eroe->missioniCompletate); //Stampa le missioni completate
    printf("\033[1;31mOggetti Posseduti:\033[0m %d\n", eroe->oggettiPosseduti);     //Stampa il numero di oggetti posseduti

    printf("\033[1;36m//---------------//\033[0m\n"); // stampa linea di chiusura colorata
}                                                


//funzione che legge il nome dell'eroe da stdin e lo pulisce
void dichiaraNomeEroe(char* nomeEroe) {           
    printf("Inserisci il nome del tuo eroe: ");    
    
    //fgets() legge una riga da tastiera (STDIN(standardinput)) di lungezza massima MAX NOME ERORE
    //legge anche le evewntuali newLine
    fgets(nomeEroe, MAX_NOME_EROE, stdin);
    
    // Rimuovi il carattere di nuova linea se presente

    //uso size_t perche strlen mi torna questo tipo di dato che esclude numeri negativi
    size_t len = strlen(nomeEroe);                  //Calcola la lunghezza della stringa letta

    if (len > 0 && nomeEroe[len - 1] == '\n') {     //Ce l'ultimo carattere è '\n'
        nomeEroe[len - 1] = '\0';                   //Sostituisci il '\n' con il terminatore di stringa
    }
}                                                 

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