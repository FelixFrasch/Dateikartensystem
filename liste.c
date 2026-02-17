#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liste.h"

DATEIKARTE* addElement(DATEIKARTE** anfang, const char frage[], const char antwort[]) {
    DATEIKARTE *neu = malloc(sizeof(DATEIKARTE));
    if (neu == NULL) return NULL;

    strncpy(neu->frage, frage, FLENGTH-1); // Maximal FLENGTH-1 Zeichen werden kopiert
    neu->frage[FLENGTH - 1] = '\0'; // Terminiert auch bei zulanem String
    strncpy(neu->antwort, antwort, ALENGTH-1);
    neu->antwort[FLENGTH - 1] = '\0';

    neu->next = NULL;
    neu->prev = NULL;

    if (*anfang == NULL) {
        neu->id = 0;
        *anfang = neu;
        return *anfang;
    }

    DATEIKARTE *karte = *anfang;
    while (karte->next != NULL) {
        karte = karte->next;
    }

    neu->id = karte->id + 1;
    karte->next = neu;
    neu->prev = karte;
    return neu;
}

void deleteElement(DATEIKARTE **anfang, const int id) {
    if (*anfang == NULL) {
        printf("Die Liste ist leer!\n");
        return;
    }

    DATEIKARTE *current = *anfang;   // Pointer auf den aktuellen Zeiger

    // Suche Element
    while (current && current->id != id)
        current = current->next;

    // Nicht gefunden
    if (current == NULL) {
        printf("Element mit ID %d nicht gefunden!\n", id);
        return;
    }

    // Verkettung anpassen
    if (current->prev)
        current->prev->next = current->next;

    if (current->next)
        current->next->prev = current->prev;

    // Anfang-Pointer anpassen
    if (*anfang == current)
        *anfang = current->next;

    free(current);
    printf("Element mit ID %d gelöscht.\n", id);
}

void deleteList(DATEIKARTE **anfang) {
    DATEIKARTE *current = *anfang;
    while (*anfang != NULL) { // solange Liste nicht zu Ende
        current = current->next; // nächstes Element (evtl. NULL)
        free(*anfang); // Anfang freigegeben
        *anfang = current; // Anfang aufs nächste gesetzt (evtl. NULL)
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
    if (anfang == NULL || *anfang == NULL) return;

    DATEIKARTE *last = getLast(*anfang);
    quickSortRec(anfang, *anfang, last, mode);
}

DATEIKARTE* getLast(DATEIKARTE *current) {
    while (current && current->next) { // solange es ein nächtest Element gibt // current damit es keinen Fehler bei leerer Liste gibt
        current = current->next; // gehe zum nächsten Element
    }
    return current; // letztes Element
}

void quickSortRec(DATEIKARTE **head, DATEIKARTE *low, DATEIKARTE *high, int mode) {
    // obere & untere Grenze NULL, nur 1 Element, kein Element
    if (!low || !high || low == high || low == high->next) return;

    DATEIKARTE *p = partition(head, low, high, mode);

    if (p && p->prev)
        quickSortRec(head, low, p->prev, mode);
    if (p && p->next)
        quickSortRec(head, p->next, high, mode);
}

DATEIKARTE* partition(DATEIKARTE **head, DATEIKARTE *low, DATEIKARTE *high, int mode) {
    DATEIKARTE *pivot = high;
    DATEIKARTE *i = low->prev;

    for (DATEIKARTE *j = low; j != high; j = j->next) {
        if (compareKarten(j, pivot, mode) <= 0) {
            i = (i == NULL) ? low : i->next;
            if (i != j)
                swapKarten(head, i, j);
        }
    }

    i = (i == NULL) ? low : i->next;
    if (i != high)
        swapKarten(head, i, high);

    return i;
}

int compareKarten(const DATEIKARTE *a, const DATEIKARTE *b, int mode) {
    if (mode == 0) {
        // zuerst nach Frage sortieren
        int r = strcmp(a->frage, b->frage);
        if (r < 0) return -1;
        if (r > 0) return 1;
        // falls frage gleich, als zweites Kriterium id
        if (a->id < b->id) return -1;
        if (a->id > b->id) return 1;
        return 0;
    } else {
        // nur nach id sortieren
        if (a->id < b->id) return -1;
        if (a->id > b->id) return 1;
        return 0;
    }
}

void swapKarten(DATEIKARTE **anfang, DATEIKARTE *a, DATEIKARTE *b) {
    if (a == b || a == NULL || b == NULL) return;

    // Sicherstellen, dass a vor b in der Liste kommt
    DATEIKARTE *tmp = *anfang;
    int foundA = 0, foundB = 0;
    while (tmp) {
        if (tmp == a) { foundA = 1; break; }
        if (tmp == b) { foundB = 1; break; }
        tmp = tmp->next;
    }
    if (foundB && !foundA) { // Reihenfolge tauschen, so, dass a „früher“ ist
        DATEIKARTE *t = a;
        a = b;
        b = t;
    }

    DATEIKARTE *prevA = a->prev;
    DATEIKARTE *nextA = a->next;
    DATEIKARTE *prevB = b->prev;
    DATEIKARTE *nextB = b->next;

    // Falls a direkt vor b steht: a <-> b <-> ...
    if (nextA == b) {
        // a.prev -> b
        if (prevA) prevA->next = b;
        b->prev = prevA;

        // b.next bleibt nextB
        if (nextB) nextB->prev = a;
        a->next = nextB;

        // b <-> a
        b->next = a;
        a->prev = b;
    }
    // Falls b direkt vor a steht: b <-> a <-> ...
    else if (nextB == a) {
        // symmetrischer Fall – kann auf obigen Fall zurückgeführt werden
        // Einfach rekursiv aufrufen und Parameter tauschen
        swapKarten(anfang, b, a);
        return;
    } else {
        // Knoten sind nicht benachbart

        // Nachbarn von a umhängen
        if (prevA) prevA->next = b;
        if (nextA) nextA->prev = b;

        // Nachbarn von b umhängen
        if (prevB) prevB->next = a;
        if (nextB) nextB->prev = a;

        // a und b gegenseitig ihre prev/next tauschen
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
