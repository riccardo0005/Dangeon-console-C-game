/**
 * @file menu.c
 * @brief Implementazione del sistema di menu principale del gioco
 * @details Questo file gestisce l'interfaccia utente principale del gioco, inclusi:
 *          - Menu principale con opzioni di gioco
 *          - Sistema di attivazione trucchi (codice Konami)
 *          - Gestione salvataggi e caricamenti
 *          - Menu del villaggio e loop di gioco principale
 *          - Gestione buffer input per evitare problemi con scanf/getchar
 * @author [RICCARDO/LORENZO/BOLA/YUNUX]
 * @date 2025
 * @version 1.0
 */

//Include la libreria standard per input/output (printf scanf getchar etcetc)
#include <stdio.h>

//Include la libreria per i tipi booleani (false e true)
#include <stdbool.h>

//Include la libreria per le funzioni di gestione delle stringhe (strcmp / strcpy)
#include <string.h>

//Include tutti i file header personalizzati del progetto che contengono le varie dichiarazioni delle funzioni
#include "menu.h"       ///< Funzioni relative al menu di gioco
#include "salvataggi.h" ///< Funzioni relative al salvataggio e il caricamento delle partite
#include "eroe.h"       ///< Funzioni e strutture relative all'eroe ed al personaggio
#include "trucchi.h"    ///< Funzioni per gestire i trucchi
#include "missioni.h"   ///< Funzioni per gestire le varie missioni di gioco

/**
 * @def MAX_TRUCCHI
 * @brief Numero massimo di caratteri ammessi per la sequenza di attivazione trucchi
 * @details Definisce la dimensione del buffer che memorizza la sequenza Konami
 *          per l'attivazione dei trucchi nel menu principale
 */
#define MAX_TRUCCHI 32

/**
 * @defgroup ANSI_Colors Codici colore ANSI
 * @brief Definizioni di codici ANSI per colorare il testo del terminale
 * @details Questi codici permettono di rendere l'interfaccia più gradevole esteticamente.
 *          La sintassi è: \033[ (sequenza escape) + 1 (grassetto) + numero colore + m (terminatore)
 * @{
 */
#define COLORE_VERDE "\033[1;32m"     ///< Colore verde grassetto
#define COLORE_ROSSO "\033[1;31m"     ///< Colore rosso grassetto
#define COLORE_GIALLO "\033[1;33m"    ///< Colore giallo grassetto
#define COLORE_BLU "\033[1;34m"       ///< Colore blu grassetto
#define COLORE_CIANO "\033[1;36m"     ///< Colore ciano grassetto
#define COLORE_MAGENTA "\033[1;35m"   ///< Colore magenta grassetto
#define COLORE_RESET "\033[0m"        ///< Ripristina colore normale (fondamentale per evitare che tutto rimanga colorato)
/** @} */ // Fine gruppo ANSI_Colors

/**
 * @var trucchiAttivi
 * @brief Variabile globale statica che traccia lo stato di attivazione dei trucchi
 * @details Visibile solo all'interno di questo file (static).
 *          È FONDAMENTALE per determinare se i trucchi sono stati attivati tramite
 *          la sequenza Konami e quindi se mostrare l'opzione trucchi nel menu
 */
static bool trucchiAttivi = false;

//Prototipi (dichiarazioni) di funzioni statiche (usate solo in questo file)
//servono per dichiarare funzioni che verranno usate più avanti nel codice

/**
 * @brief Stampa il menu principale con due opzioni (senza trucchi)
 * @details Visualizza un riquadro decorativo con le opzioni:
 *          1. Nuova partita
 *          2. Carica salvataggio
 *          0. Esci
 *          Questa versione viene mostrata quando i trucchi NON sono attivi
 * @note Funzione statica, utilizzabile solo in questo file
 * @see stampaMenuConRiquadroAtreOpzioni()
 */
static void stampaMenuConRiquadroADueOpzioni(void);

/**
 * @brief Stampa il menu principale con tre opzioni (con trucchi)
 * @details Visualizza un riquadro decorativo con le opzioni:
 *          1. Nuova partita
 *          2. Carica salvataggio
 *          3. Trucchi
 *          0. Esci
 *          Questa versione viene mostrata quando i trucchi SONO attivi
 * @note Funzione statica, utilizzabile solo in questo file
 * @see stampaMenuConRiquadroADueOpzioni()
 */
static void stampaMenuConRiquadroAtreOpzioni(void);

/**
 * @brief Stampa il menu specifico del villaggio di gioco
 * @details Visualizza le opzioni disponibili nel villaggio:
 *          1. Intraprendi una missione
 *          2. Riposati
 *          3. Inventario
 *          4. Salva la partita
 *          5. Esci
 * @note Funzione statica, utilizzabile solo in questo file
 */
