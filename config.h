#ifndef TESTATAUFGABE_FELIXFRASCH_1_0_CONFIG_H
#define TESTATAUFGABE_FELIXFRASCH_1_0_CONFIG_H

#define FLENGTH          100
#define ALENGTH          100
#define DATEINAME        "Karteikarten.csv"
#define GELERNT_SCHWELLE 3      // Anzahl erfolgreicher Wiederholungen in Folge, ab der eine Karte als gelernt gilt
#define SEP              "\x1F" // ASCII 31 (Unit Separator) – Feldtrenner in der CSV-Datei;
                                // kann nicht über normale Tastaturen eingegeben werden

// Einheitliche Fehlercodes für alle Funktionen im Projekt.
typedef enum {
    OK = 0,
    FEHLER_SPEICHER,        // malloc fehlgeschlagen
    FEHLER_LISTE_LEER,      // Liste ist leer
    FEHLER_NICHT_GEFUNDEN,  // Element mit gesuchter ID nicht vorhanden
    FEHLER_DATEI,           // Datei konnte nicht geöffnet/gelöscht werden
} FEHLERCODE;

#endif //TESTATAUFGABE_FELIXFRASCH_1_0_CONFIG_H