/**
 * @file salvataggi.c
 * @brief Implementazione del sistema di gestione salvataggi
 * @author [RICCARDO/LORENZO/BOLA/YUNUX]
 * @date 2025
 * @version 3.0 (rimosso supporto CRC32)
 * 
 * @details
 * Questo file implementa un sistema completo per la gestione dei salvataggi di gioco,
 * includendo:
 * - Creazione e lettura di file di salvataggio
 * - Gestione della cartella dei salvataggi
 * - Conversione tra strutture Eroe e Salvataggio
 * - Interfaccia utente per la gestione dei salvataggi
 */

#include "salvataggi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>   // per _mkdir su Windows
/// @brief Macro per creare directory su Windows
#define MKDIR(path) _mkdir(path)
#else
#include <unistd.h>
/// @brief Macro per creare directory su Unix/Linux con permessi 0700
#define MKDIR(path) mkdir(path, 0700)
#endif

/// @brief Nome della cartella dove vengono salvati i file di gioco
#define CARTELLA_SALVATAGGI "salvataggi"

/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * FUNZIONI UTILITY
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/

/**
 * @brief Costruisce il percorso completo del file di salvataggio
 * 
 * @details
 * Questa funzione genera il nome del file di salvataggio combinando:
 * - Il nome della cartella dei salvataggi (CARTELLA_SALVATAGGI)
 * - Il prefisso "save"
 * - L'indice numerico del salvataggio
 * - L'estensione ".dat"
 * 
 * Il risultato ha il formato: "salvataggi/saveN.dat" dove N è l'indice.
 * 
 * @param idx Indice del salvataggio (numero positivo)
 * @param buffer Buffer dove verrà scritto il nome del file.
 *               Deve avere dimensione almeno MAX_NOME_FILE byte.
 * 
 * @note La funzione non valida l'indice né controlla se il file esiste.
 *       Si limita a costruire la stringa del percorso.
 * 
 * @warning Il buffer deve essere già allocato e avere dimensione sufficiente
 *          (almeno MAX_NOME_FILE byte) per evitare buffer overflow.
 */
static void costruisciNomeFile(int idx, char* buffer) {
    snprintf(buffer, MAX_NOME_FILE, "%s/save%d.dat", CARTELLA_SALVATAGGI, idx);
}

/**
 * @brief Verifica l'esistenza della cartella salvataggi e la crea se necessario
 * 
 * @details
 * Questa funzione utilizza la syscall stat() per verificare se la cartella
 * dei salvataggi esiste nel filesystem. Se la cartella non esiste, viene
 * creata usando la macro MKDIR che è platform-independent:
 * - Su Windows: usa _mkdir()
 * - Su Unix/Linux: usa mkdir() con permessi 0700 (rwx------)
 * 
 * @note Viene chiamata automaticamente da altre funzioni prima di accedere
 *       ai file di salvataggio per garantire che la struttura directory esista.
 * 
 * @note Su sistemi Unix/Linux, i permessi 0700 significano:
 *       - 7 (rwx): il proprietario può leggere, scrivere ed eseguire
 *       - 0 (---): il gruppo non ha permessi
 *       - 0 (---): gli altri utenti non hanno permessi
 * 
 * @warning Se la creazione della directory fallisce, la funzione non segnala
 *          l'errore. Le operazioni successive sui file falliranno.
 */
static void controllaCreaCartella() {
    struct stat st = {0};
    if (stat(CARTELLA_SALVATAGGI, &st) == -1) {
        MKDIR(CARTELLA_SALVATAGGI);
    }
}

/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * FUNZIONI I/O FILE
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/

/**
 * @brief Scrive un salvataggio su file
 * 
 * @details
 * Questa funzione salva una struttura Salvataggio su file in formato binario.
 * 
 * @param path Percorso completo del file dove salvare
 * @param s Puntatore alla struttura Salvataggio da salvare
 * 
 * @return true Scrittura completata con successo
 * @return false Errore durante l'apertura o scrittura del file
 * 
 * @note Il file viene creato o sovrascritto se già esistente (modalità "wb")
 */
