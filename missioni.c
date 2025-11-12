#include "missioni.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Colori ANSI per output colorato
#define COLORE_VERDE "\033[1;32m"
#define COLORE_ROSSO "\033[1;31m"
#define COLORE_GIALLO "\033[1;33m"
#define COLORE_BLU "\033[1;34m"
#define COLORE_CIANO "\033[1;36m"
#define COLORE_MAGENTA "\033[1;35m"
#define COLORE_RESET "\033[0m"

// --- FUNZIONI DI INIZIALIZZAZIONE ---

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

void inizializzaGestoreMissioni(GestoreMissioni* gestore) {
    if (gestore == NULL) return;
    
    gestore->missioniCompletate = 0;
    gestore->missioneCorrente = MISSIONE_NESSUNA;
    
    // Inizializza Palude Putrescente
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