static void stampaMenuVillaggio(void);

/**
 * @defgroup BufferManagement Gestione del buffer degli input
 * @brief Funzioni per la gestione corretta del buffer di input
 * @details NECESSITÀ: Pulire il buffer di input dopo scanf() e getchar() per evitare 
 *          che il carattere '\n' o altri caratteri residui causino bug nelle letture successive.
 *          
 *          PROBLEMATICA: Sia scanf che getchar oltre al carattere letto memorizzano dei 
 *          caratteri residui che sarebbero poi letti da un eventuale successivo scanf.
 *          
 *          NOTA IMPORTANTE: EOF è fondamentale poiché in caso l'input fallisse inaspettatamente 
 *          si finirebbe in un while infinito. Con EOF l'input esce comunque permettendo al 
 *          programma di non bloccarsi mai. EOF è un numero intero che vale -1 definito nella 
 *          libreria stdio.h e viene restituito da getchar in varie situazioni tra cui quando fallisce.
 * @{
 */

/**
 * @brief Legge un singolo carattere dall'input e pulisce automaticamente il buffer
 * @details QUANDO USARLA:
 *          - Menu di gioco (scelta 1, 2, 3, ecc.)
 *          - Conferme (S/N)
 *          - Qualsiasi input di un singolo carattere
 *          
 *          COME FUNZIONA:
 *          1. Legge il carattere inserito dall'utente
 *          2. Automaticamente svuota il buffer (rimuove '\n' e altri residui)
 *          3. Restituisce solo il carattere valido
 * 
 * @return char Il carattere inserito dall'utente (senza caratteri residui nel buffer)
 * 
 * @code
 * // Esempio di utilizzo:
 * printf("Inserisci una scelta [1-3]: ");
 * char scelta = leggiCaratterePulito();
 * if (scelta == '1') {
 *     // Elabora scelta 1
 * }
 * @endcode
 * 
 * @warning Non utilizzare questa funzione per leggere stringhe o numeri, solo caratteri singoli
 * @see pulisciBuffer()
 */
char leggiCaratterePulito(void) {
    int c; ///< Variabile per leggere i caratteri aggiuntivi dal buffer

    char ch = getchar(); ///< Legge il primo carattere inserito dall'utente e lo memorizza in ch
    
    //Ciclo while che continua finché non trova '\n' o EOF
    while ((c = getchar()) != '\n' && c != EOF);
    
    return ch; ///< Restituisce il carattere valido
}

/**
 * @brief Pulisce completamente il buffer di input
 * @details QUANDO USARLA:
 *          - SEMPRE dopo scanf("%d", ...) (numeri interi)
 *          - SEMPRE dopo scanf("%f", ...) (numeri decimali)  
 *          - SEMPRE dopo scanf("%s", ...) (stringhe)
 *          
 *          COME FUNZIONA:
 *          1. Legge e scarta tutti i caratteri nel buffer
 *          2. Si ferma quando trova '\n' (newline) o EOF (fine file)
 *          3. Non restituisce nulla (void), pulisce solamente
 * 
 * @return void Questa funzione non restituisce alcun valore
 * 
 * @code
 * // Esempio di utilizzo:
 * int numero;
 * printf("Inserisci un numero: ");
 * scanf("%d", &numero);
 * pulisciBuffer();  // FONDAMENTALE per evitare problemi
 * @endcode
 * 
 * @warning Chiamare SEMPRE questa funzione dopo scanf per evitare malfunzionamenti
 * @see leggiCaratterePulito()
 */
void pulisciBuffer(void) {
    int c; ///< Variabile temporanea per leggere caratteri dal buffer
    while ((c = getchar()) != '\n' && c != EOF); ///< Svuota tutto fino a newline o EOF
}

/** @} */ // Fine gruppo BufferManagement

/**
 * @brief Gestisce il menu principale del gioco a 360 gradi
 * @details Questa è la funzione centrale che coordina tutto il flusso del menu principale.
 *          Gestisce:
 *          - Visualizzazione del menu appropriato (con o senza trucchi)
 *          - Lettura e validazione dell'input utente
 *          - Gestione della sequenza Konami per attivare i trucchi
 *          - Routing verso le varie funzionalità (nuova partita, carica, trucchi, esci)
 *          
 *          Il menu rimane attivo in un ciclo infinito fino a quando l'utente sceglie di uscire.
 *          La sequenza Konami viene memorizzata carattere per carattere e verificata quando
 *          l'utente preme spazio come terminatore.
 * 
 * @return void Non restituisce alcun valore (il ciclo continua fino all'uscita)
 * 
 * @note La sequenza Konami è nascosta e viene attivata inserendo una serie specifica di caratteri
 *       seguita da spazio. Una volta attivati i trucchi, appare l'opzione "3. Trucchi" nel menu.
 * 
 * @warning Se l'utente inserisce una sequenza troppo lunga (>MAX_TRUCCHI), viene automaticamente
 *          resettata per evitare overflow del buffer
 * 
 * @see stampaMenuConRiquadroADueOpzioni()
 * @see stampaMenuConRiquadroAtreOpzioni()
 * @see gestisciNuovaPartita()
 * @see gestisciCaricaSalvataggio()
 * @see gestisciMenuTrucchi()
 */
