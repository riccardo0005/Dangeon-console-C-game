// salvataggi.c  (versione corretta con aggiornamento salvataggi)
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

/*
 * IMPLEMENTAZIONE DEL CRC32
 *
 * 1. Cos'è il CRC32:
 *    Il CRC32 (Cyclic Redundancy Check a 32 bit) è un codice di controllo utilizzato per verificare
 *    l'integrità dei dati. Calcola un valore di 32 bit (4 byte) a partire da un blocco di dati,
 *    permettendo di rilevare errori o modifiche accidentali o intenzionali nel contenuto.
 *
 * 2. Tabella CRC32 (crc32_table):
 *    La tabella crc32_table contiene 256 valori precalcolati di 32 bit.
 *    Serve a velocizzare il calcolo del CRC.
 *    Senza questa tabella, bisognerebbe fare il calcolo bit per bit, molto più lento.
 *    La tabella viene generata usando un polinomio standard per il CRC32, e viene usata per ogni byte di dati
 *    per aggiornare rapidamente il valore di controllo.
 *
 * 3. Funzionamento del calcolo (funzione crc32_compute):
 *    - Si inizializza il valore CRC a 0xFFFFFFFF.
 *    - Per ogni byte del blocco dati:
 *      - Si esegue un XOR tra l'ultimo byte del CRC attuale e il byte di dati corrente.
 *      - Si usa questo valore per indicizzare la tabella crc32_table.
 *      - Si aggiorna il valore CRC con un'operazione di shift a destra di 8 bit combinata con il valore preso dalla tabella.
 *    - Alla fine, si fa un XOR finale con 0xFFFFFFFF per ottenere il valore CRC definitivo.
 *    Questo processo sintetizza il contenuto del blocco dati in un unico valore di 32 bit.
 *
 * 4. Come viene usato nel codice:
 *    Quando si salva la struttura Salvataggio, si calcola il CRC32 sul blocco dati corrispondente alla struttura.
 *    Il CRC calcolato viene scritto nel file subito dopo i dati.
 *    Quando si legge il file, si ricalcola il CRC32 sui dati letti e lo si confronta con quello salvato.
 *    Se i due valori coincidono, significa che i dati non sono stati modificati o corrotti.
 *    Altrimenti, il file è considerato corrotto o non valido.
 *
 * 5. Problematiche di questo metodo:
 *    - Il CRC32 è efficace nel rilevare errori casuali, ma non è a prova di manomissione intenzionale:
 *      qualcuno che modifica il file può rigenerare il CRC corretto e quindi bypassare il controllo.
 *    - Non è un algoritmo di crittografia né di autenticazione, solo di integrità.
 *    - Può non rilevare alcuni tipi molto di errori.
 *
 * 6. Possibili soluzioni per migliorare la sicurezza:
 *    - Usare un algoritmi crittografici che pero' non sono stati implementati per mantenere il codice il piu semplice possibile.
 * 
 * Link utilizzati per capire CRC32, la tabella e i consigli d'uso:
 *
 * 1. Spiegazione teorica e calcolo CRC32:
 *    - Wikipedia: https://en.wikipedia.org/wiki/Cyclic_redundancy_check
 *    - Tutorial con esempio: https://www.sunshine2k.de/coding/javascript/crc/crc_js.html
 *
 * 2. Tabella CRC32 e generazione:
 *    - Come generare la tabella CRC32: https://create.stephan-brumme.com/crc32/
 *    - Esempio in C per la tabella: https://stackoverflow.com/questions/21001659/crc32-implementation-in-c-with-a-look-up-table
 *
 * 3. Implementazioni comuni e librerie:
 *    - Libreria (open source): https://github.com/madler/zlib/blob/master/crc32.c
 *    - Spiegazione + codice CRC32 in C: https://www.programmersought.com/article/42936123907/
 *
 * 4. Consigli d’uso e limiti:
 *    - Quando e perché usare CRC32: https://embeddedgurus.com/stack-overflow/2013/07/crc-calculation-in-embedded-systems/
 *    - Limiti e alternative (MD5, SHA): https://stackoverflow.com/questions/22912118/when-to-use-crc32-vs-md5
 *
 * 5. Strumenti online per test:
 *    - Calcolatore CRC32 online: https://www.lammertbies.nl/comm/info/crc-calculation.html
 */

 

