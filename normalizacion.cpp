#include <iostream>
#include <cstdio> 
#include <cstring>
using namespace std;

struct ComandaHistorica {
char fecha[11]; // "DD-MM-AAAA"
char nombreMozo[50]; // el nombre completo, repetido en cada venta
int codigoProducto; int cantidad; float comision;
};

struct Mozo { 
 int idMozo; 
 char nombre[50];
 float totalComision; 
 };

int main (){
	
	FILE* f= fopen("comandas_historicas.dat", "rb");
  if (f == NULL) {
     cout << "No se pudo abrir el archivo." << endl;
 return 0;
}

ComandaHistorica c;
Mozo lista [50];// lista que tendra el nombre de los mozos y sus datos
int cantidadMozos=0; // cuenta la cantidad de mozos

while (fread(&c, sizeof(ComandaHistorica), 1, f) == 1) {
        
        int encontrado = 0;

        for (int i = 0; i < cantidadMozos; i++) { // si el mozo ya esta cargado en nuestra lista le sumamos la comisión de esta venta

            if (strcmp(lista[i].nombre, c.nombreMozo) == 0) {
                encontrado = 1;
                lista[i].totalComision += c.comision;
            }
        }

        // si no lo encontramos, cargamos un mozo nuevo
        if (encontrado == 0) {
            lista[cantidadMozos].idMozo = cantidadMozos + 1; // le asiganmos un id
            
            strcpy(lista[cantidadMozos].nombre, c.nombreMozo); // guardamos su nombre

            lista[cantidadMozos].totalComision = c.comision; //inicializamos su comisión acumulada con la comisión de esta primera venta
            
            cantidadMozos++; // aumentamos el contador de mozos

        }
        
    }
	fclose (f);
	
	// creamos un archivo para ir guardando lo q cargamos en lista
	FILE* fmozos = fopen("mozos.dat", "wb");
    if (fmozos == NULL) {
        cout << "No se pudo crear el archivo" << endl;
        return 0;
    }
    
    fwrite(lista, sizeof(Mozo), cantidadMozos, fmozos);

    fclose(fmozos);
    
    cout << "Archivo mozos.dat generado exitosamente con " << cantidadMozos << " mozos." << endl;
	return 0;
}

