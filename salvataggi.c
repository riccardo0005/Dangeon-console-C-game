// salvataggi.c  (versione con CRC32 di integrità)
#include "salvataggi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>   // per _mkdir su Windows
#define MKDIR(path) _mkdir(path)
#else
#include <unistd.h>
#define MKDIR(path) mkdir(path, 0700)
#endif

#define CARTELLA_SALVATAGGI "salvataggi"

/* ------------------ CRC32 (table-based) ------------------ */
/* polynomial 0xEDB88320 (standard CRC-32/ISO-HDLC) */
static const uint32_t crc32_table[256] = {
    /* precomputed 256-entry table */
    0x00000000U,0x77073096U,0xee0e612cU,0x990951baU,0x076dc419U,0x706af48fU,0xe963a535U,0x9e6495a3U,
    0x0edb8832U,0x79dcb8a4U,0xe0d5e91eU,0x97d2d988U,0x09b64c2bU,0x7eb17cbdU,0xe7b82d07U,0x90bf1d91U,
    0x1db71064U,0x6ab020f2U,0xf3b97148U,0x84be41deU,0x1adad47dU,0x6ddde4ebU,0xf4d4b551U,0x83d385c7U,
    0x136c9856U,0x646ba8c0U,0xfd62f97aU,0x8a65c9ecU,0x14015c4fU,0x63066cd9U,0xfa0f3d63U,0x8d080df5U,
    0x3b6e20c8U,0x4c69105eU,0xd56041e4U,0xa2677172U,0x3c03e4d1U,0x4b04d447U,0xd20d85fdU,0xa50ab56bU,
    0x35b5a8faU,0x42b2986cU,0xdbbbc9d6U,0xacbcf940U,0x32d86ce3U,0x45df5c75U,0xdcd60dcfU,0xabd13d59U,
    0x26d930acU,0x51de003aU,0xc8d75180U,0xbfd06116U,0x21b4f4b5U,0x56b3c423U,0xcfba9599U,0xb8bda50fU,
    0x2802b89eU,0x5f058808U,0xc60cd9b2U,0xb10be924U,0x2f6f7c87U,0x58684c11U,0xc1611dabU,0xb6662d3dU,
    0x76dc4190U,0x01db7106U,0x98d220bcU,0xefd5102aU,0x71b18589U,0x06b6b51fU,0x9fbfe4a5U,0xe8b8d433U,
    0x7807c9a2U,0x0f00f934U,0x9609a88eU,0xe10e9818U,0x7f6a0dbbU,0x086d3d2dU,0x91646c97U,0xe6635c01U,
    0x6b6b51f4U,0x1c6c6162U,0x856530d8U,0xf262004eU,0x6c0695edU,0x1b01a57bU,0x8208f4c1U,0xf50fc457U,
    0x65b0d9c6U,0x12b7e950U,0x8bbeb8eaU,0xfcb9887cU,0x62dd1ddfU,0x15da2d49U,0x8cd37cf3U,0xfbd44c65U,
    0x4db26158U,0x3ab551ceU,0xa3bc0074U,0xd4bb30e2U,0x4adfa541U,0x3dd895d7U,0xa4d1c46dU,0xd3d6f4fbU,
    0x4369e96aU,0x346ed9fcU,0xad678846U,0xda60b8d0U,0x44042d73U,0x33031de5U,0xaa0a4c5fU,0xdd0d7cc9U,
    0x5005713cU,0x270241aaU,0xbe0b1010U,0xc90c2086U,0x5768b525U,0x206f85b3U,0xb966d409U,0xce61e49fU,
    0x5edef90eU,0x29d9c998U,0xb0d09822U,0xc7d7a8b4U,0x59b33d17U,0x2eb40d81U,0xb7bd5c3bU,0xc0ba6cadU,
    0xedb88320U,0x9abfb3b6U,0x03b6e20cU,0x74b1d29aU,0xead54739U,0x9dd277afU,0x04db2615U,0x73dc1683U,
    0xe3630b12U,0x94643b84U,0x0d6d6a3eU,0x7a6a5aa8U,0xe40ecf0bU,0x9309ff9dU,0x0a00ae27U,0x7d079eb1U,
    0xf00f9344U,0x8708a3d2U,0x1e01f268U,0x6906c2feU,0xf762575dU,0x806567cbU,0x196c3671U,0x6e6b06e7U,
    0xfed41b76U,0x89d32be0U,0x10da7a5aU,0x67dd4accU,0xf9b9df6fU,0x8ebeeff9U,0x17b7be43U,0x60b08ed5U,
    0xd6d6a3e8U,0xa1d1937eU,0x38d8c2c4U,0x4fdff252U,0xd1bb67f1U,0xa6bc5767U,0x3fb506ddU,0x48b2364bU,
    0xd80d2bdaU,0xaf0a1b4cU,0x36034af6U,0x41047a60U,0xdf60efc3U,0xa867df55U,0x316e8eefU,0x4669be79U,
    0xcb61b38cU,0xbc66831aU,0x256fd2a0U,0x5268e236U,0xcc0c7795U,0xbb0b4703U,0x220216b9U,0x5505262fU,
    0xc5ba3bbeU,0xb2bd0b28U,0x2bb45a92U,0x5cb36a04U,0xc2d7ffa7U,0xb5d0cf31U,0x2cd99e8bU,0x5bdeae1dU,
    0x9b64c2b0U,0xec63f226U,0x756aa39cU,0x026d930aU,0x9c0906a9U,0xeb0e363fU,0x72076785U,0x05005713U,
    0x95bf4a82U,0xe2b87a14U,0x7bb12baeU,0x0cb61b38U,0x92d28e9bU,0xe5d5be0dU,0x7cdcefb7U,0x0bdbdf21U,
    0x86d3d2d4U,0xf1d4e242U,0x68ddb3f8U,0x1fda836eU,0x81be16cdU,0xf6b9265bU,0x6fb077e1U,0x18b74777U,
    0x88085ae6U,0xff0f6a70U,0x66063bcaU,0x11010b5cU,0x8f659effU,0xf862ae69U,0x616bffd3U,0x166ccf45U,
    0xa00ae278U,0xd70dd2eeU,0x4e048354U,0x3903b3c2U,0xa7672661U,0xd06016f7U,0x4969474dU,0x3e6e77dbU,
    0xaed16a4aU,0xd9d65adcU,0x40df0b66U,0x37d83bf0U,0xa9bcae53U,0xdebb9ec5U,0x47b2cf7fU,0x30b5ffe9U,
    0xbdbdf21cU,0xcabac28aU,0x53b39330U,0x24b4a3a6U,0xbad03605U,0xcdd70693U,0x54de5729U,0x23d967bfU,
    0xb3667a2eU,0xc4614ab8U,0x5d681b02U,0x2a6f2b94U,0xb40bbe37U,0xc30c8ea1U,0x5a05df1bU,0x2d02ef8dU
};

