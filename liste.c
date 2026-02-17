#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liste.h"

DATEIKARTE* addElement(DATEIKARTE** anfang, const char frage[], const char antwort[]) {
    DATEIKARTE *neu = malloc(sizeof(DATEIKARTE)); // reserviert Speicher für eine neue Karte auf dem Heap
    if (neu == NULL) return NULL; // Speicher konnte nicht reserviert werden

    strncpy(neu->frage, frage, FLENGTH-1); // kopiert max. FLENGTH-1 Zeichen von frage in neu->frage
    neu->frage[FLENGTH - 1] = '\0'; // setzt letztes Zeichen auf \0 (Stringende)
    strncpy(neu->antwort, antwort, ALENGTH-1);
    neu->antwort[ALENGTH - 1] = '\0';

    neu->next = NULL; // kein Nachfolger (wird am Ende eingefügt)
    neu->prev = NULL; // kein Vorgänger (wird unten gesetzt, falls Liste nicht leer)

    if (*anfang == NULL) { // Liste ist leer, neu wird zum ersten Element
        neu->id = 0;
        *anfang = neu;   // Zeiger auf Zeiger: ändert den originalen Pointer des Aufrufers
        return *anfang;
    }

    DATEIKARTE *karte = *anfang; // startet beim ersten Element
    while (karte->next != NULL) { // wandert bis zum letzten Element (next == NULL)
        karte = karte->next;
    }

    neu->id = karte->id + 1;
    karte->next = neu;  // letztes Element zeigt jetzt auf neu
    neu->prev = karte;  // neu zeigt zurück auf das bisherige letzte Element
    return neu;
}

void deleteElement(DATEIKARTE **anfang, const int id) {
    if (*anfang == NULL) {
        printf("Die Liste ist leer!\n");
        return;
    }

    DATEIKARTE *current = *anfang;   // Pointer auf den aktuellen Zeiger

    // Suche Element: durchläuft die Liste bis id gefunden oder Ende erreicht (NULL)
    while (current && current->id != id)
        current = current->next;

    // Nicht gefunden
    if (current == NULL) {
        printf("Element mit ID %d nicht gefunden!\n", id);
        return;
    }

    // Verkettung anpassen: Vorgänger und Nachfolger direkt verbinden, current wird übersprungen
    if (current->prev)                      // falls es einen Vorgänger gibt
        current->prev->next = current->next; // Vorgänger zeigt jetzt auf Nachfolger (überspringt current)

    if (current->next)                      // falls es einen Nachfolger gibt
        current->next->prev = current->prev; // Nachfolger zeigt jetzt zurück auf Vorgänger

    // Anfang-Pointer anpassen
    if (*anfang == current)
        *anfang = current->next;

    free(current); // gibt den Heap-Speicher des gelöschten Elements frei
    printf("Element mit ID %d gelöscht.\n", id);
}

void deleteList(DATEIKARTE **anfang) {
    DATEIKARTE *current = *anfang;
    while (*anfang != NULL) { // solange Liste nicht zu Ende
        current = current->next; // nächstes Element (ggf. NULL)
        free(*anfang); // Anfang freigegeben
        *anfang = current; // Anfang aufs nächste gesetzt (ggf. NULL)
    }
}

void printList(DATEIKARTE *anfang) {
    if  (anfang == NULL)
    {
        printf("Liste ist leer\n");
        return;
    }
    int i = 0;
    printf("-------------------- Anfang der Liste --------------------\n");
    while (anfang != NULL) {
        printf("%d:\tQ: %s\n\tA: %s\n",anfang->id, anfang->frage, anfang->antwort);
        anfang = anfang->next;
        i++;
    }
    printf("--------------------- Ende der Liste ---------------------\n");
}