void menuPrincipale(void) {
    char opzione; ///< Variabile contenente l'opzione scelta dall'utente
    
    char contTrucchi[MAX_TRUCCHI] = {0}; ///< Buffer per memorizzare la sequenza Konami mediante un array di dimensione MAX_TRUCCHI
    int i = 0; ///< Contatore per la posizione corrente nel buffer dei trucchi

    while (1) { //ciclo infinito 

        // Stampa il menu appropriato in base allo stato dei trucchi
        if (trucchiAttivi) { //se i trucchi sono attivi
            stampaMenuConRiquadroAtreOpzioni(); //personalizzazione con trucchi
        } else { //altrimenti
            stampaMenuConRiquadroADueOpzioni(); //personalizzazione senza trucchi
        }
        
        // Usa operatore ternario: condizione ? se vera : se falsa
        printf("Seleziona una delle opzioni del menu [%s] : ", trucchiAttivi ? "1 - 3" : "1 - 2 - 0");

        opzione = leggiCaratterePulito(); //svuota il buffer per evitare terminatori non desiderati

        // Verifica se il carattere inserito è valido
        if (!carattereValido(opzione, trucchiAttivi)) { //se il carattere non è valido
            printf(COLORE_ROSSO "Carattere non valido, riprova.\n" COLORE_RESET);
            continue; //salta tutto il codice corrente e torna all'inizio del loop (FONDAMENTALE)
        }

        //Opzioni del menu
        switch (opzione) {
            case '0':
                printf(COLORE_GIALLO "Uscita dal gioco. Arrivederci!\n" COLORE_RESET);
                return; // Termina il programma
                
            case '1':
                gestisciNuovaPartita(); //gestione nuova partita
                break; // Torna al menu principale dopo la partita, esce dallo switch
                
            case '2':
                gestisciCaricaSalvataggio(); //gestione carica del salvataggio
                break; // Torna al menu principale dopo la partita, esce dallo switch
                
            case '3':
                if (trucchiAttivi) { // se i trucchi sono attivi
                    gestisciMenuTrucchi(); //gestione del menu trucchi
                } else { //altrimenti
                    printf(COLORE_ROSSO "Opzione non valida.\n" COLORE_RESET);
                }
                break; //esce dallo switch
                
            case ' ':
                // Terminatore della sequenza Konami
                if (i == 0) { //se il contatore dei caratteri konami rimane invariato
                    printf(COLORE_ROSSO "Errore: terminatore spazio inserito senza sequenza.\n" COLORE_RESET);
                } else { //altrimenti
                    if (confrontoString(contTrucchi)) { //se la stringa corrisponde alla stringa della sequenza konami
                        printf("\n" COLORE_VERDE "TRUCCHI ATTIVATI!\n" COLORE_RESET);
                        trucchiAttivi = true; //imposta i trucchi a true
                    } else {
                        printf("\n" COLORE_ROSSO "Codice errato. Riprova.\n" COLORE_RESET);
                    }
                    i = 0; //riparte con il conteggio a 0
                    contTrucchi[0] = '\0'; //imposta il terminatore del contenitore di trucchi per la partita successiva o per un successivo inserimento
                }
                break; //esce dallo switch
                
            default:
                // Se non siamo in modalità trucchi, accumula caratteri per il codice Konami
                if (!trucchiAttivi) { //se i trucchi non sono stati attivati
                    if (i < MAX_TRUCCHI - 1) { //finché il contatore i dei trucchi è inferiore alla dimensione massima meno uno (il meno uno serve per lasciare lo spazio al terminatore '\0')
                        contTrucchi[i++] = opzione; //scorro l'array e salvo il carattere
                        contTrucchi[i] = '\0'; //imposto quello successivo con il terminatore
                    } else { //altrimenti se i supera il massimo di caratteri ammessi
                        printf(COLORE_ROSSO "Sequenza troppo lunga, ripartiamo.\n" COLORE_RESET);
                        i = 0; //reimposta i a 0
                        contTrucchi[0] = '\0'; //imposta il terminatore del contenitore di trucchi per la partita successiva o per un successivo inserimento
                    }
                } else {
                    printf(COLORE_ROSSO "Opzione non valida.\n" COLORE_RESET);
                }
                break; //esce dallo switch
        }
    }
}

