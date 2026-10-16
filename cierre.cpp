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

// Estructura auxiliar para guardar una comanda junto con el numero del dia al que pertenece.
struct ComandaSemana {
    int dia;
    Comanda comanda;
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

    //Vector para reunir las comandas de los siete dias.
    ComandaSemana comandas[5000];
    int cantidadComandas = 0;

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

    for (int i = 0; i < 7; i++) {
        char nombreArchivo[40];
        snprintf(nombreArchivo, sizeof(nombreArchivo), "comandas_%02d-%02d-%04d.dat",
                 dia, mesActual, anioActual);

        FILE* fDia = fopen(nombreArchivo, "rb");

        if (fDia != NULL) {
            Comanda c;

            while (cantidadComandas < 5000 && fread(&c, sizeof(Comanda), 1, fDia) == 1) {
                comandas[cantidadComandas].dia = i;
                comandas[cantidadComandas].comanda = c;
                cantidadComandas++;
            }

            fclose(fDia);
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

    // Ordenamos por ID de mozo y, si coincide, por dia de la semana.
    for (int i = 0; i < cantidadComandas - 1; i++) {
        for (int j = 0; j < cantidadComandas - i - 1; j++) {

            int id1 = comandas[j].comanda.idMozo;
            int id2 = comandas[j + 1].comanda.idMozo;

            if (id1 > id2 || (id1 == id2 && comandas[j].dia > comandas[j + 1].dia)) {
                ComandaSemana aux = comandas[j];
                comandas[j] = comandas[j + 1];
                comandas[j + 1] = aux;
            }
        }
    }

    //Creamos el archivo semanal.
    char nombreSemanal[50];

    sprintf(nombreSemanal, "comandas_semana_s%d-%02d.dat", semana, mes);

    FILE* fSemana = fopen(nombreSemanal, "wb");

    if (fSemana == NULL) {
        cout << "No se pudo crear el archivo semanal." << endl;
        return 1;
    }

    // Escribimos solamente las comandas, sin el campo auxiliar dia.
    for (int i = 0; i < cantidadComandas; i++) {
        fwrite(&comandas[i].comanda, sizeof(Comanda), 1, fSemana);
    }

    fclose(fSemana);

    cout << "Archivo semanal generado correctamente: "
         << nombreSemanal << endl;
    cout << "Cantidad de comandas reunidas: "
         << cantidadComandas << endl;
    cout << "Cantidad de mozos registrados: "
         << cantidadMozos << endl;

    return 0;
}