void sortList(DATEIKARTE **anfang, int mode) { // 0 = Frage; 1 = ID
    if (anfang == NULL || *anfang == NULL || (*anfang)->next == NULL)
        return; // Leere Liste oder nur 1 Element

    int swapped;
    // ReSharper disable once CppJoinDeclarationAndAssignment
    DATEIKARTE *current;
    DATEIKARTE *last = NULL;

    do {
        swapped = 0;
        current = *anfang;

        while (current->next != last) {
            // vergleiche current mit current->next
            if (compareKarten(current, current->next, mode) > 0) { // wenn aktuelles Element größer als nächstes Element
                swapKarten(anfang, current, current->next); // Karten tauschen
                swapped = 1;

                // Nach swap: current ist jetzt weiter hinten
                // Wir müssen nicht weitergehen, da current->next jetzt das ist,
                // was vorher current war
            } else {
                current = current->next;
            }
        }
        last = current; // letztes Element ist jetzt sortiert
    } while (swapped);
}


int compareKarten(const DATEIKARTE *a, const DATEIKARTE *b, int mode) { // a<b -> -1 // a>b -> 1 // ID gleich -> 0
    if (mode == 0) {
        // zuerst nach Frage sortieren
        int r = strcmp(a->frage, b->frage); // strcmp gibt <0, 0, oder >0 zurück (alphabetischer Vergleich)
        if (r != 0) return r;
        // falls frage gleich, als zweites Kriterium id
    }
        // nur nach id sortieren
        if (a->id < b->id) return -1;
        if (a->id > b->id) return 1;
        return 0;
}

void swapKarten(DATEIKARTE **anfang, DATEIKARTE *a, DATEIKARTE *b) {
    if (a == b || a == NULL || b == NULL) return;

    // Sicherstellen, dass a vor b in der Liste kommt
    DATEIKARTE *tmp = *anfang;
    int aFirst = 0; // wird 1, wenn a vor b in der Liste gefunden wird
    while (tmp) {
        if (tmp == a) { aFirst = 1; break; } // a wurde zuerst gefunden
        if (tmp == b) { break; }              // b wurde zuerst gefunden
        tmp = tmp->next;
    }
    if (!aFirst) { // b kommt vor a, also tauschen, damit a immer der frühere Knoten ist
        DATEIKARTE *t = a;
        a = b;
        b = t;
    }

    DATEIKARTE *prevA = a->prev;
    DATEIKARTE *nextA = a->next;
    DATEIKARTE *prevB = b->prev;
    DATEIKARTE *nextB = b->next;

    // Falls a direkt vor b steht: vorher [prevA] <-> [a] <-> [b] <-> [nextB], nachher [prevA] <-> [b] <-> [a] <-> [nextB]
    if (nextA == b) {
        if (prevA) prevA->next = b; // prevA zeigt jetzt auf b statt auf a
        b->prev = prevA;            // b zeigt zurück auf prevA statt auf a

        if (nextB) nextB->prev = a; // nextB zeigt zurück auf a statt auf b
        a->next = nextB;            // a zeigt jetzt auf nextB statt auf b

        b->next = a;                // b zeigt jetzt auf a (b ist jetzt vorne)
        a->prev = b;                // a zeigt zurück auf b (a ist jetzt hinten)
    }
    // Falls b direkt vor a steht: b <-> a <-> ...
    else if (nextB == a) {
        // symmetrischer Fall – kann auf obigen Fall zurückgeführt werden
        // Einfach rekursiv aufrufen und Parameter tauschen
        swapKarten(anfang, b, a);
        return;
    } else {
        // Knoten sind nicht benachbart

        // Nachbarn von a zeigen jetzt auf b (b nimmt a's Platz ein)
        if (prevA) prevA->next = b;
        if (nextA) nextA->prev = b;

        // Nachbarn von b zeigen jetzt auf a (a nimmt b's Platz ein)
        if (prevB) prevB->next = a;
        if (nextB) nextB->prev = a;

        // a bekommt b's alte Nachbarn, b bekommt a's alte Nachbarn
        a->prev = prevB;
        a->next = nextB;

        b->prev = prevA;
        b->next = nextA;
    }

    // Falls Kopf betroffen: head aktualisieren
    if (*anfang == a) {
        *anfang = b;
    } else if (*anfang == b) {
        *anfang = a;
    }
}
