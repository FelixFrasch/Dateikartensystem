#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "liste.h"

// Vergleicht zwei Strings ohne Berücksichtigung der Groß-/Kleinschreibung
static int strcmpIgnoreCase(const char *a, const char *b) {
    while (*a && *b) {
        char la = (char)tolower((unsigned char)*a);
        char lb = (char)tolower((unsigned char)*b);
        if (la != lb) return la - lb;
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

FEHLERCODE addElement(DATEIKARTE **anfang, const char frage[], const char antwort[]) {
    DATEIKARTE *neu = malloc(sizeof(DATEIKARTE)); // reserviert Speicher für einen neuen Listenknoten
    if (neu == NULL) return FEHLER_SPEICHER;

    KARTENINHALT *inhalt = malloc(sizeof(KARTENINHALT)); // reserviert Speicher für den Inhalt
    if (inhalt == NULL) { free(neu); return FEHLER_SPEICHER; }

    strncpy(inhalt->frage, frage, FLENGTH - 1);
    inhalt->frage[FLENGTH - 1] = '\0';
    strncpy(inhalt->antwort, antwort, ALENGTH - 1);
    inhalt->antwort[ALENGTH - 1] = '\0';

    inhalt->intervall       = 1;    // Startwert für das Lernintervall in Tagen
    inhalt->wiederholungen  = 0;    // Noch keine Wiederholung erfolgt
    inhalt->efFaktor        = 2.5f; // SM-2 Standardwert
    inhalt->naechsteAbfrage = 0;    // Epoch 0 liegt in der Vergangenheit → sofort fällig

    neu->inhalt = inhalt;
    neu->next = NULL;
    neu->prev = NULL;

    if (*anfang == NULL) { // Liste ist leer, neu wird erstes Element
        inhalt->id = 0;
        *anfang = neu;
        return OK;
    }

    DATEIKARTE *karte = *anfang;
    while (karte->next != NULL) // wandert bis zum letzten Element
        karte = karte->next;

    inhalt->id = karte->inhalt->id + 1;
    karte->next = neu;  // letztes Element zeigt jetzt auf neu
    neu->prev = karte;  // neu zeigt zurück auf das bisherige letzte Element
    return OK;
}

FEHLERCODE deleteElement(DATEIKARTE **anfang, int id) {
    if (*anfang == NULL)
        return FEHLER_LISTE_LEER;

    DATEIKARTE *current = *anfang;
    while (current && current->inhalt->id != id)
        current = current->next;

    if (current == NULL)
        return FEHLER_NICHT_GEFUNDEN;

    if (current->prev)
        current->prev->next = current->next;
    if (current->next)
        current->next->prev = current->prev;
    if (*anfang == current)
        *anfang = current->next;

    free(current->inhalt); // Inhalt freigeben
    free(current);         // Knoten freigeben
    return OK;
}

void deleteList(DATEIKARTE **anfang) {
    DATEIKARTE *current = *anfang;
    while (*anfang != NULL) {
        current = current->next;
        free((*anfang)->inhalt); // Inhalt freigeben
        free(*anfang);           // Knoten freigeben
        *anfang = current;
    }
}

void printList(DATEIKARTE *anfang) {
    if (anfang == NULL) {
        printf("Liste ist leer\n");
        return;
    }
    time_t jetzt = time(NULL); // aktueller Zeitstempel zum Vergleich der Fälligkeiten
    printf("-------------------- Anfang der Liste --------------------\n");
    while (anfang != NULL) {
        KARTENINHALT *k = anfang->inhalt;
        printf("%d:\tQ: %s\n\tA: %s\n", k->id, k->frage, k->antwort);

        // Lernstatus anzeigen
        long sek = (long)(k->naechsteAbfrage - jetzt);
        if (k->wiederholungen >= GELERNT_SCHWELLE) {
            if (sek <= 0)
                printf("\t[GELERNT] Faellig\n");
            else
                printf("\t[GELERNT] Naechste Wiederholung in %ld Tag(e)\n", sek / 86400 + 1);
        } else if (sek <= 0) {
            printf("\tFaellig | Wdh.: %d | Intervall: %d Tag(e)\n",
                   k->wiederholungen, k->intervall);
        } else {
            printf("\tIn %ld Tag(e) faellig | Wdh.: %d | Intervall: %d Tag(e)\n",
                   sek / 86400 + 1, k->wiederholungen, k->intervall);
        }
        anfang = anfang->next;
    }
    printf("--------------------- Ende der Liste ---------------------\n");
}

void sortList(DATEIKARTE **anfang, int mode) {
    if (anfang == NULL || *anfang == NULL || (*anfang)->next == NULL)
        return;

    int swapped;
    DATEIKARTE *current;
    DATEIKARTE *last = NULL;

    do {
        swapped = 0;
        current = *anfang;
        while (current->next != last) {
            if (compareKarten(current, current->next, mode) > 0) {
                swapKarten(current, current->next); // nur Inhalt tauschen, Knoten bleiben an Ort und Stelle
                swapped = 1;
            } else {
                current = current->next;
            }
        }
        last = current;
    } while (swapped);
}

int compareKarten(const DATEIKARTE *a, const DATEIKARTE *b, int mode) {
    if (mode == 0) {
        // nach Frage sortieren, Groß-/Kleinschreibung ignorieren
        int r = strcmpIgnoreCase(a->inhalt->frage, b->inhalt->frage);
        if (r != 0) return r;
        // bei gleicher Frage: als zweites Kriterium ID
    }
    // nach ID sortieren
    if (a->inhalt->id < b->inhalt->id) return -1;
    if (a->inhalt->id > b->inhalt->id) return 1;
    return 0;
}

FEHLERCODE editElement(DATEIKARTE **anfang, int id, const char *neueFrage, const char *neueAntwort) {
    if (*anfang == NULL) return FEHLER_LISTE_LEER;

    DATEIKARTE *current = *anfang;
    while (current && current->inhalt->id != id)
        current = current->next;

    if (current == NULL) return FEHLER_NICHT_GEFUNDEN;

    if (neueFrage[0] != '\0') { // leerer String = Feld unverändert lassen
        strncpy(current->inhalt->frage, neueFrage, FLENGTH - 1);
        current->inhalt->frage[FLENGTH - 1] = '\0';
    }
    if (neueAntwort[0] != '\0') {
        strncpy(current->inhalt->antwort, neueAntwort, ALENGTH - 1);
        current->inhalt->antwort[ALENGTH - 1] = '\0';
    }
    return OK;
}

void swapKarten(DATEIKARTE *a, DATEIKARTE *b) {
    // Tauscht nur die Inhalts-Pointer – kein aufwendiges Umhängen von Listenknoten nötig
    KARTENINHALT *tmp = a->inhalt;
    a->inhalt = b->inhalt;
    b->inhalt = tmp;
}
