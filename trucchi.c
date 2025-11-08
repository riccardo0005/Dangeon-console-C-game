#include <stdbool.h>
#include <string.h>
#include "trucchi.h"

static const char TRUCCHI[] = "wwssadadba";

static bool carattereTruccoValido(char c) {
    size_t len = strlen(TRUCCHI);
    for (size_t i = 0; i < len; i++) {
        if (TRUCCHI[i] == c)
            return true;
    }
    return false;
}

bool confrontoString(const char *input) {
    return strcmp(input, TRUCCHI) == 0;
}

bool carattereValido(char c, bool trucchiAttivi) {
    if (trucchiAttivi) {
        if (c == '1' || c == '2' || c == '3')
            return true;
        return false;
    } else {
        if (c == '1' || c == '2' || c == ' ' || c == '0')
            return true;

        if (carattereTruccoValido(c))
            return true;

        return false;
    }
}
