#ifndef TESTATAUFGABE_FELIXFRASCH_1_0_LISTE_H
#define TESTATAUFGABE_FELIXFRASCH_1_0_LISTE_H

#define FLENGTH 100
#define ALENGTH 100

typedef struct dateikarte
{
    int id;
    char frage[FLENGTH];
    char antwort[ALENGTH];
    struct dateikarte* next;
    struct dateikarte* prev;
} DATEIKARTE;

// Fuegt ein neues Element am Ende der Liste hinzu. Gibt Pointer auf neues Element zurueck, NULL bei Fehler.
DATEIKARTE* addElement(DATEIKARTE **anfang, const char frage[], const char antwort[]);

// Loescht das Element mit der angegebenen ID aus der Liste und gibt den Speicher frei.
void deleteElement(DATEIKARTE **anfang, const int id);

// Loescht alle Elemente der Liste und gibt den Speicher frei. Setzt *anfang auf NULL.
void deleteList(DATEIKARTE **anfang);

// Gibt alle Elemente der Liste auf dem Bildschirm aus.
void printList(DATEIKARTE *anfang);

// Sortiert die Liste mit Bubblesort durch Pointer-Tausch. mode: 0 = nach Frage, 1 = nach ID.
void sortList(DATEIKARTE **anfang, int mode);

// Vergleicht zwei Karten. Gibt <0 zurueck wenn a<b, >0 wenn a>b, 0 wenn gleich.
int compareKarten(const DATEIKARTE *a, const DATEIKARTE *b, int mode);

// Tauscht zwei Knoten in der Liste durch Umhaengen der Pointer (nicht durch Kopieren der Inhalte).
void swapKarten(DATEIKARTE **anfang, DATEIKARTE *a, DATEIKARTE *b);

#endif //TESTATAUFGABE_FELIXFRASCH_1_0_LISTE_H