#ifndef MENU_H                       // Se MENU_H non è definito allora entriamo: inizio guardia di inclusione
#define MENU_H                       // Definisce MENU_H per evitare doppie inclusioni dello stesso header


//void stampaMenuConRiquadroADueOpzioni(); // Prototipo della funzione che stampa il menu con 2 opzioni (grafica/riquadro)
//void stampaMenuConRiquadroAtreOpzioni(); // Prototipo della funzione che stampa il menu con 3 opzioni (quando i trucchi sono attivi)

// Dichiarazioni delle funzioni menu
void menuPrincipale(void);
void gestisciNuovaPartita(void);
void gestisciCaricaSalvataggio(void);
void gestisciMenuTrucchi(void);
void modificaCampoSalvataggio(int sceltaSalvataggio);

#endif // MENU_H                      // Fine della guardia di inclusione: chiude il blocco #ifndef/#define