/**
 * @brief Gestisce l'avvio di una nuova partita
 * @details Questa funzione coordina tutte le operazioni necessarie per iniziare una nuova partita:
 *          1. Richiede il nome dell'eroe al giocatore
 *          2. Inizializza la struttura Eroe con i parametri di default
 *          3. Crea un salvataggio iniziale
 *          4. Mostra le caratteristiche dell'eroe
 *          5. Inizializza il gestore delle missioni
 *          6. Mostra il messaggio introduttivo del gioco
 *          7. Entra nel menu del villaggio (loop principale di gioco)
 * 
 * @return void Non restituisce alcun valore
 * 
 * @note La funzione crea automaticamente un salvataggio iniziale per permettere al giocatore
 *       di riprendere la partita in caso di uscita
 * 
 * @warning Se il salvataggio iniziale fallisce, viene mostrato un messaggio di errore ma
 *          il gioco continua comunque
 * 
 * @see Eroe Struttura dati dell'eroe
 * @see inizializzaEroe()
 * @see creaSalvataggioDaEroe()
 * @see salvaGioco()
 * @see mostraEroe()
 * @see inizializzaGestoreMissioni()
 * @see menuDelVillaggio()
 */
void gestisciNuovaPartita(void) {
    printf("\n" COLORE_VERDE "Hai scelto NUOVA PARTITA!\n" COLORE_RESET);
    
    //Crea un nuovo eroe
    Eroe eroe; ///< Crea una variabile eroe di tipo Eroe, un'istanza di Eroe (vedi struct Eroe.h)
    printf("Inserisci il nome del tuo eroe: ");
    dichiaraNomeEroe(eroe.nome); ///< Passo come parametro il campo della struct del mio eroe appena creato alla funzione 

    inizializzaEroe(&eroe, eroe.nome); ///< Inizializzazione dell'eroe passando l'indirizzo di memoria della mia variabile eroe e il nome senza caratteri fastidiosi nel buffer
    
    //Crea un salvataggio iniziale di tipo Salvataggio (vedi struct file salvataggi.h)
    Salvataggio s = creaSalvataggioDaEroe(&eroe); ///< Passo l'indirizzo di memoria della mia variabile eroe 

    if (salvaGioco(&s)) { //Se il gioco viene salvato correttamente (se la funzione mi torna true)
        printf(COLORE_VERDE "Salvataggio iniziale creato con successo!\n" COLORE_RESET);
    } else { //altrimenti (se mi torna false)
        printf(COLORE_ROSSO "Errore nel salvataggio iniziale.\n" COLORE_RESET);
    }

    mostraEroe(&eroe); ///< Mostra tutte le caratteristiche del mio eroe
    
    //Inizializza il gestore delle missioni
    GestoreMissioni gestore; ///< Creo un'istanza di GestoreMissioni
    inizializzaGestoreMissioni(&gestore); ///< Inizializzo il gestore delle missioni (vedi missioni.c)
    
    printf("\n" COLORE_CIANO "Benvenuto nel villaggio, %s!\n" COLORE_RESET, eroe.nome);
    printf(COLORE_GIALLO "Il capo del villaggio ti chiama...\n" COLORE_RESET);
    printf(COLORE_ROSSO "\"Un'oscura minaccia incombe sul regno. Sei la nostra ultima speranza!\"\n" COLORE_RESET);
    
    //Entra nel menu del villaggio (loop principale di gioco)
    bool continuaGioco = true; ///< Flag per controllare il loop di gioco
    while (continuaGioco) {
        continuaGioco = menuDelVillaggio(&eroe, &gestore);
    }
}

