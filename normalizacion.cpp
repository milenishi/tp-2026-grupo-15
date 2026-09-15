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
    char password[20]; // agregamos este campo para almacenar la clave encriptada, exigido por el TP para el login seguro
    float totalComision; 
};

//  Creamos esta estructura para las planillas diarias porque el TP requiere que guarden el idMozo numérico en vez del nombre largo
struct Comanda { 
    int idMozo;       //  Guarda el identificador numérico del mozo
    int codigoProducto; //  Guarda el código del producto vendido
    int cantidad;     // Guarda la cantidad vendida del producto
    float comision;   // Guarda la comisión correspondiente a la venta
};

//  Creamos esta estructura para manejar el inventario y poder actualizar el stock
struct Producto {
    int codigo;         //  Código único del producto
    char descripcion[50]; // Descripción del producto.
    float precio;       // Precio unitario del product.
    int stockActual;    // Stock disponible que iremos descontando
};

// Declaramos una variable global K fuera del main para usarla como corrimiento fijo en claves
int K = 5;

// Creamos esta función para cifrar la contraseña inicial sumándole el valor K a cada carácter.
void encriptarPassword(char origen[], char destino[]) {
    int i = 0;                                       // Inicializamos el índice para recorrer el texto carácter por carácter.
    while (origen[i] != '\0') {                      //  Bucle que recorre la cadena hasta encontrar el carácter nulo de fin.
        destino[i] = origen[i] + K;                  //  Al carácter actual le sumamos K para encriptarlo
        i++;                                         //  Avanzamos a la siguiente posición del texto.
    }
    destino[i] = '\0';                               //  Colocamos el fin de cadena al final del texto ya encriptado.
}

// Creamos esta función para actualizar el stock directamente en el vec
void actualizarStockEnMemoria(Producto inv[], int cantProd, int codProd, int cantVendida) {
    for (int i = 0; i < cantProd; i++) {             //  Recorremos todo el vec de productos en memoria principal.
        if (inv[i].codigo == codProd) {              //  Buscamos el producto cuyo código coincida con el vendido.
            inv[i].stockActual -= cantVendida;       //  Restamos la cantidad vendida al stock actual.
            break;                                   //  Salimos del bucle porque ya encontramos y actualizamos el producto.
        }
    }
}