/* ------------------ CRC32 Tabella ------------------ */
static const uint32_t crc32_table[256] = {
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

//Funzione che calcola il crc di un buffer di dati
//data punta ai dati e len e' la lunghezza in byte dei dati
static uint32_t crc32_compute(const void* data, size_t len) {
    //Converto il puntatore generico 'data' in un puntatore a byte (uint8_t)
    //Perché il CRC si calcola byte per byte.
    //QUESTA CONVERSIONE E' FONDAMENTALE PER POTER TRARRARE I DATI COME BYTE
    const uint8_t* p = (const uint8_t*)data;

    //Inizializzo il valore CRC a 0xFFFFFFFF (tutti i bit a 1).
    //Questo è uno standard per il calcolo CRC32.
    uint32_t crc = 0xFFFFFFFFU;

    //Ciclo finché la lunghezza 'len' non arriva a zero,
    // processando un byte per iterazione.
while (len--) {
    // Prendo il byte attuale dai dati (il primo byte non ancora letto)
    // e sposto il puntatore 'p' al byte successivo per la prossima lettura.
    uint8_t byte = *p++;

    // Aggiorno il valore CRC usando questo nuovo byte:
    // 1) Sposto a destra di 8 bit il valore CRC attuale, 
    //    come se "tagliassi" il byte già elaborato.
    //
    // 2) Faccio un'operazione XOR tra il byte meno significativo di 'crc' 
    //    e il nuovo byte letto, per "mescolare" il nuovo dato con il CRC corrente.
    //
    // 3) Uso questo risultato che sara un valore tra zero e 255 come indice per cercare nella tabella 'crc32_table' 
    //    un valore già calcolato che aiuta a velocizzare il calcolo. Quel valore sara una sequenza di byte
    //
    // 4) Infine, faccio l'XOR tra il valore del crc shiftato di 8 bit(perche gia li ho usati ne lpasso 1) e quello preso dalla tabella (passo 3)
    //    ottenendo il nuovo valore aggiornato di CRC.
    crc = (crc >> 8) ^ crc32_table[(crc ^ byte) & 0xFFU/* & 0xFFU operazione che prende il bite meno significativo, l'ultimo*/];
}
    // Alla fine inverto tutti i bit del CRC (XOR con 0xFFFFFFFF),
    // perché la definizione standard di CRC32 richiede questo passaggio per poter rilevare il minimo errore.
    return crc ^ 0xFFFFFFFFU;

    // Queste operazioni servono a "mescolare" bene i dati,
    // in modo che anche una piccola modifica in un singolo byte
    // cambi molto il valore finale del CRC.
    // Questo rende il CRC molto sensibile agli errori,
    // permettendo di rilevare se i dati sono stati modificati o corrotti.

}

//Funzione che costruisce il nome del file, puo servire ad esempio per trovare un file
static void costruisciNomeFile(int idx, char* buffer) {

          //“Prendi la cartella dei salvataggi, aggiungi /save, poi il numero, poi .dat e metti tutto nel buffer.
          //(destinazione,dimensione massima, nome salvataggio,dove salvarli,indice salvataggo
    snprintf(buffer, MAX_NOME_FILE, "%s/save%d.dat", CARTELLA_SALVATAGGI, idx);
}


//Funzione che controlla l'esistenza di una cartella, se non esiste la crea
static void controllaCreaCartella() {
    //Questa struttura contiene vari campi tra cui permessi , dimensioni, data di modifica e tanto altro, e; una struttura standard gia presente nel linguaggio c
    struct stat st = {0};//creo una struttura stat e inizializzo i campi a 0, la struttura va creata altrimenti non posso usare la  funzione stat

    if (stat(CARTELLA_SALVATAGGI, &st) == -1) {//se la cartella non esiste allora
        MKDIR(CARTELLA_SALVATAGGI);//creo la cartella di nome CARTELLA_SALVATAGGI(vedi riga 19 per il nome)
    }
}

//Funzione che salva su un file un campo di tipo Salvataggio aggiungendo alla fine un crc a 32 bit per poi verificare lintegrita del dato alla lettura
static bool scriviFileConCRC(const char* path, const Salvataggio* s) {
    FILE* f = fopen(path, "wb");//Apro il file in modalita di sacrittura binaria, ovviamente gli passo il percorso per poterci accedere
    if (!f) return false;//Se non riesce ad aprirlo torna false

    //Scrivo sul file aperto un elemento di dimensione  salvataggio, della struttura stessa 
    //( puntatore ai dati da scrivere, dimensione in byte di ogni elemento, numero di elementi da scrivere, puntatore al file aperto)
    //Se avvengono errori nella scrittura torna un numero diverso da uno.
    //In breve sto copiando tutta la struttura Salvataggio dentro al file
    if (fwrite(s, sizeof(Salvataggio), 1, f) != 1) {
        fclose(f);//chiudo il file per evitare perdite di risorse
        perror("Errore nella scrittura del file");
        return false;
    }

    /**
     * Chiama la funzione crc32_compute che calcola il CRC32 su un blocco di dati.
     * Passa a questa funzione un puntatore generico (const void*)s che punta alla struttura Salvataggio.
     * Passa anche la dimensione in byte della struttura Salvataggio con sizeof(Salvataggio).
     * Il risultato è un valore uint32_t (32 bit) che rappresenta il checksum CRC, ovvero un codice che sintetizza i dati e permette di verificarne l'integrità.
     * uint32_t è un intero senza segno a 32 bit definito nello standard C (in <stdint.h>).
     *Serve a contenere esattamente 32 bit di dati, cioè il valore di CRC calcolato.
     *
     * const void* data: void* è un puntatore generico: può puntare a qualsiasi tipo di dato (intero, struct, array, ecc).
     * Questo rende la funzione generica, cioè può calcolare il CRC di qualunque blocco di memoria, indipendentemente dal tipo di dati che contiene.
     * const significa che la funzione non modificherà i dati a cui punta data. È una promessa che la funzione legge solo i dati senza cambiarli.
     **/
    uint32_t crc = crc32_compute((const void*)s, sizeof(Salvataggio));
    uint8_t crc_bytes[4];//dichiaro un array di 4 byte. uint8_t sono 8bit
    
    //Poiche il crc e' un valore di 32 bit e quindi 4 byte divido larray in 4
    
    //Invece di scrivere direttamente il valore uint32_t (che potrebbe essere interpretato diversamente su piattaforme diverse),
    //lo "spezzettiamo" in byte per avere un formato file  ovunque
    crc_bytes[0] = (uint8_t)(crc & 0xFFU);//prendo l'ultimo byte meno significativo del codice CRC  (gli 8 bit piu a destra) tramite crc & 0xFFU
    crc_bytes[1] = (uint8_t)((crc >> 8) & 0xFFU);//mi sposto a destra ancora di otto bit per prendere un altro byte
    crc_bytes[2] = (uint8_t)((crc >> 16) & 0xFFU);//mi sposto a destra ancora di otto bit per prendere un altro byte
    crc_bytes[3] = (uint8_t)((crc >> 24) & 0xFFU);//mi sposto a destra ancora di otto bit per prendere un altro byte
    
    //In pratica ho scomposto il valore di 32 bitin 4 blocchi di 8 bit(4byte)
    //scrivo i 4 elementi da un byte allinterno del file aperto f
    //se non riesce a scriverli tutti torna false
    if (fwrite(crc_bytes, 1, 4, f) != 4) {
        fclose(f);                           //chiudo il file per evitare perdita di dati
        return false;                       //torno false
    }
    fclose(f);              //chiudo il file terminate tutte le varie operazioni
    return true;            //Se la scrittura e' avvenuta con successo ritorno true
}

//Funzione che legge il salvataggio dal suo indice
bool leggiSalvataggioIndice(int idx, Salvataggio* s) {//parametri: indice del salvataggio e puntatore al saalvataggio stesso
    
    if (idx <= 0) return false; //Se l'indice e' zero o negativo ce un errore e quindi usciamo

    char nomeFile[MAX_NOME_FILE];//Variabile contenitore del nome del file
    costruisciNomeFile(idx, nomeFile);//Costruisce il nome del file avendo il suo indice affijnche sia possibile identificarlo

    FILE* f = fopen(nomeFile, "rb");//Provo a leggere il file in lettura binaria
    if (!f) return false;//Se f e null allora il file non e stato letto e ce un errore, quindi esco

    //fseek sposta il puntatore alla fine del file(SEEK END) permettendo di misurare la dimensione del file tramite ftell
    //Se fseek ritorna un valore diverso da zero significa che lo spostamento è fallito (ad esempio file non leggibile o errore).
    //In tal caso chiude il file e ritorna false  perche non possiamo continuare
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return false; }
    
    //In caso contrario si prosegue
    //ftell restituisce la posizione corrente del puntatore nel file, che dopo il fseek su SEEK_END equivale alla dimensione in byte del file.
    long sz = ftell(f);//long e il tipo di dato tipico per la posizione dei file 
    rewind(f);//Riportiamo il puntatore alla posizione di partenza (fondamentale per poter usare fread)

/**
 * IMPORTANTE CONSIDERAZIONE
 * Problematica riscontrata: A questo punto del programma mi sono chiesto cosa potesse succedere qualora avessi modificato
 *                           i file di testo manualmente e successivamente provato a ricaricarli.
 * 
 * Risposta: I file vengono caricati comunque e nel peggiore dei casi rischio dati invalidi.
 * 
 * Domanda iniziale: E' possibile riuscire a leggere i file vecchi e capire se sono stati modificati, ovviamente lo stesso deve essere fatto anche per i file nuovi
 * 
 * Soluzione: Dopo essermi documentato in merito ho deciso di provare ad implementare all'interno del programma un controllo chiamato CRC (CYCLIC REDUNDANCY CHECK)
 * Spiegazione: -Il controllo CRC permette quando salvi i dati, di andare a calcolare un valore chiamato CRC
 *              -Questo valore viene inserito insieme ai dati
 *              -Quando carico i dati, ricalcolo questo valore e lo confronto con il CRC salvato precedentemente
 *              -Se i due valori non corrispondono, significa che i dati sono stati modificati o corrotti
 * 
 **/            

 
    //se la dimensione e' esattamente la dimensione della struttura salvataggio
    //Allora il file corrisponde al formato vecchio senza il CRC
    if (sz == (long)sizeof(Salvataggio)) {

        //Tenta di leggere dal file la struttura Salvataggio in un blocco singolo
        //fread ritorna il numero di elementi letti
        //Se la lettura fallisce(numero di elementi diversi da 1)
        //Legge dal file aperto (f) un blocco di dati in binario di dimensione sizeof(Salvataggio(cioe lo spazio di memoria che occupa)) e lo salva nella memoria puntata da s
        //Il parametro uno indica che si vuole leggere un solo elemento di quella dimensione (quindi l'intera struttura)
        //Se il valore di ritorno e' diverso da uno significa che non e' riuscito a leggere in modo corretto la struttura
        if (fread(s, sizeof(Salvataggio), 1, f) != 1) { fclose(f); return false; }
        fclose(f);///se la lettura e andata bene chiude il file per liberare le risorse
        controllaCreaCartella();//controlla se la cartellaesiste altrimenti la crea
        
        if (!scriviFileConCRC(nomeFile, s)) {
            return true;
        }
        return true;
    } else if (sz == (long)(sizeof(Salvataggio) + 4)) {
        if (fread(s, sizeof(Salvataggio), 1, f) != 1) { fclose(f); return false; }
        uint8_t crc_bytes[4];
        if (fread(crc_bytes, 1, 4, f) != 4) { fclose(f); return false; }
        fclose(f);
        uint32_t file_crc = (uint32_t)crc_bytes[0] | ((uint32_t)crc_bytes[1] << 8) | ((uint32_t)crc_bytes[2] << 16) | ((uint32_t)crc_bytes[3] << 24);
        uint32_t calc = crc32_compute((const void*)s, sizeof(Salvataggio));
        if (file_crc != calc) {
            fprintf(stderr, "Attenzione: salvataggio %d corrotto (CRC mismatch).\n", idx);
            return false;
        }
        return true;
    } else {
        fclose(f);
        fprintf(stderr, "Attenzione: salvataggio %d con formato sconosciuto (size=%ld).\n", idx, sz);
        return false;
    }
}