static bool scriviFile(const char* path, const Salvataggio* s) {
    FILE* f = fopen(path, "wb");
    if (!f) return false;

    if (fwrite(s, sizeof(Salvataggio), 1, f) != 1) {
        fclose(f);
        return false;
    }
    
    fclose(f);
    return true;
}

/**
 * @brief Legge un salvataggio da file
 * 
 * @details
 * Questa funzione carica un salvataggio dal file system.
 * 
 * @param idx Indice del salvataggio da caricare (deve essere > 0)
 * @param s Puntatore alla struttura Salvataggio dove salvare i dati letti
 * 
 * @return true Caricamento riuscito, dati validi
 * @return false Errore: file non trovato o indice invalido
 */
bool leggiSalvataggioIndice(int idx, Salvataggio* s) {
    if (idx <= 0) return false;

    char nomeFile[MAX_NOME_FILE];
    costruisciNomeFile(idx, nomeFile);

    FILE* f = fopen(nomeFile, "rb");
    if (!f) return false;

    if (fread(s, sizeof(Salvataggio), 1, f) != 1) {
        fclose(f);
        return false;
    }
    
    fclose(f);
    return true;
}

/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * FUNZIONI GESTIONE SALVATAGGI
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/

/**
 * @brief Salva o aggiorna un salvataggio di gioco
 * 
 * @details
 * Questa funzione implementa una logica intelligente di salvataggio che:
 * - Cerca se esiste già un salvataggio con lo stesso nome dell'eroe
 * - Se trovato: AGGIORNA il salvataggio esistente mantenendo lo stesso slot
 * - Se non trovato: CREA un nuovo salvataggio in un nuovo slot
 * 
 * @param s Puntatore costante alla struttura Salvataggio da salvare
 * 
 * @return true Salvataggio creato o aggiornato con successo
 * @return false Errore: puntatore NULL, errore I/O, o altri problemi
 */
bool salvaGioco(const Salvataggio* s) {
    if (s == NULL) return false;
    
    controllaCreaCartella();

    char nomeFile[MAX_NOME_FILE];
    Salvataggio temp;
    int count = contaSalvataggi();
    bool trovato = false;
    int indiceTrovato = -1;

    // Ricerca salvataggio esistente con stesso nome
    for (int i = 1; i <= count; i++) {
        if (!leggiSalvataggioIndice(i, &temp)) {
            continue;
        }
        
        if (strcmp(temp.nome, s->nome) == 0) {
            indiceTrovato = i;
            trovato = true;
            break;
        }
    }

    // Aggiornamento salvataggio esistente
    if (trovato) {
        costruisciNomeFile(indiceTrovato, nomeFile);
        Salvataggio salvataggioAggiornato = *s;
        salvataggioAggiornato.dataSalvataggio = time(NULL);
        
        if (!scriviFile(nomeFile, &salvataggioAggiornato)) {
            printf("Errore nell'aggiornamento del salvataggio!\n");
            return false;
        }
        
        printf("Salvataggio aggiornato per '%s' (slot %d)\n", s->nome, indiceTrovato);
        return true;
    }
    
    // Creazione nuovo salvataggio
    else {
        costruisciNomeFile(count + 1, nomeFile);
        Salvataggio nuovoSalvataggio = *s;
        nuovoSalvataggio.dataSalvataggio = time(NULL);
        
        if (!scriviFile(nomeFile, &nuovoSalvataggio)) {
            printf("Errore nella creazione del salvataggio!\n");
            return false;
        }
        
        printf("Nuovo salvataggio creato per '%s' (slot %d)\n", s->nome, count + 1);
        return true;
    }
}

/**
 * @brief Conta il numero totale di salvataggi presenti
 * 
 * @details
 * Questa funzione determina quanti file di salvataggio esistono nella cartella
 * contando i file consecutivi a partire da save1.dat.
 * 
 * @return int Numero di salvataggi presenti (0 se nessuno)
 */