static uint32_t crc32_compute(const void* data, size_t len) {
    const uint8_t* p = (const uint8_t*)data;
    uint32_t crc = 0xFFFFFFFFU;
    while (len--) {
        uint8_t byte = *p++;
        crc = (crc >> 8) ^ crc32_table[(crc ^ byte) & 0xFFU];
    }
    return crc ^ 0xFFFFFFFFU;
}

/* ------------------ helper file utilities ------------------ */

static void costruisciNomeFile(int idx, char* buffer) {
    snprintf(buffer, MAX_NOME_FILE, "%s/save%d.dat", CARTELLA_SALVATAGGI, idx);
}

static void controllaCreaCartella() {
    struct stat st = {0};
    if (stat(CARTELLA_SALVATAGGI, &st) == -1) {
        MKDIR(CARTELLA_SALVATAGGI);
    }
}

/* ------------------ I/O con CRC ------------------ */

/*
  Formato on-disk:
    [Salvataggio] (binary sizeof(Salvataggio))
    [uint32_t CRC32 little-endian] (4 bytes)
*/

/* Scrive un Salvataggio e il CRC nel file specificato (overwrite).
   Ritorna true se OK. */
static bool scriviFileConCRC(const char* path, const Salvataggio* s) {
    FILE* f = fopen(path, "wb");
    if (!f) return false;

    if (fwrite(s, sizeof(Salvataggio), 1, f) != 1) {
        fclose(f);
        return false;
    }
    uint32_t crc = crc32_compute((const void*)s, sizeof(Salvataggio));
    /* scrivo crc in formato little-endian portabile */
    uint8_t crc_bytes[4];
    crc_bytes[0] = (uint8_t)(crc & 0xFFU);
    crc_bytes[1] = (uint8_t)((crc >> 8) & 0xFFU);
    crc_bytes[2] = (uint8_t)((crc >> 16) & 0xFFU);
    crc_bytes[3] = (uint8_t)((crc >> 24) & 0xFFU);
    if (fwrite(crc_bytes, 1, 4, f) != 4) {
        fclose(f);
        return false;
    }
    fclose(f);
    return true;
}

/* Legge un file che potrebbe essere nel vecchio formato (solo Salvataggio)
   o nel nuovo formato (Salvataggio + CRC). Se il CRC è presente lo verifica.
   Se file è nel vecchio formato, esegue l'upgrade riscrivendo con CRC.
   Ritorna:
     - true e popola *s se lettura OK e CRC valido (o upgrade OK)
     - false se file corrotto, CRC mismatch o altro errore */
