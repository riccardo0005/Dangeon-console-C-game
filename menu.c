#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "menu.h"
#include "salvataggi.h"
#include "eroe.h"
#include "trucchi.h"
#include "missioni.h"

#define MAX_TRUCCHI 32

// Colori ANSI per output colorato
#define COLORE_VERDE "\033[1;32m"
#define COLORE_ROSSO "\033[1;31m"
#define COLORE_GIALLO "\033[1;33m"
#define COLORE_BLU "\033[1;34m"
#define COLORE_CIANO "\033[1;36m"
#define COLORE_MAGENTA "\033[1;35m"
#define COLORE_RESET "\033[0m"

// Variabile statica per tracciare se i trucchi sono attivi
static bool trucchiAttivi = false;

// Prototipi di funzioni private (usate solo in questo file)
static void stampaMenuConRiquadroADueOpzioni(void);
static void stampaMenuConRiquadroAtreOpzioni(void);
static void stampaMenuVillaggio(void);

/**
 * Legge un singolo carattere dall'input e svuota il buffer fino a newline o EOF.
 * Restituisce il carattere letto.
 */
char leggiCaratterePulito(void) {
    int c;
    char ch = getchar();
    while ((c = getchar()) != '\n' && c != EOF);
    return ch;
}

// ============================================
// MENU PRINCIPALE (SCHERMATA INIZIALE)
// ============================================

void menuPrincipale(void) {
    char opzione;
    char contTrucchi[MAX_TRUCCHI] = {0};  // Buffer per memorizzare la sequenza Konami
    int i = 0;                             // Contatore per la posizione nel buffer

    while (1) {
        // Stampa il menu appropriato in base allo stato dei trucchi
        if (trucchiAttivi) {
            stampaMenuConRiquadroAtreOpzioni();
        } else {
            stampaMenuConRiquadroADueOpzioni();
        }
        
        printf("Seleziona una delle opzioni del menu [%s] : ", trucchiAttivi ? "1 - 3" : "1 - 2 - 0");

        opzione = leggiCaratterePulito();

        // Verifica se il carattere inserito è valido
        if (!carattereValido(opzione, trucchiAttivi)) {
            printf(COLORE_ROSSO "Carattere non valido, riprova.\n" COLORE_RESET);
            continue;
        }

        switch (opzione) {
            case '0':
                printf(COLORE_GIALLO "Uscita dal gioco. Arrivederci!\n" COLORE_RESET);
                return; // Termina il programma
                
            case '1':
                gestisciNuovaPartita();
                break; // Torna al menu principale dopo la partita
                
            case '2':
                gestisciCaricaSalvataggio();
                break;
                
            case '3':
                if (trucchiAttivi) {
                    gestisciMenuTrucchi();
                } else {
                    printf(COLORE_ROSSO "Opzione non valida.\n" COLORE_RESET);
                }
                break;
                
            case ' ':
                // Terminatore della sequenza Konami
                if (i == 0) {
                    printf(COLORE_ROSSO "Errore: terminatore spazio inserito senza sequenza.\n" COLORE_RESET);
                } else {
                    if (confrontoString(contTrucchi)) {
                        printf("\n" COLORE_VERDE "TRUCCHI ATTIVATI!\n" COLORE_RESET);
                        trucchiAttivi = true;
                    } else {
                        printf("\n" COLORE_ROSSO "Codice errato. Riprova.\n" COLORE_RESET);
                    }
                    i = 0;
                    contTrucchi[0] = '\0';
                }
                break;
                
            default:
                // Se non siamo in modalità trucchi, accumula caratteri per il codice Konami
                if (!trucchiAttivi) {
                    if (i < MAX_TRUCCHI - 1) {
                        contTrucchi[i++] = opzione;
                        contTrucchi[i] = '\0';
                    } else {
                        printf(COLORE_ROSSO "Sequenza troppo lunga, ripartiamo.\n" COLORE_RESET);
                        i = 0;
                        contTrucchi[0] = '\0';
                    }
                } else {
                    printf(COLORE_ROSSO "Opzione non valida.\n" COLORE_RESET);
                }
                break;
        }
    }
}

// ============================================
// NUOVA PARTITA
// ============================================

