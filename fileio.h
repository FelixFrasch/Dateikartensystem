#ifndef TESTATAUFGABE_FELIXFRASCH_1_0_FILEIO_H
#define TESTATAUFGABE_FELIXFRASCH_1_0_FILEIO_H

#include "liste.h"

// Liest Karteikarten aus dateiname und ersetzt die aktuelle Liste. Gibt OK oder FEHLER_DATEI zurück.
FEHLERCODE readFromFile(DATEIKARTE **anfang, const char *dateiname);

// Schreibt alle Karteikarten im Format "frage;antwort" in dateiname. Gibt OK oder FEHLER_DATEI zurück.
FEHLERCODE writeToFile(DATEIKARTE *anfang, const char *dateiname);

// Löscht die angegebene Datei vom Dateisystem. Gibt OK oder FEHLER_DATEI zurück.
FEHLERCODE deleteFile(const char *dateiname);

#endif //TESTATAUFGABE_FELIXFRASCH_1_0_FILEIO_H
