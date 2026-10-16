#include <iostream>
#include <cstdio>
using namespace std;

// Usamos la estructura sugerida por la cátedra para leer las ventas
struct Comanda { 
    int idMozo; 
    int codigoProducto; 
    int cantidad; 
    float comision; 
};

int main() {
    char nombreArchivo[50];
    cout << "Ingrese el nombre del archivo semanal a procesar (ej: comandas_semana_s1-06.dat): ";
    cin >> nombreArchivo;

    // Abrimos el archivo semanal en modo lectura binaria
    FILE* f = fopen(nombreArchivo, "rb");
    if (f == NULL) {
        cout << "Error: No se pudo abrir " << nombreArchivo << endl;
        return 1;
    }

    Comanda c;
    int totalProductosBuffet = 0; 

    //  Leemos el primer registro antes de entrar al ciclo para inicializar
    int leido = fread(&c, sizeof(Comanda), 1, f);

    cout << "\n--- RESUMEN DE CIERRE SEMANAL ---" << endl;

    //  Ciclo principal (recorre todo el archivo)
    while (leido == 1) {
        // Guardamos el mozo actual y reiniciamos sus contadores
        int idMozoActual = c.idMozo;
        int totalProductosMozo = 0;
        float totalComisionMozo = 0.0;

        // Ciclo secundario (agrupa todo lo que sea del MISMO mozo)
        while (leido == 1 && c.idMozo == idMozoActual) {
            totalProductosMozo += c.cantidad;
            totalComisionMozo += c.comision;
            
            // Sumamos al contador general del buffet
            totalProductosBuffet += c.cantidad;

            // Leemos la siguiente comanda
            leido = fread(&c, sizeof(Comanda), 1, f);
        }

        //  Corte: Cuando cambia el mozo, mostramos sus totales
        cout << "Mozo ID " << idMozoActual << " | Productos vendidos: " << totalProductosMozo 
             << " | Comision a pagar: $" << totalComisionMozo << endl;
    }

    //Mostrar el total general exigido por Alberto
    cout << "------------------------------------------------" << endl;
    cout << "TOTAL DEL BUFFET: " << totalProductosBuffet << " productos vendidos en la semana." << endl;

    fclose(f);
    return 0;
}