int contaSalvataggi(void) {
    controllaCreaCartella();

    int count = 0;
    char nomeFile[MAX_NOME_FILE];
    FILE* f;

    while (1) {
        costruisciNomeFile(count + 1, nomeFile);
        f = fopen(nomeFile, "rb");
        
        if (f) {
            fclose(f);
            count++;
        } else {
            break;
        }
    }
    
    return count;
}

/**
 * @brief Elimina un salvataggio e riordina i file rimanenti
 * 
 * @details
 * Questa funzione elimina un file di salvataggio e rinomina tutti i file
 * successivi per mantenere la numerazione consecutiva senza "buchi".
 * 
 * @param idx Indice del salvataggio da eliminare (deve essere > 0)
 * 
 * @return true Eliminazione riuscita
 * @return false Errore: indice invalido o file non eliminabile
 */
bool eliminaSalvataggio(int idx) {
    if (idx <= 0) return false;

    char nomeFile[MAX_NOME_FILE];
    costruisciNomeFile(idx, nomeFile);

    int totalePrima = contaSalvataggi();

    if (remove(nomeFile) != 0) return false;

    // Rinomina files successivi per mantenere numerazione consecutiva
    for (int i = idx + 1; i <= totalePrima; i++) {
        char nomeVecchio[MAX_NOME_FILE];
        char nomeNuovo[MAX_NOME_FILE];
        
        costruisciNomeFile(i, nomeVecchio);
        costruisciNomeFile(i - 1, nomeNuovo);
        
        rename(nomeVecchio, nomeNuovo);
    }

    return true;
}

/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * FUNZIONI CONVERSIONE
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/

/**
 * @brief Crea una struttura Salvataggio a partire da una struttura Eroe
 * 
 * @param eroe Puntatore costante alla struttura Eroe sorgente
 * @return Salvataggio Nuova struttura Salvataggio con i dati dell'eroe
 */
Salvataggio creaSalvataggioDaEroe(const Eroe* eroe) {
    Salvataggio s = {0};
    
    strncpy(s.nome, eroe->nome, MAX_NOME_EROE - 1);
    s.nome[MAX_NOME_EROE - 1] = '\0';
    
    s.vita = eroe->vita;
    s.monete = eroe->monete;
    s.missioniCompletate = eroe->missioniCompletate;
    s.oggettiPosseduti = eroe->oggettiPosseduti;
    s.dataSalvataggio = 0;
    
    return s;
}

/**
 * @brief Crea una struttura Eroe a partire da una struttura Salvataggio
 * 
 * @param salvataggio Puntatore costante alla struttura Salvataggio sorgente
 * @return Eroe Nuova struttura Eroe con i dati del salvataggio
 */
Eroe creaEroeDaSalvataggio(const Salvataggio* salvataggio) {
    Eroe e = {0};
    
    strncpy(e.nome, salvataggio->nome, MAX_NOME_EROE - 1);
    e.nome[MAX_NOME_EROE - 1] = '\0';
    
    e.vita = salvataggio->vita;
    e.monete = salvataggio->monete;
    e.missioniCompletate = salvataggio->missioniCompletate;
    e.oggettiPosseduti = salvataggio->oggettiPosseduti;
    
    return e;
}

/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * FUNZIONI INTERFACCIA UTENTE
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/

/**
 * @brief Mostra un menu formattato con l'elenco dei salvataggi disponibili
 * 
 * @details
 * Visualizza tutti i salvataggi presenti con le loro informazioni principali
 * in un formato tabellare leggibile.
 */
void mostraMenuSalvataggi() {
    int totaleSalvataggi = contaSalvataggi();
    
    printf("\n----------------------------------------\n");
    printf("       LISTA SALVATAGGI DISPONIBILI     \n");
    printf("----------------------------------------\n");
    
    if (totaleSalvataggi == 0) {
        printf("Nessun salvataggio trovato.\n");
        return;
    }
    
    printf("Ci sono %d salvataggio/i disponibile/i:\n\n", totaleSalvataggi);
    
    for (int i = 0; i < totaleSalvataggi; i++) {
        Salvataggio s;
        
        if (leggiSalvataggioIndice(i + 1, &s)) {
            char* dataStr = ctime(&s.dataSalvataggio);
            
            if (dataStr) {
                size_t len = strlen(dataStr);
                if (len > 0 && dataStr[len-1] == '\n') {
                    dataStr[len-1] = '\0';
                }
            }
            
            printf("\033[94m[%d]\033[0m %s", i + 1, s.nome);
            printf("     %s", dataStr ? dataStr : "Data sconosciuta");
            printf("      Vita: %d |  Monete: %d |  Oggetti: %d |  Missioni: %d\n\n",
                s.vita, s.monete, s.oggettiPosseduti, s.missioniCompletate);
        } else {
            printf("[%d] File non leggibile.\n\n", i + 1);
        }
    }
}

