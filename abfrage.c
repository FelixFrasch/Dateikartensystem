#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "abfrage.h"

// --- Hilfsfunktionen ---

// Groß-/Kleinschreibung ignorierender Stringvergleich
static int vergleicheIgnoreCase(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return tolower((unsigned char)*a) - tolower((unsigned char)*b);
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

// Prüft ob die erwartete Antwort eine Ja/Nein-Antwort ist
static int istJaNeinAntwort(const char *antwort) {
    char lower[ALENGTH];
    int i;
    for (i = 0; antwort[i] && i < ALENGTH - 1; i++)
        lower[i] = (char)tolower((unsigned char)antwort[i]);
    lower[i] = '\0';
    return (strcmp(lower, "ja") == 0 || strcmp(lower, "nein") == 0 ||
            strcmp(lower, "yes") == 0 || strcmp(lower, "no") == 0 ||
            strcmp(lower, "richtig") == 0 || strcmp(lower, "falsch") == 0);
}

// Normalisiert Ja/Nein-Varianten auf "ja" oder "nein"
// Akzeptiert: j/ja/y/yes/richtig/wahr -> "ja"; n/nein/no/falsch -> "nein"
static void normalisiereJaNein(char *s) {
    char lower[ALENGTH];
    int i;
    for (i = 0; s[i] && i < ALENGTH - 1; i++)
        lower[i] = (char)tolower((unsigned char)s[i]);
    lower[i] = '\0';

    if (strcmp(lower, "j") == 0 || strcmp(lower, "ja") == 0 ||
        strcmp(lower, "y") == 0 || strcmp(lower, "yes") == 0 ||
        strcmp(lower, "richtig") == 0 || strcmp(lower, "wahr") == 0) {
        strcpy(s, "ja");
    } else if (strcmp(lower, "n") == 0 || strcmp(lower, "nein") == 0 ||
               strcmp(lower, "no") == 0 || strcmp(lower, "falsch") == 0) {
        strcpy(s, "nein");
    }
    // Unbekannte Eingaben bleiben unverändert und schlagen beim Vergleich fehl
}

static int zaehleKarten(DATEIKARTE *anfang) {
    int n = 0;
    while (anfang) { n++; anfang = anfang->next; }
    return n;
}

// --- SM-2 Lernlogik ---

// Gibt 1 zurück wenn die Karte heute oder früher abgefragt werden soll
static int istFaellig(const KARTENINHALT *k) {
    return k->naechsteAbfrage <= time(NULL);
}

// Aktualisiert eine Karte nach dem vereinfachten SM-2-Algorithmus.
// richtig=1: Intervall wächst (1 -> 6 -> intervall*efFaktor), Wiederholungen +1.
// richtig=0: Zurücksetzen auf Anfang, sofort wieder fällig.
static void aktualisiereKarte(KARTENINHALT *k, int richtig) {
    if (!richtig) {
        k->wiederholungen  = 0;
        k->intervall       = 1;
        k->naechsteAbfrage = time(NULL); // sofort wieder fällig
    } else {
        // SM-2 Intervallberechnung: 1 -> 6 -> intervall * efFaktor
        if      (k->wiederholungen == 0) k->intervall = 1;
        else if (k->wiederholungen == 1) k->intervall = 6;
        else k->intervall = (int)(k->intervall * k->efFaktor + 0.5f);

        k->wiederholungen++;
        // Nächste Abfrage in intervall Tagen (86400 Sekunden pro Tag)
        k->naechsteAbfrage = time(NULL) + (time_t)k->intervall * 86400L;
    }
}

// --- Abfrage-Durchlauf ---

// Fragt die Karten aus pool ab, wendet SM-2 an und füllt falschPool.
// zaehltFuerScore: 1 = dieser Durchlauf zählt für das Ergebnis, 0 = Wiederholung
// Rückgabe: Anzahl richtig beantworteter Karten (nur wenn zaehltFuerScore=1 sinnvoll)
static int durchlauf(DATEIKARTE **pool, int n, DATEIKARTE **falschPool,
                     int *falschAnzahl, int *abgebrochen, int zaehltFuerScore) {
    int richtig = 0;
    *falschAnzahl = 0;
    *abgebrochen = 0;

    for (int i = 0; i < n; i++) {
        DATEIKARTE *karte = pool[i];
        char antwort[ALENGTH];
        int faellig = istFaellig(karte->inhalt); // SM-2 nur für fällige Karten anwenden

        printf("\n[%d/%d] Frage: %s\n", i + 1, n, karte->inhalt->frage);

        // Lernstatus vor der Frage anzeigen
        if (karte->inhalt->wiederholungen >= GELERNT_SCHWELLE)
            printf("      [GELERNT – Wdh.: %d | Intervall: %d Tag(e)]\n",
                   karte->inhalt->wiederholungen, karte->inhalt->intervall);

        // Hinweis wenn die Karte noch nicht fällig ist: SM-2 bleibt unverändert
        if (!faellig)
            printf("      (Noch nicht faellig – Lernfortschritt wird nicht veraendert)\n");

        while (1) {
            printf("Antwort (h=Hinweis, q=Beenden): ");
            if (fgets(antwort, sizeof(antwort), stdin) == NULL) {
                *abgebrochen = 1;
                return richtig;
            }
            antwort[strcspn(antwort, "\n")] = '\0';

            if (strcmp(antwort, "q") == 0) {
                *abgebrochen = 1;
                return richtig;
            }

            // Hinweis: ersten Buchstaben der Antwort zeigen
            if (strcmp(antwort, "h") == 0) {
                printf("Hinweis: Die Antwort beginnt mit '%c'\n", karte->inhalt->antwort[0]);
                continue;
            }

            // Kopien für die Normalisierung anlegen (Original bleibt unberührt)
            char antwortNorm[ALENGTH];
            strncpy(antwortNorm, antwort, ALENGTH - 1);
            antwortNorm[ALENGTH - 1] = '\0';

            char erwartetNorm[ALENGTH];
            strncpy(erwartetNorm, karte->inhalt->antwort, ALENGTH - 1);
            erwartetNorm[ALENGTH - 1] = '\0';

            // Ja/Nein-Normalisierung falls die erwartete Antwort eine Ja/Nein-Antwort ist
            if (istJaNeinAntwort(karte->inhalt->antwort)) {
                normalisiereJaNein(antwortNorm);
                normalisiereJaNein(erwartetNorm);
            }

            int korrekt = (vergleicheIgnoreCase(antwortNorm, erwartetNorm) == 0);

            if (faellig) {
                // Fällige Karte: SM-2 anwenden, Intervall aktualisieren
                aktualisiereKarte(karte->inhalt, korrekt);
                if (korrekt) {
                    printf("Richtig! (Naechste Wiederholung in %d Tag(e))\n",
                           karte->inhalt->intervall);
                    if (zaehltFuerScore) richtig++;
                } else {
                    printf("Falsch! Erwartet: %s\n", karte->inhalt->antwort);
                    falschPool[*falschAnzahl] = karte;
                    (*falschAnzahl)++;
                }
            } else {
                // Nicht fällige Karte: kein SM-2 Update, nur Rückmeldung zur Übung
                if (korrekt) {
                    printf("Richtig! (Kein SM-2 Update – Karte war noch nicht faellig)\n");
                    if (zaehltFuerScore) richtig++;
                } else {
                    printf("Falsch! Erwartet: %s (Kein SM-2 Update)\n", karte->inhalt->antwort);
                    falschPool[*falschAnzahl] = karte;
                    (*falschAnzahl)++;
                }
            }
            break; // weiter zur nächsten Karte
        }
    }
    return richtig;
}

// --- Hauptfunktion ---

void abfrageStarten(DATEIKARTE *anfang) {
    if (anfang == NULL) {
        printf("Liste ist leer.\n");
        return;
    }

    int gesamt = zaehleKarten(anfang);

    // Fällige und gelernte Karten zählen für die Übersicht
    int faellig = 0, gelerntVorher = 0;
    DATEIKARTE *tmp = anfang;
    while (tmp) {
        if (istFaellig(tmp->inhalt))                           faellig++;
        if (tmp->inhalt->wiederholungen >= GELERNT_SCHWELLE)   gelerntVorher++;
        tmp = tmp->next;
    }
    printf("Karten gesamt: %d | Heute faellig: %d | Gelernt (>= %d Wdh.): %d\n",
           gesamt, faellig, GELERNT_SCHWELLE, gelerntVorher);

    // Abfragebereich wählen
    printf("(f) Nur faellige Karten  (a) Alle Karten  (b) ID-Bereich: ");
    char eingabe[20];
    if (fgets(eingabe, sizeof(eingabe), stdin) == NULL) return;
    eingabe[strcspn(eingabe, "\n")] = '\0';
    for (int i = 0; eingabe[i]; i++)
        eingabe[i] = (char)tolower((unsigned char)eingabe[i]);

    // modus: 0=fällig, 1=alle, 2=Bereich
    int modus = 0;
    if      (strcmp(eingabe, "a") == 0) modus = 1;
    else if (strcmp(eingabe, "b") == 0) modus = 2;

    int vonId = 0, bisId = 0;
    if (modus == 2) {
        char buf[30];
        printf("Von ID: ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) return;
        if (sscanf(buf, "%d", &vonId) != 1) { printf("Ungueltige Eingabe.\n"); return; }
        printf("Bis ID: ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) return;
        if (sscanf(buf, "%d", &bisId) != 1) { printf("Ungueltige Eingabe.\n"); return; }
    }

    // Pool aufbauen: nur Karten die dem gewählten Modus entsprechen
    DATEIKARTE **pool = malloc(gesamt * sizeof(DATEIKARTE *));
    if (pool == NULL) { printf("Speicherfehler.\n"); return; }

    int n = 0;
    tmp = anfang;
    while (tmp) {
        int aufnehmen = 0;
        if      (modus == 0) aufnehmen = istFaellig(tmp->inhalt);
        else if (modus == 1) aufnehmen = 1;
        else                 aufnehmen = (tmp->inhalt->id >= vonId && tmp->inhalt->id <= bisId);
        if (aufnehmen) pool[n++] = tmp;
        tmp = tmp->next;
    }

    if (n == 0) {
        printf("Keine Karten in diesem Bereich.\n");
        free(pool);
        return;
    }

    // Zufällige Reihenfolge (Fisher-Yates)
    srand((unsigned int)time(NULL));
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        DATEIKARTE *t = pool[i];
        pool[i] = pool[j];
        pool[j] = t;
    }

    DATEIKARTE **falschPool = malloc(n * sizeof(DATEIKARTE *));
    if (falschPool == NULL) { free(pool); printf("Speicherfehler.\n"); return; }

    printf("\n---------- Abfrage beginnt (%d Karten) ----------\n", n);

    int falschAnzahl = 0;
    int abgebrochen = 0;

    // Erster Durchlauf – zählt für den Score, SM-2 wird angewendet
    int richtig = durchlauf(pool, n, falschPool, &falschAnzahl, &abgebrochen, 1);

    // Falsch beantwortete Karten wiederholen (kein Einfluss auf Score, SM-2 weiterhin aktiv)
    while (falschAnzahl > 0 && !abgebrochen) {
        printf("\n%d Karte(n) falsch beantwortet – werden wiederholt.\n", falschAnzahl);

        DATEIKARTE **neuPool = malloc(falschAnzahl * sizeof(DATEIKARTE *));
        if (neuPool == NULL) break;
        memcpy(neuPool, falschPool, falschAnzahl * sizeof(DATEIKARTE *));
        int neuN = falschAnzahl;

        // Wiederholung ebenfalls in zufälliger Reihenfolge
        for (int i = neuN - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            DATEIKARTE *t = neuPool[i];
            neuPool[i] = neuPool[j];
            neuPool[j] = t;
        }

        falschAnzahl = 0;
        durchlauf(neuPool, neuN, falschPool, &falschAnzahl, &abgebrochen, 0);
        free(neuPool);
    }

    // Auswertung
    printf("\n---------- Abfrage beendet ----------\n");
    printf("Ergebnis: %d/%d richtig (%.0f%%)\n",
           richtig, n, n > 0 ? (100.0 * richtig / n) : 0.0);

    // Lernfortschritt nach der Abfrage
    int gelerntNachher = 0;
    tmp = anfang;
    while (tmp) {
        if (tmp->inhalt->wiederholungen >= GELERNT_SCHWELLE) gelerntNachher++;
        tmp = tmp->next;
    }
    printf("Gelernte Karten: %d/%d (Definition: >= %d Wdh. in Folge richtig)\n",
           gelerntNachher, gesamt, GELERNT_SCHWELLE);
    if (gelerntNachher > gelerntVorher)
        printf("Neu gelernt: %d Karte(n)!\n", gelerntNachher - gelerntVorher);

    printf("Tipp: Mit 'w' Lernfortschritt in Datei speichern.\n");

    free(falschPool);
    free(pool);
}