/**
 * @brief Gestisce il caricamento di un salvataggio esistente
 * @details Questa funzione coordina l'intero processo di caricamento di un salvataggio:
 *          1. Mostra l'elenco dei salvataggi disponibili
 *          2. Verifica se esistono salvataggi
 *          3. Chiede all'utente quale salvataggio selezionare
 *          4. Presenta un sottomenu con opzioni: Carica / Elimina / Annulla
 *          5. Se l'utente sceglie "Carica":
 *             - Carica i dati del salvataggio
 *             - Ricrea l'eroe con le caratteristiche salvate
 *             - Ripristina lo stato delle missioni completate
 *             - Entra nel menu del villaggio
 *          6. Se l'utente sceglie "Elimina":
 *             - Chiede conferma
 *             - Elimina definitivamente il salvataggio
 *          7. Se l'utente sceglie "Annulla":
 *             - Torna al menu principale
 * 
 * @return void Non restituisce alcun valore
 * 
 * @note La funzione include un meccanismo di sicurezza che richiede conferma esplicita
 *       prima di eliminare un salvataggio
 * 
 * @todo Implementare il salvataggio e il ripristino completo dello stato delle missioni
 *       (attualmente viene ripristinato solo il contatore delle missioni completate)
 * 
 * @warning Se non ci sono salvataggi disponibili, la funzione esce immediatamente
 *          senza mostrare ulteriori opzioni
 * 
 * @see mostraMenuSalvataggi()
 * @see contaSalvataggi()
 * @see chiediSalvataggioDaCaricare()
 * @see leggiSalvataggioIndice()
 * @see eliminaSalvataggio()
 * @see creaEroeDaSalvataggio()
 * @see menuDelVillaggio()
 */
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
    
    char scelta; ///< Variabile per memorizzare la scelta dell'utente
    
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
    Salvataggio s; ///< Struttura per contenere i dati del salvataggio caricato
    if (!leggiSalvataggioIndice(sceltaSalvataggio, &s)) {
        printf(COLORE_ROSSO "Errore nel caricamento del salvataggio.\n" COLORE_RESET);
        return;
    }
    
    Eroe eroeCaricato = creaEroeDaSalvataggio(&s); ///< Ricrea l'eroe dai dati del salvataggio
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

/**
 * @brief Gestisce l'accesso al menu trucchi
 * @details Questa funzione viene chiamata quando l'utente accede al menu trucchi
 *          (disponibile solo dopo aver attivato la sequenza Konami).
 *          Processo:
 *          1. Mostra l'elenco dei salvataggi disponibili
 *          2. Verifica se esistono salvataggi da modificare
 *          3. Chiede all'utente quale salvataggio modificare
 *          4. Se valido, passa il controllo a modificaCampoSalvataggio()
 * 
 * @return void Non restituisce alcun valore
 * 
 * @note Questa funzione è accessibile solo quando trucchiAttivi == true
 * 
 * @warning Se non ci sono salvataggi disponibili, la funzione esce immediatamente
 * 
 * @see modificaCampoSalvataggio()
 * @see mostraMenuSalvataggi()
 * @see contaSalvataggi()
 * @see chiediSalvataggioDaCaricare()
 */
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

/**
 * @brief Permette di modificare i campi di un salvataggio specifico
 * @details Questa funzione implementa il sistema di trucchi permettendo di modificare:
 *          1. Vita dell'eroe (aggiungere o sottrarre punti vita)
 *          2. Monete dell'eroe (aggiungere o sottrarre monete)
 *          3. Sblocco missione finale (completa automaticamente le 3 missioni preliminari)
 *          
 *          Processo:
 *          1. Mostra le opzioni di modifica disponibili
 *          2. Carica il salvataggio selezionato
 *          3. Applica la modifica scelta dall'utente
 *          4. Salva le modifiche nel file di salvataggio
 * 
 * @param[in] sceltaSalvataggio Indice del salvataggio da modificare (0-based)
 * 
 * @return void Non restituisce alcun valore
 * 
 * @note Le modifiche sono permanenti e sovrascrivono il salvataggio originale
 * 
 * @warning L'utente può inserire valori negativi per diminuire vita/monete.
 *          La funzione modificaVita/modificaMonete dovrebbe gestire i limiti appropriati.
 * 
 * @code
 * // Esempio di flusso:
 * // Utente seleziona salvataggio 1
 * // Sceglie opzione 1 (Vita)
 * // Inserisce +50 -> vita aumenta di 50 punti
 * // Modifiche salvate automaticamente
 * @endcode
 * 
 * @see leggiSalvataggioIndice()
 * @see creaEroeDaSalvataggio()
 * @see modificaVita()
 * @see modificaMonete()
 * @see creaSalvataggioDaEroe()
 * @see salvaGioco()
 */