void gestisciNuovaPartita(void) {
    printf("\n" COLORE_VERDE "Hai scelto NUOVA PARTITA!\n" COLORE_RESET);
    
    // Crea un nuovo eroe
    Eroe eroe;
    printf("Inserisci il nome del tuo eroe: ");
    dichiaraNomeEroe(eroe.nome);

    inizializzaEroe(&eroe, eroe.nome);
    
    // Crea un salvataggio iniziale
    Salvataggio s = creaSalvataggioDaEroe(&eroe);

    if (salvaGioco(&s)) {
        printf(COLORE_VERDE "Salvataggio iniziale creato con successo!\n" COLORE_RESET);
    } else {
        printf(COLORE_ROSSO "Errore nel salvataggio iniziale.\n" COLORE_RESET);
    }

    mostraEroe(&eroe);
    
    // Inizializza il gestore delle missioni
    GestoreMissioni gestore;
    inizializzaGestoreMissioni(&gestore);
    
    printf("\n" COLORE_CIANO "Benvenuto nel villaggio, %s!\n" COLORE_RESET, eroe.nome);
    printf(COLORE_GIALLO "Il capo del villaggio ti chiama...\n" COLORE_RESET);
    printf(COLORE_ROSSO "\"Un'oscura minaccia incombe sul regno. Sei la nostra ultima speranza!\"\n" COLORE_RESET);
    
    // Entra nel menu del villaggio (loop principale di gioco)
    bool continuaGioco = true;
    while (continuaGioco) {
        continuaGioco = menuDelVillaggio(&eroe, &gestore);
    }
}

// ============================================
// CARICA SALVATAGGIO
// ============================================

void gestisciCaricaSalvataggio(void) {
    printf("\n" COLORE_CIANO "Hai scelto CARICA SALVATAGGIO!\n" COLORE_RESET);
    mostraMenuSalvataggi();

    if (contaSalvataggi() == 0) {
        printf(COLORE_GIALLO "Nessun salvataggio disponibile.\n" COLORE_RESET);
        return;
    }

    int sceltaSalvataggio = chiediSalvataggioDaCaricare();
    if (sceltaSalvataggio == -1) {
        printf(COLORE_GIALLO "Tornando al menu principale...\n" COLORE_RESET);
        return;
    }

    // ===== QUI È IL PUNTO CHIAVE =====
    // Chiedi all'utente cosa vuoi fare con il salvataggio selezionato
    printf("\n" COLORE_CIANO "Seleziona un'opzione per il salvataggio %d:\n" COLORE_RESET, sceltaSalvataggio);
    printf("1. Carica\n");
    printf("2. Elimina\n");
    printf("3. Annulla\n");
    
    char scelta;
    
    while (1) {
        printf("Seleziona opzione [1-3]: ");
        scelta = leggiCaratterePulito();
        
        if (scelta >= '1' && scelta <= '3') {
            break;
        }
        printf(COLORE_ROSSO "Opzione non valida. Riprova.\n" COLORE_RESET);
    }
    
    if (scelta == '3') {
        // Annulla
        printf(COLORE_GIALLO "Operazione annullata. Tornando al menu principale...\n" COLORE_RESET);
        return;
    }
    
    if (scelta == '2') {
        // Elimina
        printf(COLORE_ROSSO "\nSei sicuro di voler eliminare definitivamente il salvataggio? [S/N]: " COLORE_RESET);
        char conferma = leggiCaratterePulito();
        
        if (conferma == 'S' || conferma == 's') {
            if (eliminaSalvataggio(sceltaSalvataggio)) {
                printf(COLORE_VERDE "Salvataggio eliminato con successo.\n" COLORE_RESET);
            } else {
                printf(COLORE_ROSSO "Errore nell'eliminazione del salvataggio.\n" COLORE_RESET);
            }
        } else {
            printf(COLORE_GIALLO "Eliminazione annullata.\n" COLORE_RESET);
        }
        return; // Torna al menu principale
    }
    
    // Se arrivi qui, significa che hai scelto '1' = Carica
    
    // Carica il salvataggio selezionato
    Salvataggio s;
    if (!leggiSalvataggioIndice(sceltaSalvataggio, &s)) {
        printf(COLORE_ROSSO "Errore nel caricamento del salvataggio.\n" COLORE_RESET);
        return;
    }
    
    Eroe eroeCaricato = creaEroeDaSalvataggio(&s);
    printf(COLORE_VERDE "\nSalvataggio caricato con successo!\n" COLORE_RESET);
    mostraEroe(&eroeCaricato);
    
    // Inizializza il gestore missioni (TODO: salvare e caricare anche lo stato delle missioni)
    GestoreMissioni gestore;
    inizializzaGestoreMissioni(&gestore);
    
    // TODO: Ripristinare lo stato delle missioni completate dal salvataggio
    // Per ora utilizziamo il contatore missioniCompletate dell'eroe
    for (int i = 0; i < eroeCaricato.missioniCompletate && i < 3; i++) {
        gestore.missioni[i].completata = true;
        gestore.missioniCompletate = eroeCaricato.missioniCompletate;
    }
    
    // Sblocca la missione finale se necessario
    if (tutteLePreliminariCompletate(&gestore)) {
        sbloccaMissioneFinale(&gestore);
    }
    
    printf(COLORE_CIANO "\nBentornato, %s!\n" COLORE_RESET, eroeCaricato.nome);
    
    // Entra nel menu del villaggio
    bool continuaGioco = true;
    while (continuaGioco) {
        continuaGioco = menuDelVillaggio(&eroeCaricato, &gestore);
    }
}

