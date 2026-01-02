/**
 * @file missioni.c
 * @brief Implementazione del sistema di gestione delle missioni
 * 
 * Questo file contiene l'implementazione completa del sistema di missioni
 * del gioco, incluse le funzioni per inizializzare, visualizzare, selezionare
 * ed eseguire le missioni disponibili.
 * 
 * @author 
 * @date
 * @version 
 */

#include "missioni.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Colori ANSI per output colorato
#define COLORE_VERDE "\033[1;32m"      /**< Codice ANSI per testo verde brillante */
#define COLORE_ROSSO "\033[1;31m"      /**< Codice ANSI per testo rosso brillante */
#define COLORE_GIALLO "\033[1;33m"     /**< Codice ANSI per testo giallo brillante */
#define COLORE_BLU "\033[1;34m"        /**< Codice ANSI per testo blu brillante */
#define COLORE_CIANO "\033[1;36m"      /**< Codice ANSI per testo ciano brillante */
#define COLORE_MAGENTA "\033[1;35m"    /**< Codice ANSI per testo magenta brillante */
#define COLORE_RESET "\033[0m"         /**< Codice ANSI per reset formattazione */


// --- FUNZIONI DI INIZIALIZZAZIONE ---

/**
 * @brief Inizializza una singola missione con i parametri specificati
 * 
 * Questa funzione configura tutti i campi di una struttura Missione con i valori
 * iniziali appropriati. Il nome e la descrizione vengono copiati in modo sicuro
 * usando strncpy con terminazione garantita della stringa.
 * 
 * @param[out] m Puntatore alla struttura Missione da inizializzare
 * @param[in] tipo Tipo della missione (MISSIONE_PALUDE, MISSIONE_MAGIONE, etc.)
 * @param[in] nome Nome descrittivo della missione (max 49 caratteri)
 * @param[in] descrizione Descrizione dettagliata dell'obiettivo (max 199 caratteri)
 * @param[in] obiettiviTotali Numero totale di obiettivi da completare
 * 
 * @note Se m è NULL, la funzione ritorna immediatamente senza operazioni
 * @note Le stringhe vengono troncate se superano la lunghezza massima consentita
 * @note Tutte le missioni sono sbloccate di default tranne MISSIONE_CASTELLO
 * 
 * @warning Il puntatore m deve essere valido e allocato prima della chiamata
 */
void inizializzaMissione(Missione* m, TipoMissione tipo, const char* nome, 
                         const char* descrizione, int obiettiviTotali) {
    if (m == NULL) return;
    
    m->tipo = tipo;
    strncpy(m->nome, nome, 49);
    m->nome[49] = '\0';
    strncpy(m->descrizione, descrizione, 199);
    m->descrizione[199] = '\0';
    m->completata = false;
    m->sbloccata = (tipo != MISSIONE_CASTELLO); // Tutte sbloccate tranne la finale
    m->obiettiviCompletati = 0;
    m->obiettiviTotali = obiettiviTotali;
    m->oggettoRecuperato = false;
}

/**
 * @brief Inizializza il gestore delle missioni con tutte le missioni del gioco
 * 
 * Questa funzione configura il GestoreMissioni con tutte e quattro le missioni
 * disponibili nel gioco, impostando i loro parametri iniziali. Utilizza lo stile
 * di formattazione Vertical-Alignment-Style (Antirez).
 * 
 * Le missioni inizializzate sono:
 * - MISSIONE_PALUDE: "Palude Putrescente" - 3 Generali Orco da eliminare
 * - MISSIONE_MAGIONE: "Magione Infestata" - 1 Vampiro Superiore + recupero chiave
 * - MISSIONE_GROTTA: "Grotta di Cristallo" - Recupero Spada dell'Eroe
 * - MISSIONE_CASTELLO: "Castello del Signore Oscuro" - Boss finale (bloccata)
 * 
 * @param[out] gestore Puntatore alla struttura GestoreMissioni da inizializzare
 * 
 * @pre gestore deve essere un puntatore valido ad una struttura allocata
 * @post Tutte le missioni sono inizializzate con i loro parametri di default
 * @post missioniCompletate è impostato a 0
 * @post missioneCorrente è impostato a MISSIONE_NESSUNA
 * @post Solo MISSIONE_CASTELLO inizia in stato bloccato
 * 
 * @note Se gestore è NULL, la funzione ritorna immediatamente senza operazioni
 */
