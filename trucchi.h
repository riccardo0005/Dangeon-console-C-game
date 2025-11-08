#ifndef TRUCCHI_H
#define TRUCCHI_H

#include <stdbool.h>

// Controlla se la stringa input corrisponde al codice trucchi
bool confrontoString(const char *input);

// Controlla se il carattere c è valido come input nel menu o nella sequenza trucchi
// trucchiAttivi = true significa che il menu ha anche opzione 3 (trucchi)
bool carattereValido(char c, bool trucchiAttivi);

#endif // TRUCCHI_H