int main (){
    FILE* f = fopen("comandas_historicas.dat", "rb");
    if (f == NULL) {
        cout << "No se pudo abrir el archivo." << endl;
        return 0;
    }

    // Declaramos un vec estático para cargar todo el archivo histórico 
    ComandaHistorica v[500];
    // Variable para llevar la cuenta exacta de cuántas comandas leímos del archivo.
    int cantComandas = 0;
    ComandaHistorica c;

    // Leemos el archivo histórico completo y lo guardamos en el arreglo v para ordenarlo después
    while (fread(&c, sizeof(ComandaHistorica), 1, f) == 1 && cantComandas < 500) {
        v[cantComandas] = c;                             //  Almacenamos cada comanda leída en la posición del vec.
        cantComandas++;                                  // Incrementamos el contador de comandas almacenadas
    }
    fclose(f);

    // Validamos si el arreglo quedó vacío para evitar errores de ejecución en el ordenamiento.
    if (cantComandas == 0) return 0;

    //Aplicamos ordenamiento burbuja para ordenar el arreglo primero por fecha y luego por nombre de mozo
    for (int i = 0; i < cantComandas - 1; i++) {
        for (int j = 0; j < cantComandas - i - 1; j++) {
            int comp = strcmp(v[j].fecha, v[j+1].fecha); //  Comparamos las fechas de dos comandas contiguas.
            if (comp > 0 || (comp == 0 && strcmp(v[j].nombreMozo, v[j+1].nombreMozo) > 0)) { 
                ComandaHistorica aux = v[j];             // Pasamos el elemento siguiente al lugar actual.
                v[j+1] = aux;                            // Asignamos el auxiliar a la siguiente posición.
            }
        }
    }

    //  Creamos un vec para cargar todo el inventario desde el archivo inventario.dat
    Producto inv[100];
    //  Contador para saber cuántos productos hay cargados en la memoria principal.
    int cantProductos = 0;
    // Abrimos el archivo de inventario 
    FILE* finv = fopen("inventario.dat", "rb");
    if (finv != NULL) {
        // Leemos todo el inventario hacia el vec
        while (fread(&inv[cantProductos], sizeof(Producto), 1, finv) == 1 && cantProductos < 100) {
            cantProductos++;                             // Incrementamos el contador de productos cargados.
        }
        fclose(finv);                                    // Cerramos el archivo de inventario tras pasarlo al vec
    }

    Mozo lista[50];
    int cantidadMozos = 0; 

    // Modificamos el bucle para recorrer el arreglo ordenado `v` en lugar de leer directo del archivo viejo.
    for (int i = 0; i < cantComandas; i++) {
        int encontrado = 0;

        for (int j = 0; j < cantidadMozos; j++) { 
            if (strcmp(lista[j].nombre, v[i].nombreMozo) == 0) {
                encontrado = 1;
                lista[j].totalComision += v[i].comision;
            }
        }

        if (encontrado == 0) {
            lista[cantidadMozos].idMozo = cantidadMozos + 1; 
            strcpy(lista[cantidadMozos].nombre, v[i].nombreMozo); 
            lista[cantidadMozos].totalComision = v[i].comision; 
            
            // Creamos un texto temporal para guardar el ID numérico del mozo
            char passTemp[20];
            sprintf(passTemp, "%d", lista[cantidadMozos].idMozo); // Convertimos el ID entero a formato de texto para usarlo como clave inicial
            encriptarPassword(passTemp, lista[cantidadMozos].password); // Encriptamos esa contraseña inicial y la guardamos en el mozo.

            cantidadMozos++; 
        }
    }
    
    FILE* fmozos = fopen("mozos.dat", "wb");
    if (fmozos == NULL) {
        cout << "No se pudo crear el archivo" << endl;
        return 0;
    }
    
    fwrite(lista, sizeof(Mozo), cantidadMozos, fmozos);
    fclose(fmozos);
    
    cout << "Archivo mozos.dat generado exitosamente con " << cantidadMozos << " mozos." << endl;

    // Declaramos una variable para almacenar la fecha actual que se está procesando en el corte de control
    char fechaActual[11] = "";
    
    FILE* fDia = NULL;

    // Recorremos nuevamente el vec ordenado para hacer el corte de control por fecha
    for (int i = 0; i < cantComandas; i++) {
        // Comprobamos si cambió la fecha respecto a la iteración anterior.
        if (strcmp(v[i].fecha, fechaActual) != 0) {
            if (fDia != NULL) fclose(fDia);               //  Si ya había un archivo diario abierto, lo cerramos antes de cambiar de día
            strcpy(fechaActual, v[i].fecha);              //  Actualizamos la fecha actual con la nueva fecha encontrada
            
            char nombreArch[30];                          // Variable temporal para construir el nombre del archivo diario
            sprintf(nombreArch, "comandas_%s.dat", fechaActual); //  Armamos el nombre  con el formato comandas_dd-mm-aaaa.dat
            fDia = fopen(nombreArch, "wb");               //  Abrimos el nuevo archivo diario 
        }

        //  Buscamos el ID numérico que le corresponde al nombre del mozo actual
        int idMozoEncontrado = 0;
        for (int j = 0; j < cantidadMozos; j++) {
            if (strcmp(lista[j].nombre, v[i].nombreMozo) == 0) {
                idMozoEncontrado = lista[j].idMozo;       // Guardamos el ID numérico hallado
                break;
            }
        }

        // Creamos una comanda con la estructura reducida 
        Comanda comandaDiaria;
        comandaDiaria.idMozo = idMozoEncontrado;         //  Asignamos el ID del mozo.
        comandaDiaria.codigoProducto = v[i].codigoProducto; //  Copiamos el código del producto.
        comandaDiaria.cantidad = v[i].cantidad;         // : Copiamos la cantidad vendida
        comandaDiaria.comision = v[i].comision;         //  Copiamos la comisión de la venta

        if (fDia != NULL) {
            fwrite(&comandaDiaria, sizeof(Comanda), 1, fDia); //  Escribimos la comanda reducida en el archivo del día correspondiente
        }
        
        //  Llamamos a la función para descontar la cantidad vendida en el inventario almacenado en el vec
        actualizarStockEnMemoria(inv, cantProductos, comandaDiaria.codigoProducto, comandaDiaria.cantidad);
    }

    //  Cerramos el último archivo diario que haya quedado abierto al finalizar el bucle
    if (fDia != NULL) fclose(fDia);

    // Abrimos el archivo inventario.dat en modo escritura para sobrescribirlo con los datos ya actualizados desde el vec
    finv = fopen("inventario.dat", "wb");
    if (finv != NULL) {
        fwrite(inv, sizeof(Producto), cantProductos, finv); //  ponemos el vec ene le archivo
        fclose(finv);                                    
    }

    return 0;
}