void inizializzaGestoreMissioni(GestoreMissioni* gestore) {
    if (gestore == NULL) return;//se il puntatore e' nulla e quindi non punta a nulla esce
    
    gestore->missioniCompletate = 0;                //Inizializza a 0 i campi gel gestore
    gestore->missioneCorrente = MISSIONE_NESSUNA;   //Inizializza la missione attuale (vedi typedef Enum)
    
    //Inizializza Palude Putrescente
    inizializzaMissione(&gestore->missioni[MISSIONE_PALUDE], 
                       MISSIONE_PALUDE,
                       "Palude Putrescente",
                       "Sconfiggi 3 Generale Orco del Signore Oscuro",
                       3); // 3 Generali Orco da eliminare
    
    // Inizializza Magione Infestata
    inizializzaMissione(&gestore->missioni[MISSIONE_MAGIONE],
                       MISSIONE_MAGIONE,
                       "Magione Infestata",
                       "Recupera la chiave del Castello e sconfiggi un Vampiro Superiore",
                       1); // 1 Vampiro Superiore da eliminare
    
    // Inizializza Grotta di Cristallo
    inizializzaMissione(&gestore->missioni[MISSIONE_GROTTA],
                       MISSIONE_GROTTA,
                       "Grotta di Cristallo",
                       "Recupera la Spada dell'Eroe",
                       0); // Nessun nemico specifico, solo recuperare la spada
    
    // Inizializza Castello del Signore Oscuro (bloccata inizialmente)
    inizializzaMissione(&gestore->missioni[MISSIONE_CASTELLO],
                       MISSIONE_CASTELLO,
                       "Castello del Signore Oscuro",
                       "Sconfiggi il Signore Oscuro",
                       1); // 1 boss finale
}

// --- FUNZIONI DI VISUALIZZAZIONE ---

/**
 * @brief Mostra il menu di selezione delle missioni disponibili
 * 
 * Visualizza un menu formattato con tutte le missioni attualmente disponibili
 * per il giocatore. Vengono mostrate solo le missioni non completate e sbloccate.
 * Il menu include il nome e la descrizione di ciascuna missione disponibile.
 * La missione finale viene evidenziata con una marcatura speciale.
 * 
 * @param[in] gestore Puntatore costante al GestoreMissioni
 * 
 * @return Numero di missioni disponibili (0 se tutte completate o gestore è NULL)
 * 
 * @pre gestore deve essere un puntatore valido
 * @post Visualizza il menu formattato su stdout
 * 
 * @note Se non ci sono missioni disponibili, mostra un messaggio di completamento
 * @note Utilizza codici colore ANSI per migliorare la visualizzazione
 * 
 * @see selezionaMissione()
 * @see TipoMissione
 */
int mostraMenuMissioni(const GestoreMissioni* gestore) {
    if (gestore == NULL) return 0;
    
    printf("\n");
    printf(COLORE_CIANO "----------------------------------------------\n" COLORE_RESET);
    printf(COLORE_CIANO "     MENU DI SELEZIONE MISSIONE             \n" COLORE_RESET);
    printf(COLORE_CIANO "----------------------------------------------\n" COLORE_RESET);
    
    int disponibili = 0;
    
    // Mostra solo le missioni non completate e sbloccate
    for (int i = 0; i < 4; i++) {
        const Missione* m = &gestore->missioni[i];
        
        if (!m->completata && m->sbloccata) {
            disponibili++;
            printf(COLORE_GIALLO "%d. %s\n" COLORE_RESET, disponibili, m->nome);
            printf("   Obiettivo: %s\n", m->descrizione);
            
            // Mostra icona speciale per la missione finale
            if (m->tipo == MISSIONE_CASTELLO) {
                printf(COLORE_ROSSO "    MISSIONE FINALE    \n" COLORE_RESET);
            }
            printf("\n");
        }
    }
    
    if (disponibili == 0) {
        printf(COLORE_VERDE "Tutte le missioni sono state completate!\n" COLORE_RESET);
    }
    
    return disponibili;
}

