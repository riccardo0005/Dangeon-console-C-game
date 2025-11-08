#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "menu.h"
#include "salvataggi.h"
#include "eroe.h"
#include "trucchi.h"

#define MAX_TRUCCHI 32

static bool trucchiAttivi = false;

static void stampaMenuConRiquadroADueOpzioni(void);
static void stampaMenuConRiquadroAtreOpzioni(void);

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

void menuPrincipale(void) {
    char opzione;
    char contTrucchi[MAX_TRUCCHI] = {0};
    int i = 0;

    while (1) {
        if (trucchiAttivi) {
            stampaMenuConRiquadroAtreOpzioni();
        } else {
            stampaMenuConRiquadroADueOpzioni();
        }
        printf("Seleziona una delle opzioni del menu [%s] : ", trucchiAttivi ? "1 - 3" : "1 - 2 - 0");

        opzione = leggiCaratterePulito();

        if (!carattereValido(opzione, trucchiAttivi)) {
            printf("Carattere non valido, riprova.\n");
            continue;
        }

        switch (opzione) {
            case '0':
                printf("Uscita dal gioco. Arrivederci!\n");
                return; // Termina il programma
            case '1':
                gestisciNuovaPartita();
                return; // Esce dopo nuova partita
            case '2':
                gestisciCaricaSalvataggio();
                break;
            case '3':
                if (trucchiAttivi) {
                    gestisciMenuTrucchi();
                } else {
                    printf("Opzione non valida.\n");
                }
                break;
            case ' ':
                if (i == 0) {
                    printf("Errore: terminatore spazio inserito senza sequenza.\n");
                } else {
                    if (confrontoString(contTrucchi)) {
                        printf("\n🔥 TRUCCHI ATTIVATI! 🔥\n");
                        trucchiAttivi = true;
                    } else {
                        printf("\n❌ Codice errato. Riprova.\n");
                    }
                    i = 0;
                    contTrucchi[0] = '\0';
                }
                break;
            default:
                if (!trucchiAttivi) {
                    if (i < MAX_TRUCCHI - 1) {
                        contTrucchi[i++] = opzione;
                        contTrucchi[i] = '\0';
                    } else {
                        printf("Sequenza troppo lunga, ripartiamo.\n");
                        i = 0;
                        contTrucchi[0] = '\0';
                    }
                } else {
                    printf("Opzione non valida.\n");
                }
                break;
        }
    }
}

void gestisciNuovaPartita(void) {
    printf("Hai scelto NUOVA PARTITA!\n");
    Eroe eroe;
    printf("Inserisci il nome del tuo eroe: ");
    dichiaraNomeEroe(eroe.nome);

    inizializzaEroe(&eroe, eroe.nome);
    Salvataggio s = creaSalvataggioDaEroe(&eroe);

    if (salvaGioco(&s)) {
        printf("Salvataggio iniziale creato con successo!\n");
    } else {
        printf("Errore nel salvataggio iniziale.\n");
    }

    mostraEroe(&eroe);
}

void gestisciCaricaSalvataggio(void) {
    printf("Hai scelto CARICA SALVATAGGIO!\n");
    mostraMenuSalvataggi();

    if (contaSalvataggi() == 0) {
        printf("Nessun salvataggio disponibile.\n");
        return;
    }

    int sceltaSalvataggio = chiediSalvataggioDaCaricare();
    if (sceltaSalvataggio == -1) {
        printf("Tornando al menu principale...\n");
        return;
    }

    int res = gestioneSalvataggioScelto(sceltaSalvataggio);
    if (res == 0) {
        printf("Tornando al menu principale...\n");
    }
}

void gestisciMenuTrucchi(void) {
    printf("Sei entrato nel menu TRUCCHI!\n");
    mostraMenuSalvataggi();

    if (contaSalvataggi() == 0) {
        printf("Nessun salvataggio disponibile per modifiche.\n");
        return;
    }

    int sceltaSalvataggio = chiediSalvataggioDaCaricare();
    if (sceltaSalvataggio == -1) {
        printf("Tornando al menu principale...\n");
        return;
    }

    modificaCampoSalvataggio(sceltaSalvataggio);
}

void modificaCampoSalvataggio(int sceltaSalvataggio) {
    printf("Hai scelto il salvataggio %d\n", sceltaSalvataggio);
    printf("Cosa vuoi modificare nel salvataggio %d?\n", sceltaSalvataggio);
    printf("1. Vita\n2. Monete\n3. Missione Finale\n");

    char scelta;
    int c;
    while (1) {
        printf("Scegli un'opzione [1-3]: ");
        scelta = leggiCaratterePulito();

        if (scelta < '1' || scelta > '3') {
            printf("Opzione non valida. Riprova.\n");
            continue;
        }
        break;
    }

    Salvataggio s;
    if (!leggiSalvataggioIndice(sceltaSalvataggio, &s)) {
        printf("Errore nel caricamento del salvataggio.\n");
        return;
    }

    Eroe eroeModificato = creaEroeDaSalvataggio(&s);

    if (scelta == '1') {
        printf("Modifica della VITA selezionata.\n");
        printf("Di quanto vuoi modificare la vita? (usa numero negativo per diminuire): ");
        int delta;
        scanf("%d", &delta);
        while ((c = getchar()) != '\n' && c != EOF);
        modificaVita(&eroeModificato, delta);
    } else if (scelta == '2') {
        printf("Modifica delle MONETE selezionata.\n");
        printf("Di quanto vuoi modificare le monete? (usa numero negativo per diminuire): ");
        int delta;
        scanf("%d", &delta);
        while ((c = getchar()) != '\n' && c != EOF);
        modificaMonete(&eroeModificato, delta);
    } else if (scelta == '3') {
        printf("Modifica della MISSIONE FINALE selezionata.\n");
        // TODO: implementare modifiche missione finale
        printf("Funzionalità non ancora implementata.\n");
        return;
    }

    Salvataggio sModificato = creaSalvataggioDaEroe(&eroeModificato);
    if (salvaGioco(&sModificato)) {
        printf("Modifica salvata con successo!\n");
    } else {
        printf("Errore nel salvataggio delle modifiche.\n");
    }
}

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
    printf("*  0. Esci                         *\n");
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
    printf("*  3. Trucchi                      *\n");
    printf("*  0. Esci                         *\n");
    printf("*************************************\n");
    printf("%s", reset);
}
