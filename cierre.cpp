#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

// Estructura de cada comanda diaria.
struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

// Estructura para consultar los mozos.
struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

bool esBisiesto(int anio) {
    return anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0);
}

int diasDelMes(int mes, int anio) {
    const int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (mes == 2 && esBisiesto(anio)) {
        return 29;
    }
    return diasPorMes[mes - 1];
}

int main() {
    // Solicitamos el numero de semana y el mes.
    int semana, mes, anio;

    cout << "Ingrese el numero de semana: ";
    cin >> semana;
    cout << "Ingrese el numero de mes (1-12): ";
    cin >> mes;
    cout << "Ingrese el anio: ";
    cin >> anio;

    if (semana < 1 || semana > 5 || mes < 1 || mes > 12 || anio < 1) {
        cout << "Semana, mes o anio invalido." << endl;
        return 1;
    }

    // Abrimos el archivo de mozos para verificar los IDs.
    FILE* fMozos = fopen("mozos.dat", "rb");

    if (fMozos == NULL) {
        cout << "No se pudo abrir mozos.dat." << endl;
        return 1;
    }

    Mozo mozo;
    int cantidadMozos = 0;
    while (fread(&mozo, sizeof(Mozo), 1, fMozos) == 1) {
        cantidadMozos++;
    }
    fclose(fMozos);

    // Recorremos los dias de la semana.
    // El formato de los archivos diarios es dd-mm-aaaa.
    // Para construir sus nombres necesitamos el dia y el mes.
    // Se solicita el dia inicial de la semana para poder identificar correctamente los siete archivos.
    int diaInicial;

    cout << "Ingrese el dia del mes en que comienza la semana: ";
    cin >> diaInicial;

    if (diaInicial < 1 || diaInicial > diasDelMes(mes, anio)) {
        cout << "Dia invalido para ese mes y anio." << endl;
        return 1;
    }

    int dia = diaInicial;
    int mesActual = mes;
    int anioActual = anio;

    FILE* archivosDia[7] = {};
    Comanda actual[7];
    bool hayComanda[7] = {};

    for (int i = 0; i < 7; i++) {
        char nombreArchivo[40];
        snprintf(nombreArchivo, sizeof(nombreArchivo), "comandas_%02d-%02d-%04d.dat",
                 dia, mesActual, anioActual);

        archivosDia[i] = fopen(nombreArchivo, "rb");
        if (archivosDia[i] != NULL) {
            if (fread(&actual[i], sizeof(Comanda), 1, archivosDia[i]) == 1) {
                hayComanda[i] = true;
            } else if (ferror(archivosDia[i])) {
                cout << "Error al leer " << nombreArchivo << "." << endl;
                for (int j = 0; j <= i; j++) {
                    if (archivosDia[j] != NULL) {
                        fclose(archivosDia[j]);
                    }
                }
                return 1;
            } else {
                fclose(archivosDia[i]);
                archivosDia[i] = NULL;
            }
        }

        dia++;
        if (dia > diasDelMes(mesActual, anioActual)) {
            dia = 1;
            mesActual++;
            if (mesActual > 12) {
                mesActual = 1;
                anioActual++;
            }
        }
    }

    // Apareamos los siete archivos diarios ya ordenados por ID de mozo.
    char nombreSemanal[50];
    sprintf(nombreSemanal, "comandas_semana_s%d-%02d.dat", semana, mes);

    FILE* fSemana = fopen(nombreSemanal, "wb");

    if (fSemana == NULL) {
        cout << "No se pudo crear el archivo semanal." << endl;
        for (int i = 0; i < 7; i++) {
            if (archivosDia[i] != NULL) {
                fclose(archivosDia[i]);
            }
        }
        return 1;
    }

    int cantidadComandas = 0;
    bool errorLectura = false;
    bool errorEscritura = false;

    while (true) {
        int diaMenor = -1;

        for (int i = 0; i < 7; i++) {
            if (hayComanda[i] &&
                (diaMenor == -1 || actual[i].idMozo < actual[diaMenor].idMozo)) {
                diaMenor = i;
            }
        }

        if (diaMenor == -1) {
            break;
        }

        if (fwrite(&actual[diaMenor], sizeof(Comanda), 1, fSemana) != 1) {
            errorEscritura = true;
            break;
        }
        cantidadComandas++;

        if (fread(&actual[diaMenor], sizeof(Comanda), 1, archivosDia[diaMenor]) == 1) {
            hayComanda[diaMenor] = true;
        } else if (ferror(archivosDia[diaMenor])) {
            errorLectura = true;
            break;
        } else {
            hayComanda[diaMenor] = false;
        }
    }

    for (int i = 0; i < 7; i++) {
        if (archivosDia[i] != NULL) {
            fclose(archivosDia[i]);
        }
    }

    fclose(fSemana);

    if (errorLectura || errorEscritura) {
        cout << (errorLectura
                     ? "Error al leer uno de los archivos diarios."
                     : "Error al escribir el archivo semanal.")
             << endl;
        return 1;
    }

    cout << "Archivo semanal generado correctamente: "
         << nombreSemanal << endl;
    cout << "Cantidad de comandas reunidas: "
         << cantidadComandas << endl;
    cout << "Cantidad de mozos registrados: "
         << cantidadMozos << endl;

    return 0;
}