bool leggiSalvataggioIndice(int idx, Salvataggio* s) {
    if (idx <= 0) return false;
    char nomeFile[MAX_NOME_FILE];
    costruisciNomeFile(idx, nomeFile);

    FILE* f = fopen(nomeFile, "rb");
    if (!f) return false;

    /* determina dimensione file */
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return false; }
    long sz = ftell(f);
    rewind(f);

    if (sz == (long)sizeof(Salvataggio)) {
        /* vecchio formato: leggilo e poi riscrivi con CRC (upgrade) */
        if (fread(s, sizeof(Salvataggio), 1, f) != 1) { fclose(f); return false; }
        fclose(f);
        /* write back with CRC (safe rewrite) */
        controllaCreaCartella();
        if (!scriviFileConCRC(nomeFile, s)) {
            /* se non riesce a riscrivere, comunque ritorna il salvataggio letto */
            return true;
        }
        return true;
    } else if (sz == (long)(sizeof(Salvataggio) + 4)) {
        /* nuovo formato: leggi salvataggio + crc e verifica */
        if (fread(s, sizeof(Salvataggio), 1, f) != 1) { fclose(f); return false; }
        uint8_t crc_bytes[4];
        if (fread(crc_bytes, 1, 4, f) != 4) { fclose(f); return false; }
        fclose(f);
        uint32_t file_crc = (uint32_t)crc_bytes[0] | ((uint32_t)crc_bytes[1] << 8) | ((uint32_t)crc_bytes[2] << 16) | ((uint32_t)crc_bytes[3] << 24);
        uint32_t calc = crc32_compute((const void*)s, sizeof(Salvataggio));
        if (file_crc != calc) {
            /* CRC mismatch -> file corrotto */
            fprintf(stderr, "Attenzione: salvataggio %d corrotto (CRC mismatch).\n", idx);
            return false;
        }
        return true;
    } else {
        /* formato non riconosciuto */
        fclose(f);
        fprintf(stderr, "Attenzione: salvataggio %d con formato sconosciuto (size=%ld).\n", idx, sz);
        return false;
    }
}

/* Scrive/aggiorna file con CRC. Ritorna true se OK. */
bool salvaGioco(const Salvataggio* s) {
    controllaCreaCartella();

    char nomeFile[MAX_NOME_FILE];
    Salvataggio temp;


    int count = contaSalvataggi();
    bool trovato = false;

    /* cerca se esiste profilo con lo stesso nome (usando leggiSalvataggioIndice che verifica CRC) */
    for (int i = 1; i <= count; i++) {
        if (!leggiSalvataggioIndice(i, &temp)) {
            /* salta file non leggibile (corrotto) */
            continue;
        }
        if (strcmp(temp.nome, s->nome) == 0) {
            /* aggiorna quel file */
            costruisciNomeFile(i, nomeFile);
            if (!scriviFileConCRC(nomeFile, s)) return false;
            trovato = true;
            printf("Salvataggio aggiornato per il profilo '%s'!\n", s->nome);
            break;
        }
    }

    if (!trovato) {
        costruisciNomeFile(count + 1, nomeFile);
        if (!scriviFileConCRC(nomeFile, s)) return false;
        printf("Nuovo salvataggio creato per '%s'!\n", s->nome);
    }

    return true;
}

/* Conta quanti file di salvataggio esistono (slot sequenziali) */
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

/* Rimuove slot e ricompatta gli altri */
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

/* Crea Salvataggio da Eroe (non tocca CRC / file I/O) */
Salvataggio creaSalvataggioDaEroe(const Eroe* eroe) {
    Salvataggio s = {0};
    strncpy(s.nome, eroe->nome, MAX_NOME_EROE - 1);
    s.nome[MAX_NOME_EROE - 1] = '\0';
    s.vita = eroe->vita;
    s.monete = eroe->monete;
    s.missioniCompletate = eroe->missioniCompletate;
    s.oggettiPosseduti = eroe->oggettiPosseduti;
    s.dataSalvataggio = time(NULL);
    return s;
}

/* Crea Eroe da Salvataggio (in-memory conversion) */
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

/* Stampa elenco salvataggi (usa leggiSalvataggioIndice per leggere e verificare) */
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
            printf("Salvataggio %d: file corrotto o non leggibile.\n", i + 1);
        }
    }
}

/* chiediSalvataggioDaCaricare() e gestioneSalvataggioScelto() restano identiche al tuo codice precedente,
   quindi te le lascio come erano (non modificate) — se vuoi, le reintegro qui identiche. */

int chiediSalvataggioDaCaricare(void) {
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
                printf("Nome: %s\nVita: %d\nMonete: %d\nMissioni completate: %d\nOggetti posseduti: %d\n",
                    eroeCaricato.nome,
                    eroeCaricato.vita,
                    eroeCaricato.monete,
                    eroeCaricato.missioniCompletate,
                    eroeCaricato.oggettiPosseduti);
            } else {
                printf("Errore nel caricamento (file corrotto o non leggibile).\n");
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