/**
 * @brief Visualizza lo stato dettagliato di una missione specifica
 * 
 * Mostra informazioni complete sulla missione corrente, inclusi:
 * - Nome e descrizione della missione
 * - Progresso degli obiettivi (se applicabile) con barra visuale
 * - Stato del recupero di oggetti speciali (chiave o spada)
 * 
 * La funzione adatta la visualizzazione in base al tipo di missione:
 * - Per missioni con obiettivi numerici mostra una barra di progresso
 * - Per MISSIONE_MAGIONE mostra lo stato della chiave
 * - Per MISSIONE_GROTTA mostra lo stato della spada
 * 
 * @param[in] missione Puntatore costante alla Missione da visualizzare
 * 
 * @pre missione deve essere un puntatore valido
 * @post Stampa le informazioni formattate su stdout
 * 
 * @note Se missione è NULL, la funzione ritorna immediatamente
 * @note La barra di progresso usa caratteri Unicode per una migliore visualizzazione
 * 
 * @see TipoMissione
 * @see Missione
 */
void mostraStatoMissione(const Missione* missione) {
    if (missione == NULL) return;
    
    printf("\n");
    printf(COLORE_BLU "---------------------------------------\n" COLORE_RESET);
    printf(COLORE_BLU "  MISSIONE: %s\n" COLORE_RESET, missione->nome);
    printf(COLORE_BLU "---------------------------------------\n" COLORE_RESET);
    printf(COLORE_GIALLO "Obiettivo: " COLORE_RESET "%s\n", missione->descrizione);
    
    // Mostra progresso solo se ci sono obiettivi numerici
    if (missione->obiettiviTotali > 0) {
        printf(COLORE_CIANO "Stato di avanzamento: " COLORE_RESET);
        printf("Eliminati %d su %d", missione->obiettiviCompletati, missione->obiettiviTotali);
        
        // Mostra barra di progresso
        printf(" [");
        for (int i = 0; i < missione->obiettiviTotali; i++) {
            if (i < missione->obiettiviCompletati) {
                printf(COLORE_VERDE "█" COLORE_RESET);
            } else {
                printf("░");
            }
        }
        printf("]\n");
    }
    
    // Mostra se l'oggetto speciale è stato recuperato
    if (missione->tipo == MISSIONE_MAGIONE) {
        if (missione->oggettoRecuperato) {
            printf(COLORE_VERDE "Chiave del Castello: RECUPERATA\n" COLORE_RESET);
        } else {
            printf(COLORE_ROSSO "Chiave del Castello: NON ANCORA TROVATA\n" COLORE_RESET);
        }
    } else if (missione->tipo == MISSIONE_GROTTA) {
        if (missione->oggettoRecuperato) {
            printf(COLORE_VERDE "Spada dell'Eroe: RECUPERATA\n" COLORE_RESET);
        } else {
            printf(COLORE_ROSSO "Spada dell'Eroe: NON ANCORA TROVATA\n" COLORE_RESET);
        }
    }
    
    printf(COLORE_BLU "------------------------------------------\n" COLORE_RESET);
}

/**
 * @brief Visualizza il menu di azioni disponibili durante una missione
 * 
 * Mostra il menu interattivo che il giocatore vede mentre è all'interno di una
 * missione. Include lo stato corrente della missione e le opzioni disponibili:
 * - Esplora stanza del Dungeon
 * - Negozio
 * - Inventario
 * - Torna al Villaggio (con indicazione del costo se applicabile)
 * 
 * L'opzione di ritorno al villaggio mostra il costo di 50 monete solo se
 * la missione non è ancora completata (obiettivi non raggiunti o oggetto
 * speciale non recuperato).
 * 
 * @param[in] missione Puntatore costante alla Missione corrente
 * @param[in] eroe Puntatore costante all'Eroe (per visualizzare statistiche)
 * 
 * @pre missione e eroe devono essere puntatori validi
 * @post Visualizza lo stato della missione e il menu su stdout
 * 
 * @note Se missione o eroe sono NULL, la funzione ritorna immediatamente
 * @note Il costo per tornare al villaggio è 50 monete se la missione non è completa
 * 
 * @see mostraStatoMissione()
 * @see obiettiviRaggiunti()
 * @see Eroe
 */