void modificaCampoSalvataggio(int sceltaSalvataggio) {
    printf(COLORE_CIANO "\nHai scelto il salvataggio %d\n" COLORE_RESET, sceltaSalvataggio);
    printf("Cosa vuoi modificare nel salvataggio %d?\n", sceltaSalvataggio);
    printf("1. Vita\n");
    printf("2. Monete\n");
    printf("3. Sblocca Missione Finale\n");

    char scelta; ///< Variabile per memorizzare la scelta dell'utente
    
    while (1) {
        printf("Scegli un'opzione [1-3]: ");
        scelta = leggiCaratterePulito();

        if (scelta < '1' || scelta > '3') {
            printf(COLORE_ROSSO "Opzione non valida. Riprova.\n" COLORE_RESET);
            continue;
        }
        break;
    }

    Salvataggio s; ///< Struttura per contenere i dati del salvataggio
    if (!leggiSalvataggioIndice(sceltaSalvataggio, &s)) {
        printf(COLORE_ROSSO "Errore nel caricamento del salvataggio.\n" COLORE_RESET);
        return;
    }

    Eroe eroeModificato = creaEroeDaSalvataggio(&s); ///< Ricrea l'eroe dal salvataggio per modificarlo

    if (scelta == '1') {
        printf(COLORE_GIALLO "Modifica della VITA selezionata.\n" COLORE_RESET);
        printf("Di quanto vuoi modificare la vita? (usa numero negativo per diminuire): ");
        int delta; ///< Valore di modifica (positivo o negativo)
        scanf("%d", &delta);
        pulisciBuffer();  
        modificaVita(&eroeModificato, delta);
        printf(COLORE_VERDE "Vita modificata! Nuova vita: %d\n" COLORE_RESET, eroeModificato.vita);
        
    } else if (scelta == '2') {
        printf(COLORE_GIALLO "Modifica delle MONETE selezionata.\n" COLORE_RESET);
        printf("Di quanto vuoi modificare le monete? (usa numero negativo per diminuire): ");
        int delta; ///< Valore di modifica (positivo o negativo)
        scanf("%d", &delta);
        pulisciBuffer();  
        modificaMonete(&eroeModificato, delta);
        printf(COLORE_VERDE "Monete modificate! Nuove monete: %d\n" COLORE_RESET, eroeModificato.monete);
        
    } else if (scelta == '3') {
        printf(COLORE_MAGENTA "Sblocco della MISSIONE FINALE selezionata.\n" COLORE_RESET);
        eroeModificato.missioniCompletate = 3; ///< Imposta a 3 per sbloccare la missione finale
        printf(COLORE_VERDE "Missione finale sbloccata! Tutte le missioni preliminari sono ora completate.\n" COLORE_RESET);
    }

    Salvataggio sModificato = creaSalvataggioDaEroe(&eroeModificato); ///< Crea un nuovo salvataggio con le modifiche
    if (salvaGioco(&sModificato)) {
        printf(COLORE_VERDE "Modifica salvata con successo!\n" COLORE_RESET);
    } else {
        printf(COLORE_ROSSO "Errore nel salvataggio delle modifiche.\n" COLORE_RESET);
    }
}

/**
 * @brief Loop principale del gioco - Menu del villaggio
 * @details Questa è la funzione centrale che gestisce il gameplay nel villaggio.
 *          Presenta al giocatore le seguenti opzioni:
 *          1. Intraprendi una missione - permette di selezionare ed eseguire missioni
 *          2. Riposati - ripristina i punti vita dell'eroe al massimo
 *          3. Inventario - mostra le statistiche e l'inventario dell'eroe
 *          4. Salva la partita - salva i progressi attuali
 *          5. Esci - torna al menu principale (con conferma)
 *          
 *          La funzione continua a ciclare finché il giocatore non sceglie di uscire
 *          o finché l'eroe non viene sconfitto (vita = 0).
 * 
 * @param[in,out] eroe Puntatore alla struttura dell'eroe (modificabile durante il gioco)
 * @param[in,out] gestore Puntatore al gestore delle missioni (traccia missioni completate)
 * 
 * @return bool true se il gioco deve continuare, false per uscire al menu principale
 * 
 * @retval true Il giocatore ha scelto di continuare a giocare
 * @retval false Il giocatore ha scelto di uscire al menu principale
 * 
 * @note Questa funzione valida i puntatori all'inizio e restituisce false se nulli
 * 
 * @warning Se l'eroe muore durante una missione, il gioco dovrebbe gestirlo appropriatamente
 *          (attualmente non implementato in questa funzione)
 * 
 * @see stampaMenuVillaggio()
 * @see selezionaMissione()
 * @see eseguiMissione()
 * @see riposatiAlVillaggio()
 * @see mostraInventario()
 * @see salvaPartitaCorrente()
 * @see gestisciUscita()
 */