/**
 * @brief Richiede all'utente di selezionare un salvataggio da gestire
 * 
 * @return int Indice del salvataggio selezionato (1-N), o -1 per annullare
 */
int chiediSalvataggioDaCaricare(void) {
    char input[16];
    int scelta;

    while (1) {
        printf("\nSeleziona il salvataggio da gestire [1 - %d] (o '%c' per tornare indietro): ", 
               contaSalvataggi(), INPUT_BACK);

        if (!fgets(input, sizeof(input), stdin)) {
            printf("Errore di input.\n");
            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        bool valido = true;
        for (int i = 0; input[i] != '\0'; i++) {
            if (input[i] == INPUT_BACK) {
                return -1;
            } 
            else if (input[i] < '0' || input[i] > '9') {
                valido = false;
                break;
            }
        }

        if (!valido || strlen(input) == 0) {
            printf("Inserisci solo numeri tra 1 e %d.\n", contaSalvataggi());
            continue;
        }

        scelta = atoi(input);
        
        if (scelta < 1 || scelta > contaSalvataggi()) {
            printf("Numero non valido! Scegli tra 1 e %d.\n", contaSalvataggi());
            continue;
        }

        return scelta;
    }
}

/**
 * @brief Gestisce le operazioni su un salvataggio selezionato
 * 
 * @param sceltaSalvataggio Indice del salvataggio da gestire
 * @return int 1 se un'operazione è stata completata, 0 se annullata
 */
int gestioneSalvataggioScelto(int sceltaSalvataggio) {
    printf("Seleziona un'opzione per il salvataggio %d:\n", sceltaSalvataggio);
    printf("1. Carica Salvataggio\n");
    printf("2. Elimina Salvataggio\n");
    printf("3. Annulla e torna al menu principale\n");
    
    char scelta;
    int c;

    while (1) {
        printf("Scegli un'opzione [1-3]: ");
        scelta = getchar();
        while ((c = getchar()) != '\n' && c != EOF);

        if (scelta == '1') {
            Salvataggio s;
            
            if (leggiSalvataggioIndice(sceltaSalvataggio, &s)) {
                Eroe eroeCaricato = creaEroeDaSalvataggio(&s);
                
                printf("Salvataggio caricato con successo!\n");
                printf("Nome: %s\nVita: %d\nMonete: %d\nMissioni completate: %d\nOggetti posseduti: %d\n",
                    eroeCaricato.nome,
                    eroeCaricato.vita,
                    eroeCaricato.monete,
                    eroeCaricato.missioniCompletate,
                    eroeCaricato.oggettiPosseduti);
            } else {
                printf("Errore nel caricamento.\n");
            }
            
            return 1;
        }
        else if (scelta == '2') {
            printf("Sei sicuro di voler eliminare questo salvataggio? [S/N]: ");
            char conferma = getchar();
            while ((c = getchar()) != '\n' && c != EOF);
            
            if (conferma == 'S' || conferma == 's') {
                if (eliminaSalvataggio(sceltaSalvataggio)) {
                    printf("Salvataggio eliminato con successo.\n");
                } else {
                    printf("Errore nell'eliminazione.\n");
                }
            } else {
                printf("Eliminazione annullata.\n");
            }
            
            return 1;
        }
        else if (scelta == '3') {
            printf("Operazione annullata.\n");
            return 0;
        }
        else {
            printf("Opzione non valida. Riprova.\n");
        }
    }
}