void mostraMenuDuranteMissione(const Missione* missione, const Eroe* eroe) {
    if (missione == NULL || eroe == NULL) return;
    
    mostraStatoMissione(missione);
    
    printf("\n" COLORE_CIANO "Menu di Missione:\n" COLORE_RESET);
    printf("1. Esplora stanza del Dungeon\n");
    printf("2. Negozio\n");
    printf("3. Inventario\n");
    printf("4. Torna al Villaggio");
    
    // Indica il costo per tornare se la missione non è completa
    if (!obiettiviRaggiunti(missione) || 
        (missione->tipo != MISSIONE_CASTELLO && !missione->oggettoRecuperato)) {
        printf(" " COLORE_GIALLO "(Paga 50 Monete)" COLORE_RESET);
    }
    printf("\n");
}

// --- FUNZIONI DI SELEZIONE ---

/**
 * @brief Permette al giocatore di selezionare una missione dal menu
 * 
 * Mostra il menu delle missioni disponibili e gestisce l'input dell'utente
 * per la selezione. Valida l'input e restituisce il tipo di missione
 * corrispondente alla scelta del giocatore.
 * 
 * Il processo di selezione:
 * 1. Mostra tutte le missioni disponibili
 * 2. Richiede input numerico all'utente
 * 3. Valida la scelta (deve essere nel range valido)
 * 4. Mappa la scelta numerica alla missione corrispondente
 * 
 * @param[in,out] gestore Puntatore al GestoreMissioni
 * 
 * @return TipoMissione selezionato dall'utente, o MISSIONE_NESSUNA se:
 *         - gestore è NULL
 *         - Non ci sono missioni disponibili
 *         - L'input dell'utente non è valido
 *         - Si verifica un errore di lettura
 * 
 * @pre gestore deve essere un puntatore valido
 * @post Non modifica lo stato del gestore
 * 
 * @note Utilizza fgets per una lettura sicura dell'input
 * @note Mostra messaggio di errore per scelte non valide
 * 
 * @see mostraMenuMissioni()
 * @see TipoMissione
 */
TipoMissione selezionaMissione(GestoreMissioni* gestore) {
    if (gestore == NULL) return MISSIONE_NESSUNA;
    
    int disponibili = mostraMenuMissioni(gestore);
    
    if (disponibili == 0) {
        return MISSIONE_NESSUNA;
    }
    
    printf("Seleziona una delle opzioni del menu [1-%d]: ", disponibili);
    
    int scelta;
    char buffer[10];
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return MISSIONE_NESSUNA;
    }
    
    scelta = atoi(buffer);
    
    if (scelta < 1 || scelta > disponibili) {
        printf(COLORE_ROSSO "Scelta non valida!\n" COLORE_RESET);
        return MISSIONE_NESSUNA;
    }
    
    // Mappa la scelta alla missione corrispondente
    int contatore = 0;
    for (int i = 0; i < 4; i++) {
        if (!gestore->missioni[i].completata && gestore->missioni[i].sbloccata) {
            contatore++;
            if (contatore == scelta) {
                return (TipoMissione)i;
            }
        }
    }
    
    return MISSIONE_NESSUNA;
}

// --- FUNZIONI DI GESTIONE ---

