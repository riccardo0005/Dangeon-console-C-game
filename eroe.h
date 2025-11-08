// ...existing code...
#ifndef EROE_H            // Se EROE_H non è definito, entra: inizio della guardia di inclusione per evitare doppie inclusioni
#define EROE_H            // Definisce la macro EROE_H per segnare che questo header è stato incluso

#define MAX_NOME_EROE 25  // Definisce la costante MAX_NOME_EROE = 25 (usata per limiti di lunghezza nome)
 // Nota: assicurarsi che la dimensione dell'array nome nella struct sia coerente con questa macro

// Definizione della struttura Eroe che rappresenta lo stato di un personaggio
typedef struct{           // Inizio della dichiarazione della struct anonima che poi verrà chiamata Eroe

    char nome[50];        // Array di char per il nome dell'eroe (qui 50 caratteri inclusivo di '\0')
                          // Attenzione: 50 non corrisponde a MAX_NOME_EROE (potenziale incoerenza)
    int vita;             // Intero che memorizza i punti vita correnti dell'eroe
    int monete;           // Intero che memorizza il numero di monete possedute
    int missioniCompletate;// Intero che memorizza quante missioni l'eroe ha completato
    int oggettiPosseduti; // Intero che memorizza quanti oggetti sono nel suo inventario
}Eroe;                    // Fine della struct e typedef: il nuovo tipo si chiama 'Eroe'

// Prototipi delle funzioni che operano sulla struttura Eroe:

void modificaMonete(Eroe* eroe, int delta); // Dichiara funzione che modifica il campo monete di 'eroe' di 'delta' (positivo o negativo)
void modificaVita(Eroe* eroe, int delta);   // Dichiara funzione che modifica il campo vita di 'eroe' di 'delta' (positivo o negativo)


void dichiaraNomeEroe(char* nomeEroe);      // Dichiara funzione che legge/assegna il nome dell'eroe in 'nomeEroe'
// Funzione per mostrare le informazioni dell'eroe
void mostraEroe(const Eroe* eroe);          // Dichiara funzione che stampa i campi dell'eroe; prende un puntatore const per non modificare i dati

// Funzione per inizializzare un eroe
void inizializzaEroe(Eroe* eroe, const char* nome);// Dichiara funzione che imposta i valori iniziali dell'eroe e copia 'nome' in eroe->nome
                                                      // Commento aggiuntivo: utile chiamare questa funzione all'inizio di una nuova partita
#endif                    // Fine della guardia di inclusione: chiude il blocco #ifndef/#define
// ...existing code...