//Funzione che salva o aggiorna il salvataggio
bool salvaGioco(const Salvataggio* s) {
    if (s == NULL) return false;    //se il puntatore e nullo torna false
    
    controllaCreaCartella();        //controlla se la cartella esiste o meno. La crea se non esiste

    char nomeFile[MAX_NOME_FILE];   //nome del file di gioco di dimensione massima pari a MAX NOME FILE
    Salvataggio temp;               //Creo un istanza salvataggio

    int count = contaSalvataggi();  //contatore del numero di salvataggi presenti nella cartella
    bool trovato = false;           //Variabile che permette di capire se un file ce o no
    int indiceTrovato = -1;

    // Cerca se esiste già un salvataggio con lo stesso nome
    for (int i = 1; i <= count; i++) {
        if (!leggiSalvataggioIndice(i, &temp)) {
            continue; // Salta file corrotti
        }
        
        // Confronta i nomi (case-sensitive)
        if (strcmp(temp.nome, s->nome) == 0) {
            indiceTrovato = i;
            trovato = true;
            break;
        }
    }

    if (trovato) {
        // AGGIORNA il salvataggio esistente
        costruisciNomeFile(indiceTrovato, nomeFile);
        
        // Crea una copia del nuovo salvataggio ma mantiene il timestamp originale
        Salvataggio salvataggioAggiornato = *s;
        salvataggioAggiornato.dataSalvataggio = time(NULL); // Aggiorna timestamp
        
        if (!scriviFileConCRC(nomeFile, &salvataggioAggiornato)) {
            printf("Errore nell'aggiornamento del salvataggio!\n");
            return false;
        }
        
        printf("Salvataggio aggiornato per '%s' (slot %d)\n", s->nome, indiceTrovato);
        return true;
    } else {
        // CREA un nuovo salvataggio
    
        costruisciNomeFile(count + 1, nomeFile);
        
        Salvataggio nuovoSalvataggio = *s;
        nuovoSalvataggio.dataSalvataggio = time(NULL);
        
        if (!scriviFileConCRC(nomeFile, &nuovoSalvataggio)) {
            printf("Errore nella creazione del salvataggio!\n");
            return false;
        }
        
        printf("Nuovo salvataggio creato per '%s' (slot %d)\n", s->nome, count + 1);
        return true;
    }
}