/**
 * @brief Esegue il loop principale di una missione selezionata
 * 
 * Questa funzione gestisce l'intera esecuzione di una missione, fornendo
 * un menu interattivo che permette al giocatore di:
 * - Esplorare il dungeon (da implementare)
 * - Visitare il negozio (da implementare)
 * - Controllare l'inventario
 * - Tornare al villaggio (gratis se completata, 50 monete altrimenti)
 * 
 * La missione continua finché il giocatore non decide di tornare al villaggio.
 * Se la missione è completata (obiettivi raggiunti e oggetti recuperati),
 * viene automaticamente marcata come completata.
 * 
 * @param[in,out] gestore Puntatore al GestoreMissioni
 * @param[in,out] eroe Puntatore all'Eroe che esegue la missione
 * @param[in] tipo Tipo di missione da eseguire
 * 
 * @return true se la missione è stata completata, false altrimenti
 * 
 * @retval true La missione è stata completata con successo
 * @retval false La missione non è disponibile, è già completata, o parametri non validi
 * 
 * @pre gestore ed eroe devono essere puntatori validi
 * @pre tipo deve essere un valore valido tra 0 e 3
 * @pre La missione deve essere sbloccata e non ancora completata
 * 
 * @post gestore->missioneCorrente è impostato a tipo durante l'esecuzione
 * @post gestore->missioneCorrente è reimpostato a MISSIONE_NESSUNA alla fine
 * @post Le monete dell'eroe possono essere ridotte di 50 se torna senza completare
 * @post La missione può essere marcata come completata
 * 
 * @note Questa funzione contiene segnaposto (TODO) per sistemi non ancora implementati
 * @note Il ritorno anticipato costa 50 monete se la missione non è completa
 * 
 * @warning Attualmente alcune funzionalità sono stub e richiedono implementazione
 * 
 * @see completaMissione()
 * @see obiettiviRaggiunti()
 * @see mostraMenuDuranteMissione()
 */
bool eseguiMissione(GestoreMissioni* gestore, Eroe* eroe, TipoMissione tipo) {
    if (gestore == NULL || eroe == NULL || tipo < 0 || tipo > 3) {
        return false;
    }
    
    Missione* missione = &gestore->missioni[tipo];
    
    if (!missione->sbloccata || missione->completata) {
        printf(COLORE_ROSSO "Questa missione non è disponibile!\n" COLORE_RESET);
        return false;
    }
    
    gestore->missioneCorrente = tipo;
    
    printf(COLORE_VERDE "\nInizia la missione: %s\n" COLORE_RESET, missione->nome);
    printf(COLORE_MAGENTA "Che l'avventura abbia inizio!\n" COLORE_RESET);
    
    // TODO: Qui verrà integrato il sistema di dungeon e combattimento
    // Per ora mostriamo solo il menu
    
    bool missioneInCorso = true;
    
    while (missioneInCorso) {
        mostraMenuDuranteMissione(missione, eroe);
        
        printf("\nSeleziona una delle opzioni del menu [1-4]: ");
        
        char scelta;
        int c;
        scelta = getchar();
        while ((c = getchar()) != '\n' && c != EOF);
        
        switch (scelta) {
            case '1':
                printf(COLORE_GIALLO "Esplorazione del dungeon... (DA IMPLEMENTARE)\n" COLORE_RESET);
                // TODO: Chiamare la funzione di esplorazione dungeon
                break;
                
            case '2':
                printf(COLORE_GIALLO "Negozio... (DA IMPLEMENTARE)\n" COLORE_RESET);
                // TODO: Aprire il negozio
                break;
                
            case '3':
                printf(COLORE_CIANO "Inventario:\n" COLORE_RESET);
                mostraEroe(eroe);
                break;
                
            case '4':
                // Verifica se può tornare gratuitamente
                if (obiettiviRaggiunti(missione) && 
                    (missione->tipo == MISSIONE_CASTELLO || missione->oggettoRecuperato)) {
                    printf(COLORE_VERDE "Missione completata! Torni al villaggio.\n" COLORE_RESET);
                    completaMissione(gestore, tipo);
                    missioneInCorso = false;
                } else if (eroe->monete >= 50) {
                    printf(COLORE_GIALLO "Paghi 50 monete per tornare al villaggio.\n" COLORE_RESET);
                    modificaMonete(eroe, -50);
                    missioneInCorso = false;
                } else {
                    printf(COLORE_ROSSO "Non hai abbastanza monete! (Servono 50 monete)\n" COLORE_RESET);
                }
                break;
                
            default:
                printf(COLORE_ROSSO "Opzione non valida!\n" COLORE_RESET);
                break;
        }
    }
    
    gestore->missioneCorrente = MISSIONE_NESSUNA;
    return missione->completata;
}