// ============================================
// MENU TRUCCHI
// ============================================

void gestisciMenuTrucchi(void) {
    printf("\n" COLORE_MAGENTA "Sei entrato nel menu TRUCCHI!\n" COLORE_RESET);
    mostraMenuSalvataggi();

    if (contaSalvataggi() == 0) {
        printf(COLORE_GIALLO "Nessun salvataggio disponibile per modifiche.\n" COLORE_RESET);
        return;
    }

    int sceltaSalvataggio = chiediSalvataggioDaCaricare();
    if (sceltaSalvataggio == -1) {
        printf(COLORE_GIALLO "Tornando al menu principale...\n" COLORE_RESET);
        return;
    }

    modificaCampoSalvataggio(sceltaSalvataggio);
}

void modificaCampoSalvataggio(int sceltaSalvataggio) {
    printf(COLORE_CIANO "\nHai scelto il salvataggio %d\n" COLORE_RESET, sceltaSalvataggio);
    printf("Cosa vuoi modificare nel salvataggio %d?\n", sceltaSalvataggio);
    printf("1. Vita\n");
    printf("2. Monete\n");
    printf("3. Sblocca Missione Finale\n");

    char scelta;
    int c;
    
    while (1) {
        printf("Scegli un'opzione [1-3]: ");
        scelta = leggiCaratterePulito();

        if (scelta < '1' || scelta > '3') {
            printf(COLORE_ROSSO "Opzione non valida. Riprova.\n" COLORE_RESET);
            continue;
        }
        break;
    }

    Salvataggio s;
    if (!leggiSalvataggioIndice(sceltaSalvataggio, &s)) {
        printf(COLORE_ROSSO "Errore nel caricamento del salvataggio.\n" COLORE_RESET);
        return;
    }

    Eroe eroeModificato = creaEroeDaSalvataggio(&s);

    if (scelta == '1') {
        printf(COLORE_GIALLO "Modifica della VITA selezionata.\n" COLORE_RESET);
        printf("Di quanto vuoi modificare la vita? (usa numero negativo per diminuire): ");
        int delta;
        scanf("%d", &delta);
        while ((c = getchar()) != '\n' && c != EOF);
        modificaVita(&eroeModificato, delta);
        printf(COLORE_VERDE "Vita modificata! Nuova vita: %d\n" COLORE_RESET, eroeModificato.vita);
        
    } else if (scelta == '2') {
        printf(COLORE_GIALLO "Modifica delle MONETE selezionata.\n" COLORE_RESET);
        printf("Di quanto vuoi modificare le monete? (usa numero negativo per diminuire): ");
        int delta;
        scanf("%d", &delta);
        while ((c = getchar()) != '\n' && c != EOF);
        modificaMonete(&eroeModificato, delta);
        printf(COLORE_VERDE "Monete modificate! Nuove monete: %d\n" COLORE_RESET, eroeModificato.monete);
        
    } else if (scelta == '3') {
        printf(COLORE_MAGENTA "Sblocco della MISSIONE FINALE selezionata.\n" COLORE_RESET);
        // Impostiamo tutte le missioni come completate
        eroeModificato.missioniCompletate = 3;
        printf(COLORE_VERDE "Missione finale sbloccata! Tutte le missioni preliminari sono ora completate.\n" COLORE_RESET);
    }

    Salvataggio sModificato = creaSalvataggioDaEroe(&eroeModificato);
    if (salvaGioco(&sModificato)) {
        printf(COLORE_VERDE "Modifica salvata con successo!\n" COLORE_RESET);
    } else {
        printf(COLORE_ROSSO "Errore nel salvataggio delle modifiche.\n" COLORE_RESET);
    }
}

// ============================================
// MENU DEL VILLAGGIO (LOOP PRINCIPALE DI GIOCO)
// ============================================

bool menuDelVillaggio(Eroe* eroe, GestoreMissioni* gestore) {
    if (eroe == NULL || gestore == NULL) return false;
    
    stampaMenuVillaggio();
    
    printf("Seleziona una delle opzioni del menu [1-5]: ");
    
    char scelta = leggiCaratterePulito();
    
    switch (scelta) {
        case '1': {
            // Intraprendi una missione
            printf("\n" COLORE_CIANO "INTRAPRENDI UNA MISSIONE\n" COLORE_RESET);
            TipoMissione missioneScelta = selezionaMissione(gestore);
            
            if (missioneScelta != MISSIONE_NESSUNA) {
                eseguiMissione(gestore, eroe, missioneScelta);
                
                // Aggiorna il contatore delle missioni completate dell'eroe
                eroe->missioniCompletate = gestore->missioniCompletate;
            } else {
                printf(COLORE_GIALLO "Nessuna missione selezionata.\n" COLORE_RESET);
            }
            break;
        }
        
        case '2':
            // Riposati
            riposatiAlVillaggio(eroe);
            break;
            
        case '3':
            // Inventario
            mostraInventario(eroe);
            break;
            
        case '4':
            // Salva la partita
            salvaPartitaCorrente(eroe);
            break;
            
        case '5':
            // Esci
            return gestisciUscita();
            
        default:
            printf(COLORE_ROSSO "Opzione non valida!\n" COLORE_RESET);
            break;
    }
    
    return true; // Continua il gioco
}

