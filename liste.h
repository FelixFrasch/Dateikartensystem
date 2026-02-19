#ifndef TESTATAUFGABE_FELIXFRASCH_1_0_LISTE_H
#define TESTATAUFGABE_FELIXFRASCH_1_0_LISTE_H

#include "config.h"

// Inhalt einer Karteikarte (ausgelagert damit swapKarten nur Pointer tauscht, nicht Knoten)
typedef struct karteninhalt {
    int id;
    char frage[FLENGTH];
    char antwort[ALENGTH];
} KARTENINHALT;

typedef struct dateikarte {
    KARTENINHALT *inhalt;        // Pointer auf den Inhalt (Frage, Antwort, ID)
    struct dateikarte *next;
    struct dateikarte *prev;
} DATEIKARTE;

// Fügt ein neues Element am Ende der Liste hinzu. Gibt OK oder FEHLER_SPEICHER zurück.
FEHLERCODE addElement(DATEIKARTE **anfang, const char frage[], const char antwort[]);

// Löscht das Element mit der angegebenen ID. Gibt OK, FEHLER_LISTE_LEER oder FEHLER_NICHT_GEFUNDEN zurück.
FEHLERCODE deleteElement(DATEIKARTE **anfang, int id);

// Löscht alle Elemente der Liste und gibt den Speicher frei. Setzt *anfang auf NULL.
void deleteList(DATEIKARTE **anfang);

// Gibt alle Elemente der Liste auf dem Bildschirm aus.
void printList(DATEIKARTE *anfang);

// Sortiert die Liste mit Bubblesort. mode: 0 = nach Frage, 1 = nach ID.
void sortList(DATEIKARTE **anfang, int mode);

// Vergleicht zwei Karten. Gibt <0 zurück wenn a<b, >0 wenn a>b, 0 wenn gleich.
int compareKarten(const DATEIKARTE *a, const DATEIKARTE *b, int mode);

// Tauscht die Inhalts-Pointer zweier Knoten (kein aufwendiges Umhängen der Listenknoten nötig).
void swapKarten(DATEIKARTE *a, DATEIKARTE *b);

#endif //TESTATAUFGABE_FELIXFRASCH_1_0_LISTE_H