/**
 * @brief Marca una missione come completata e gestisce gli effetti collaterali
 * 
 * Questa funzione:
 * - Marca la missione specificata come completata
 * - Incrementa il contatore delle missioni completate
 * - Mostra un messaggio celebrativo
 * - Verifica se tutte le missioni preliminari sono complete
 * - Sblocca la missione finale se le condizioni sono soddisfatte
 * 
 * @param[in,out] gestore Puntatore al GestoreMissioni
 * @param[in] tipo Tipo della missione da completare
 * 
 * @pre gestore deve essere un puntatore valido
 * @pre tipo deve essere un valore valido (0-3)
 * 
 * @post missione->completata è impostato a true
 * @post gestore->missioniCompletate è incrementato
 * @post MISSIONE_CASTELLO può essere sbloccata se tutte le preliminari sono complete
 * 
 * @note Se la missione è già completata, la funzione ritorna senza modifiche
 * @note Se gestore è NULL o tipo non è valido, la funzione ritorna senza operazioni
 * 
 * @see sbloccaMissioneFinale()
 * @see tutteLePreliminariCompletate()
 * @see TipoMissione
 */
void completaMissione(GestoreMissioni* gestore, TipoMissione tipo) {
    if (gestore == NULL || tipo < 0 || tipo > 3) return;
    
    Missione* m = &gestore->missioni[tipo];
    
    if (m->completata) return;
    
    m->completata = true;
    gestore->missioniCompletate++;
    
    printf("\n");
    printf(COLORE_VERDE "----------------------------------------------\n" COLORE_RESET);
    printf(COLORE_VERDE "            MISSIONE COMPLETATA!            \n" COLORE_RESET);
    printf(COLORE_VERDE "----------------------------------------------\n" COLORE_RESET);
    printf(COLORE_GIALLO "Hai completato: %s\n" COLORE_RESET, m->nome);
    
    // Sblocca la missione finale se tutte le preliminari sono complete
    if (tutteLePreliminariCompletate(gestore)) {
        sbloccaMissioneFinale(gestore);
    }
}

/**
 * @brief Incrementa il contatore degli obiettivi completati di una missione
 * 
 * Aumenta di uno il numero di obiettivi completati per la missione specificata.
 * Fornisce feedback visivo all'utente riguardo al progresso della missione:
 * - Messaggio di completamento se tutti gli obiettivi sono raggiunti
 * - Messaggio di progresso con conteggio corrente altrimenti
 * 
 * Il contatore non può superare il numero totale di obiettivi della missione.
 * 
 * @param[in,out] missione Puntatore alla Missione da aggiornare
 * 
 * @pre missione deve essere un puntatore valido
 * @post missione->obiettiviCompletati è incrementato di 1
 * @post Il valore viene limitato a obiettiviTotali se necessario
 * @post Viene stampato un messaggio di feedback su stdout
 * 
 * @note Se missione è NULL, la funzione ritorna senza operazioni
 * @note Il valore massimo è limitato a obiettiviTotali anche se incrementato oltre
 * 
 * @see obiettiviRaggiunti()
 * @see Missione
 */
void incrementaObiettivi(Missione* missione) {
    if (missione == NULL) return;
    
    missione->obiettiviCompletati++;
    
    if (missione->obiettiviCompletati > missione->obiettiviTotali) {
        missione->obiettiviCompletati = missione->obiettiviTotali;
    }
    
    // Feedback visivo
    if (obiettiviRaggiunti(missione)) {
        printf(COLORE_VERDE "Obiettivi della missione raggiunti!\n" COLORE_RESET);
    } else {
        printf(COLORE_CIANO "Progresso: %d/%d obiettivi completati\n" COLORE_RESET,
               missione->obiettiviCompletati, missione->obiettiviTotali);
    }
}

/**
 * @brief Segna un oggetto speciale come recuperato nella missione
 * 
 * Imposta il flag oggettoRecuperato a true per la missione specificata
 * e mostra messaggi appropriati in base al tipo di oggetto:
 * - Per MISSIONE_MAGIONE: mostra messaggio recupero Chiave del Castello
 * - Per MISSIONE_GROTTA: mostra messaggio recupero Spada dell'Eroe e bonus attacco
 * 
 * Gli oggetti speciali sono:
 * - Chiave del Castello (MISSIONE_MAGIONE): necessaria per accedere alla missione finale
 * - Spada dell'Eroe (MISSIONE_GROTTA): fornisce bonus di +2 all'attacco
 * 
 * @param[in,out] missione Puntatore alla Missione in cui recuperare l'oggetto
 * 
 * @pre missione deve essere un puntatore valido
 * @post missione->oggettoRecuperato è impostato a true
 * @post Messaggi informativi vengono stampati su stdout
 * 
 * @note Se missione è NULL, la funzione ritorna senza operazioni
 * @note L'incremento effettivo dell'attacco per la Spada deve essere gestito altrove
 * 
 * @see TipoMissione
 * @see Missione
 */