void riposatiAlVillaggio(Eroe* eroe) {
    if (eroe == NULL) return;
    
    printf("\n" COLORE_VERDE "Ti riposi alla locanda del villaggio...\n" COLORE_RESET);
    
    if (eroe->vita >= 20) {
        printf(COLORE_CIANO "Sei gia in perfetta salute! (20/20 punti vita)\n" COLORE_RESET);
    } else {
        int vitaPrecedente = eroe->vita;
        eroe->vita = 20;
        printf(COLORE_VERDE "Hai recuperato %d punti vita!\n" COLORE_RESET, 20 - vitaPrecedente);
        printf(COLORE_CIANO "Punti vita: %d/20\n" COLORE_RESET, eroe->vita);
    }
    
    printf(COLORE_GIALLO "Sei pronto per nuove avventure!\n" COLORE_RESET);
}

void mostraInventario(const Eroe* eroe) {
    if (eroe == NULL) return;
    
    printf("\n" COLORE_CIANO "INVENTARIO\n" COLORE_RESET);
    mostraEroe(eroe);
}

void salvaPartitaCorrente(const Eroe* eroe) {
    if (eroe == NULL) return;
    
    printf("\n" COLORE_GIALLO "Salvataggio della partita in corso...\n" COLORE_RESET);
    
    Salvataggio s = creaSalvataggioDaEroe(eroe);
    
    if (salvaGioco(&s)) {
        printf(COLORE_VERDE "Partita salvata con successo!\n" COLORE_RESET);
    } else {
        printf(COLORE_ROSSO "Errore durante il salvataggio.\n" COLORE_RESET);
    }
}

bool gestisciUscita(void) {
    printf("\n" COLORE_GIALLO "Stai uscendo dal gioco.\n" COLORE_RESET);
    printf(COLORE_ROSSO "Ricordati di salvare la partita per non perdere i tuoi progressi!\n" COLORE_RESET);
    printf("Sei sicuro di voler procedere? [S/N]: ");
    
    char conferma = leggiCaratterePulito();
    
    if (conferma == 'S' || conferma == 's') {
        printf(COLORE_GIALLO "Tornando al menu principale...\n" COLORE_RESET);
        return false; // Esce dal loop del villaggio
    } else {
        printf(COLORE_VERDE "Continuiamo l'avventura!\n" COLORE_RESET);
        return true; // Continua il gioco
    }
}

// ============================================
// FUNZIONI PRIVATE PER STAMPARE I MENU
// ============================================

static void stampaMenuConRiquadroADueOpzioni(void)
{
    const char *blu = "\033[94m";
    const char *reset = "\033[0m";

    printf("%s", blu);
    printf("*************************************\n");
    printf("*           MENU PRINCIPALE         *\n");
    printf("*                                   *\n");
    printf("*  1. Nuova partita                 *\n");
    printf("*  2. Carica salvataggio            *\n");
    printf("*  0. Esci                          *\n");
    printf("*                                   *\n");
    printf("*************************************\n");
    printf("%s", reset);
}

static void stampaMenuConRiquadroAtreOpzioni(void)
{
    const char *blu = "\033[94m";
    const char *reset = "\033[0m";

    printf("%s", blu);
    printf("*************************************\n");
    printf("*           MENU PRINCIPALE         *\n");
    printf("*                                   *\n");
    printf("*  1. Nuova partita                 *\n");
    printf("*  2. Carica salvataggio            *\n");
    printf("*  3. Trucchi                       *\n");
    printf("*  0. Esci                          *\n");
    printf("*************************************\n");
    printf("%s", reset);
}

static void stampaMenuVillaggio(void)
{
    printf("\n");
    printf(COLORE_VERDE "--------------------------------------------\n" COLORE_RESET);
    printf(COLORE_VERDE "         MENU DEL VILLAGGIO\n" COLORE_RESET);
    printf(COLORE_VERDE "--------------------------------------------\n" COLORE_RESET);
    printf("1. Intraprendi una missione\n");
    printf("2. Riposati\n");
    printf("3. Inventario\n");
    printf("4. Salva la partita\n");
    printf("5. Esci\n");
    printf("\n");
}