//Funzione che conta il numero di salvataggi e torna il numero scorrendoli e si ferma quando non ne trova piu
int contaSalvataggi(void) {

    controllaCreaCartella();         //Controlla se la cartella esiste o meno. La crea se non esiste

    int count = 0;                   //imposta un contatore di salvataggi a 0
    char nomeFile[MAX_NOME_FILE];    //Variabile contenente il nome del salvataggio
    FILE* f;                         //Creo un file f di tipo FILE

    while (1) {                                 //Ciclo infinito
        costruisciNomeFile(count + 1, nomeFile);//costruisce il nome del file numero count+1, per  identificare i salvataggio e lo inserisce nel buffer nomeFile
        f = fopen(nomeFile, "rb");              //Prova ad aprire il file in lettura binarioa (rb)
        
        if (f) {           //Se fopen non e' null allora il file esiste
            fclose(f);     //Chiudi il file
            count++;       //Passa al salvataggio successiov
        } else {//Altrimenti
            break;         //Il file non esiste quindi esci dal ciclo
        }
    }   
    return count;          //Torna il numero di file
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


//Funzione che ritorna un salvataggio una volta creato un eroe
Salvataggio creaSalvataggioDaEroe(const Eroe* eroe) {//passo un puntatore all'eroe
    
    //best practice
    Salvataggio s = {0};//creo un salvataggio inizializzandolo tutti i campi a 0 per evitare valori casuali indesiderati che potrebbero causare problematiche
    
    strncpy(s.nome, eroe->nome, MAX_NOME_EROE - 1);//Copio il nome dell'eroe nel campo del salvataggio indicato con un numero massimo di caratteri da copiare pari a MAX NOME EROE -1
    
    s.nome[MAX_NOME_EROE - 1] = '\0';//Imposto il terminatore
    
    //Imposto i campi del salvataggio s di tipo Salvataggio con i campi attuali del mio eroe
    s.vita = eroe->vita;
    s.monete = eroe->monete;
    s.missioniCompletate = eroe->missioniCompletate;
    s.oggettiPosseduti = eroe->oggettiPosseduti;

    //NON impostiamo il timestamp qui, lo fa salvaGioco() questo perche la nostra funzioene crea un dato, non lo salva
    s.dataSalvataggio = 0; //Imposto un valore iniziale per evitare dati indesiderati
    return s;//Ritorno il salvataggio
}

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
            // Rimuovi il newline da ctime
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
            printf("[%d] File corrotto o non leggibile.\n\n", i + 1);
        }
    }
}

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
        } else if (scelta == '3') {
            printf("Operazione annullata.\n");
            return 0;
        } else {
            printf("Opzione non valida. Riprova.\n");
        }
    }
}