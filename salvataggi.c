#include "salvataggi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>   // Aggiungi questo include per _mkdir su Windows
#define MKDIR(path) _mkdir(path)
#else
#include <unistd.h>
#define MKDIR(path) mkdir(path, 0700)
#endif

#define CARTELLA_SALVATAGGI "salvataggi"

static void costruisciNomeFile(int idx, char* buffer) {
    snprintf(buffer, MAX_NOME_FILE, "%s/save%d.dat", CARTELLA_SALVATAGGI, idx);
}

static void controllaCreaCartella() {
    struct stat st = {0};
    if (stat(CARTELLA_SALVATAGGI, &st) == -1) {
        MKDIR(CARTELLA_SALVATAGGI);
    }
}

bool salvaGioco(const Salvataggio* s) {
    controllaCreaCartella();

    char nomeFile[MAX_NOME_FILE];
    Salvataggio temp;
    FILE* f = NULL;

    int count = contaSalvataggi();
    bool trovato = false;

    for (int i = 1; i <= count; i++) {
        costruisciNomeFile(i, nomeFile);
        f = fopen(nomeFile, "rb");
        if (f) {
            fread(&temp, sizeof(Salvataggio), 1, f);
            fclose(f);
            if (strcmp(temp.nome, s->nome) == 0) {
                f = fopen(nomeFile, "wb");
                if (!f) return false;
                fwrite(s, sizeof(Salvataggio), 1, f);
                fclose(f);
                trovato = true;
                printf("Salvataggio aggiornato per il profilo '%s'!\n", s->nome);
                break;
            }
        }
    }

    if (!trovato) {
        costruisciNomeFile(count + 1, nomeFile);
        f = fopen(nomeFile, "wb");
        if (!f) return false;
        fwrite(s, sizeof(Salvataggio), 1, f);
        fclose(f);
        printf("Nuovo salvataggio creato per '%s'!\n", s->nome);
    }

    return true;
}

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

bool leggiSalvataggioIndice(int idx, Salvataggio* s) {
    if (idx <= 0) return false;
    char nomeFile[MAX_NOME_FILE];
    costruisciNomeFile(idx, nomeFile);

    FILE* f = fopen(nomeFile, "rb");
    if (!f) return false;

    fread(s, sizeof(Salvataggio), 1, f);
    fclose(f);
    return true;
}

bool eliminaSalvataggio(int idx) {
    if (idx <= 0) return false;

    char nomeFile[MAX_NOME_FILE];
    costruisciNomeFile(idx, nomeFile);

    int totalePrima = contaSalvataggi();

    if (remove(nomeFile) != 0) return false;

    for (int i = idx + 1; i <= totalePrima; i++) {
        char nomeVecchio[MAX_NOME_FILE];
        char nomeNuovo[MAX_NOME_FILE];
        costruisciNomeFile(i, nomeVecchio);
        costruisciNomeFile(i - 1, nomeNuovo);
        rename(nomeVecchio, nomeNuovo);
    }

    return true;
}

Salvataggio creaSalvataggioDaEroe(const Eroe* eroe) {
    Salvataggio s = {0};
    strcpy(s.nome, eroe->nome);
    s.vita = eroe->vita;
    s.monete = eroe->monete;
    s.missioniCompletate = eroe->missioniCompletate;
    s.oggettiPosseduti = eroe->oggettiPosseduti;
    s.dataSalvataggio = time(NULL);
    return s;
}

Eroe creaEroeDaSalvataggio(const Salvataggio* salvataggio) {
    Eroe e = {0};
    strcpy(e.nome, salvataggio->nome);
    e.vita = salvataggio->vita;
    e.monete = salvataggio->monete;
    e.missioniCompletate = salvataggio->missioniCompletate;
    e.oggettiPosseduti = salvataggio->oggettiPosseduti;
    return e;
}

void mostraMenuSalvataggi() {
    int totaleSalvataggi = contaSalvataggi();
    printf("Ci sono %d salvataggi disponibili:\n", totaleSalvataggi);
    if (totaleSalvataggi == 0) {
        printf("Nessun salvataggio trovato.\n");
        return;
    }
    for (int i = 0; i < totaleSalvataggi; i++) {
        Salvataggio s;
        if (leggiSalvataggioIndice(i + 1, &s)) {
            printf("\033[94mSalvataggio\033[0m %d: Nome: %s | %s | %d P. Vita| %d Monete| %d Oggetti| %d Missioni Completate\n",
                i + 1,
                s.nome,
                strtok(ctime(&s.dataSalvataggio), "\n"),
                s.vita,
                s.monete,
                s.oggettiPosseduti,
                s.missioniCompletate);
        } else {
            printf("Errore nel leggere il salvataggio %d\n", i + 1);
        }
    }
}

int chiediSalvataggioDaCaricare() {
    char input[16];
    int scelta;

    while (1) {
        printf("\nSeleziona il salvataggio da gestire [1 - %d] (o '%c' per tornare indietro): ", contaSalvataggi(), INPUT_BACK);

        if (!fgets(input, sizeof(input), stdin)) {
            printf("Errore di input.\n");
            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        bool valido = true;
        for (int i = 0; input[i] != '\0'; i++) {
            if (input[i] == INPUT_BACK) {
                return -1;
            } else if (input[i] < '0' || input[i] > '9') {
                valido = false;
                break;
            }
        }

        if (!valido || strlen(input) == 0) {
            printf("❌ Inserisci solo numeri tra 1 e %d.\n", contaSalvataggi());
            continue;
        }

        scelta = atoi(input);
        if (scelta < 1 || scelta > contaSalvataggi()) {
            printf("⚠️  Numero non valido! Scegli tra 1 e %d.\n", contaSalvataggi());
            continue;
        }

        return scelta;
    }
}

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
                // Mostra i dati dell'eroe caricato
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
        } else if (scelta == '2') {
            if (eliminaSalvataggio(sceltaSalvataggio)) {
                printf("Salvataggio eliminato con successo.\n");
            } else {
                printf("Errore nell'eliminazione.\n");
            }
            return 1;
        } else if (scelta == '3') {
            printf("Operazione annullata.\n");
            return 0;
        } else {
            printf("Opzione non valida. Riprova.\n");
        }
    }
}
