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

DATEIKARTE* addElement(DATEIKARTE **anfang, const char frage[], const char antwort[]);
void deleteElement(DATEIKARTE **anfang, int id);
void deleteList(DATEIKARTE **anfang);
void printList(DATEIKARTE *anfang);
void sortList(DATEIKARTE **anfang, int mode);
DATEIKARTE* getLast(DATEIKARTE *anfang);
void quickSortRec(DATEIKARTE **head, DATEIKARTE *low, DATEIKARTE *high, int mode);
DATEIKARTE* partition(DATEIKARTE **head, DATEIKARTE *low, DATEIKARTE *high, int mode);
int compareKarten(const DATEIKARTE *a, const DATEIKARTE *b, int mode);
void swapKarten(DATEIKARTE **anfang, DATEIKARTE *a, DATEIKARTE *b);

#endif //TESTATAUFGABE_FELIXFRASCH_1_0_LISTE_H