bool menuDelVillaggio(Eroe* eroe, GestoreMissioni* gestore) {
    if (eroe == NULL || gestore == NULL) return false;
    
    stampaMenuVillaggio();
    
    printf("Seleziona una delle opzioni del menu [1-5]: ");
    
    char scelta = leggiCaratterePulito(); ///< Legge la scelta dell'utente
    
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

/**
 * @brief Permette all'eroe di riposarsi e recuperare i punti vita
 * @details Questa funzione simula il riposo alla locanda del villaggio.
 *          Se l'eroe non è già al massimo della vita (20/20), ripristina
 *          completamente i suoi punti vita e mostra quanti punti sono stati recuperati.
 *          Se l'eroe è già in perfetta salute, lo notifica senza fare modifiche.
 * 
 * @param[in,out] eroe Puntatore alla struttura dell'eroe da far riposare
 * 
 * @return void Non restituisce alcun valore
 * 
 * @note La funzione esegue un controllo di sicurezza sul puntatore null prima di operare
 * 
 * @pre L'eroe deve avere vita >= 0 e <= 20
 * @post L'eroe avrà vita = 20 (massimo)
 * 
 * @code
 * // Esempio di utilizzo:
 * Eroe mioEroe;
 * mioEroe.vita = 10; // Eroe ferito
 * riposatiAlVillaggio(&mioEroe);
 * // Output: "Hai recuperato 10 punti vita!"
 * // mioEroe.vita è ora 20
 * @endcode
 * 
 * @warning Se eroe è NULL, la funzione ritorna immediatamente senza fare nulla
 */
void riposatiAlVillaggio(Eroe* eroe) {
    if (eroe == NULL) return;
    
    printf("\n" COLORE_VERDE "Ti riposi alla locanda del villaggio...\n" COLORE_RESET);
    
    if (eroe->vita >= 20) {
        printf(COLORE_CIANO "Sei già in perfetta salute! (20/20 punti vita)\n" COLORE_RESET);
    } else {
        int vitaPrecedente = eroe->vita; ///< Memorizza la vita prima del riposo per calcolare il recupero
        eroe->vita = 20; ///< Imposta la vita al massimo
        printf(COLORE_VERDE "Hai recuperato %d punti vita!\n" COLORE_RESET, 20 - vitaPrecedente);
        printf(COLORE_CIANO "Punti vita: %d/20\n" COLORE_RESET, eroe->vita);
    }
    
    printf(COLORE_GIALLO "Sei pronto per nuove avventure!\n" COLORE_RESET);
}

/**
 * @brief Mostra l'inventario e le statistiche dell'eroe
 * @details Questa funzione fornisce una visualizzazione dettagliata di tutte le
 *          caratteristiche dell'eroe chiamando la funzione mostraEroe().
 *          Include tipicamente: nome, vita, monete, missioni completate, ecc.
 * 
 * @param[in] eroe Puntatore costante alla struttura dell'eroe da visualizzare
 * 
 * @return void Non restituisce alcun valore
 * 
 * @note Il parametro è const perché questa funzione è di sola lettura
 * @note La funzione esegue un controllo di sicurezza sul puntatore null prima di operare
 * 
 * @warning Se eroe è NULL, la funzione ritorna immediatamente senza fare nulla
 * 
 * @see mostraEroe()
 */
void mostraInventario(const Eroe* eroe) {
    if (eroe == NULL) return;
    
    printf("\n" COLORE_CIANO "INVENTARIO\n" COLORE_RESET);
    mostraEroe(eroe);
}

/**
 * @brief Salva lo stato corrente della partita
 * @details Questa funzione crea un salvataggio dei progressi attuali dell'eroe.
 *          Il salvataggio viene scritto su file e può essere successivamente
 *          caricato dal menu "Carica salvataggio".
 *          
 *          Processo:
 *          1. Converte i dati dell'eroe in una struttura Salvataggio
 *          2. Scrive il salvataggio su file
 *          3. Conferma l'operazione o segnala eventuali errori
 * 
 * @param[in] eroe Puntatore costante alla struttura dell'eroe da salvare
 * 
 * @return void Non restituisce alcun valore
 * 
 * @note Il parametro è const perché questa funzione non modifica l'eroe
 * @note La funzione esegue un controllo di sicurezza sul puntatore null prima di operare
 * 
 * @warning Se eroe è NULL, la funzione ritorna immediatamente senza fare nulla
 * @warning Se il salvataggio fallisce, viene mostrato un messaggio di errore ma
 *          il gioco continua normalmente
 * 
 * @see creaSalvataggioDaEroe()
 * @see salvaGioco()
 */
void salvaPartitaCorrente(const Eroe* eroe) {
    if (eroe == NULL) return;
    
    printf("\n" COLORE_GIALLO "Salvataggio della partita in corso...\n" COLORE_RESET);
    
    Salvataggio s = creaSalvataggioDaEroe(eroe); ///< Converte i dati dell'eroe in formato salvataggio
    
    if (salvaGioco(&s)) {
        printf(COLORE_VERDE "Partita salvata con successo!\n" COLORE_RESET);
    } else {
        printf(COLORE_ROSSO "Errore durante il salvataggio.\n" COLORE_RESET);
    }
}

/**
 * @brief Gestisce la conferma di uscita dal gioco
 * @details Questa funzione implementa un meccanismo di sicurezza che previene
 *          la perdita accidentale dei progressi. Quando l'utente sceglie di uscire:
 *          1. Mostra un avviso di ricordarsi di salvare
 *          2. Chiede conferma esplicita (S/N)
 *          3. Se conferma, esce dal loop di gioco
 *          4. Se non conferma, riprende il gioco
 * 
 * @return bool Indica se il gioco deve continuare o terminare
 * 
 * @retval true Il giocatore vuole continuare a giocare (ha annullato l'uscita)
 * @retval false Il giocatore conferma l'uscita (torna al menu principale)
 * 
 * @note Questa funzione NON salva automaticamente - è responsabilità del giocatore
 *       salvare prima di uscire
 * 
 * @code
 * // Esempio di utilizzo nel menu:
 * case '5':
 *     return gestisciUscita(); // Il valore di ritorno determina se continuare
 * @endcode
 * 
 * @see menuDelVillaggio()
 */
bool gestisciUscita(void) {
    printf("\n" COLORE_GIALLO "Stai uscendo dal gioco.\n" COLORE_RESET);
    printf(COLORE_ROSSO "Ricordati di salvare la partita per non perdere i tuoi progressi!\n" COLORE_RESET);
    printf("Sei sicuro di voler procedere? [S/N]: ");
    
    char conferma = leggiCaratterePulito(); ///< Legge la risposta dell'utente
    
    if (conferma == 'S' || conferma == 's') {
        printf(COLORE_GIALLO "Tornando al menu principale...\n" COLORE_RESET);
        return false; // Esce dal loop del villaggio
    } else {
        printf(COLORE_VERDE "Continuiamo l'avventura!\n" COLORE_RESET);
        return true; // Continua il gioco
    }
}

/**
 * @defgroup MenuDisplay Funzioni di visualizzazione menu
 * @brief Funzioni statiche per la stampa formattata dei menu
 * @details Queste funzioni sono responsabili della visualizzazione grafica dei menu.
 *          Sono dichiarate static per limitarne la visibilità a questo file.
 * @{
 */

/**
 * @brief Stampa il menu principale senza l'opzione trucchi
 * @details Visualizza un riquadro decorativo ASCII con bordi blu contenente le opzioni:
 *          - 1. Nuova partita
 *          - 2. Carica salvataggio
 *          - 0. Esci
 *          
 *          Questa versione viene mostrata quando trucchiAttivi == false
 * 
 * @return void Non restituisce alcun valore
 * 
 * @note Funzione statica - visibile solo in questo file
 * @note Utilizza codici ANSI per colorare il bordo in blu
 * 
 * @see stampaMenuConRiquadroAtreOpzioni()
 * @see menuPrincipale()
 */
static void stampaMenuConRiquadroADueOpzioni(void)
{
    const char *blu = "\033[94m";     ///< Codice ANSI per colore blu chiaro
    const char *reset = "\033[0m";    ///< Codice ANSI per ripristinare il colore

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

/**
 * @brief Stampa il menu principale con l'opzione trucchi
 * @details Visualizza un riquadro decorativo ASCII con bordi blu contenente le opzioni:
 *          - 1. Nuova partita
 *          - 2. Carica salvataggio
 *          - 3. Trucchi (opzione aggiuntiva)
 *          - 0. Esci
 *          
 *          Questa versione viene mostrata quando trucchiAttivi == true
 * 
 * @return void Non restituisce alcun valore
 * 
 * @note Funzione statica - visibile solo in questo file
 * @note Utilizza codici ANSI per colorare il bordo in blu
 * @note L'opzione "3. Trucchi" appare solo dopo aver inserito la sequenza Konami
 * 
 * @see stampaMenuConRiquadroADueOpzioni()
 * @see menuPrincipale()
 */
static void stampaMenuConRiquadroAtreOpzioni(void)
{
    const char *blu = "\033[94m";     ///< Codice ANSI per colore blu chiaro
    const char *reset = "\033[0m";    ///< Codice ANSI per ripristinare il colore

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

/**
 * @brief Stampa il menu del villaggio
 * @details Visualizza le opzioni disponibili quando l'eroe si trova nel villaggio:
 *          - 1. Intraprendi una missione
 *          - 2. Riposati
 *          - 3. Inventario
 *          - 4. Salva la partita
 *          - 5. Esci
 *          
 *          Il menu è incorniciato da linee verdi per distinguerlo dal menu principale
 * 
 * @return void Non restituisce alcun valore
 * 
 * @note Funzione statica - visibile solo in questo file
 * @note Utilizza codici ANSI per colorare il titolo e i bordi in verde
 * 
 * @see menuDelVillaggio()
 */
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

/** @} */ // Fine gruppo MenuDisplay