void segnaOggettoRecuperato(Missione* missione) {
    if (missione == NULL) return;
    
    missione->oggettoRecuperato = true;
    
    printf(COLORE_VERDE "✨ Hai recuperato un oggetto speciale!\n" COLORE_RESET);
    
    if (missione->tipo == MISSIONE_MAGIONE) {
        printf(COLORE_GIALLO "Hai ottenuto la Chiave del Castello del Signore Oscuro!\n" COLORE_RESET);
    } else if (missione->tipo == MISSIONE_GROTTA) {
        printf(COLORE_GIALLO "Hai ottenuto la leggendaria Spada dell'Eroe!\n" COLORE_RESET);
        printf(COLORE_CIANO "   La tua potenza di attacco aumenta di +2!\n" COLORE_RESET);
    }
}

// --- FUNZIONI DI CONTROLLO STATO ---

bool missioneCompletata(const Missione* missione) {
    return (missione != NULL && missione->completata);
}

bool missioneSbloccata(const Missione* missione) {
    return (missione != NULL && missione->sbloccata);
}

bool tutteLePreliminariCompletate(const GestoreMissioni* gestore) {
    if (gestore == NULL) return false;
    
    return (gestore->missioni[MISSIONE_PALUDE].completata &&
            gestore->missioni[MISSIONE_MAGIONE].completata &&
            gestore->missioni[MISSIONE_GROTTA].completata);
}

bool obiettiviRaggiunti(const Missione* missione) {
    if (missione == NULL) return false;
    
    // Per missioni senza obiettivi numerici (come Grotta), considera solo l'oggetto
    if (missione->obiettiviTotali == 0) {
        return missione->oggettoRecuperato;
    }
    
    return (missione->obiettiviCompletati >= missione->obiettiviTotali);
}

void sbloccaMissioneFinale(GestoreMissioni* gestore) {
    if (gestore == NULL) return;
    
    gestore->missioni[MISSIONE_CASTELLO].sbloccata = true;
    
    printf("\n");
    printf(COLORE_ROSSO "-------------------------------------------------\n" COLORE_RESET);
    printf(COLORE_ROSSO "          MISSIONE FINALE SBLOCCATA!            \n" COLORE_RESET);
    printf(COLORE_ROSSO "-------------------------------------------------\n" COLORE_RESET);
    printf(COLORE_MAGENTA "Il Castello del Signore Oscuro ti attende...\n" COLORE_RESET);
    printf(COLORE_GIALLO "Preparati per lo scontro finale!\n" COLORE_RESET);
}

// --- FUNZIONI DI UTILITÀ ---

const char* getNomeMissione(TipoMissione tipo) {
    switch (tipo) {
        case MISSIONE_PALUDE:   return "Palude Putrescente";
        case MISSIONE_MAGIONE:  return "Magione Infestata";
        case MISSIONE_GROTTA:   return "Grotta di Cristallo";
        case MISSIONE_CASTELLO: return "Castello del Signore Oscuro";
        default:                return "Sconosciuta";
    }
}

const char* getDescrizioneMissione(TipoMissione tipo) {
    switch (tipo) {
        case MISSIONE_PALUDE:
            return "Sconfiggi 3 Generale Orco del Signore Oscuro";
        case MISSIONE_MAGIONE:
            return "Recupera la chiave del Castello e sconfiggi un Vampiro Superiore";
        case MISSIONE_GROTTA:
            return "Recupera la Spada dell'Eroe";
        case MISSIONE_CASTELLO:
            return "Sconfiggi il Signore Oscuro";
        default:
            return "Nessuna descrizione disponibile";
    }
}

Missione* getMissione(GestoreMissioni* gestore, TipoMissione tipo) {
    if (gestore == NULL || tipo < 0 || tipo > 3) {
        return NULL;
    }
    return &gestore->missioni[tipo];
}