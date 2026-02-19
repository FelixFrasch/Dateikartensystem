#ifndef TESTATAUFGABE_FELIXFRASCH_1_0_FILEIO_H
#define TESTATAUFGABE_FELIXFRASCH_1_0_FILEIO_H

#include "liste.h"

// Liest Karteikarten aus dateiname und ersetzt die aktuelle Liste. Gibt 0 bei Erfolg, 1 bei Fehler zurück.
int readFromFile(DATEIKARTE **anfang, const char *dateiname);

// Schreibt alle Karteikarten im Format "frage;antwort" in dateiname. Gibt 0 bei Erfolg, 1 bei Fehler zurück.
int writeToFile(DATEIKARTE *anfang, const char *dateiname);

// Löscht die angegebene Datei vom Dateisystem. Gibt 0 bei Erfolg, 1 bei Fehler zurück.
int deleteFile(const char *dateiname);

#endif //TESTATAUFGABE_FELIXFRASCH_1_0_FILEIO_H
