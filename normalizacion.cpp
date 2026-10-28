#include <iostream>
#include <cstdio> 
#include <cstring>
using namespace std;

struct ComandaHistorica {
    char fecha[11]; // "DD-MM-AAAA"
    char nombreMozo[50]; // el nombre completo, repetido en cada venta
    int codigoProducto; int cantidad; float comision;
};
//Para el archivo de mozos.dat
struct Mozo { 
    int idMozo; 
    char nombre[50];
    char password[20]; 
    float totalComision; 
};
//Para los archivos de comandas por dia
struct Comanda { 
    int idMozo;       //  Guarda el identificador numerico del mozo
    int codigoProducto; //  Guarda el codigo del producto vendido
    int cantidad;     // Guarda la cantidad vendida del producto
    float comision;   // Guarda la comision correspondiente a la venta
};

struct Producto {
    int codigo;         //  Codigo del producto
    char descripcion[50]; // Nombre del producto
    float precio;       // Precio unitario del producto
    int stockActual;    // Stock disponible 
};
// Declaramos una variable global k fuera del main para usarla como corrimiento fijo en claves
int k = 5;

// Creamos esta funcion para establecer la contrasenia inicial sumandole el valor k a cada caracter.
void encriptarPassword(char origen[], char destino[]) {
    int i = 0;                                       // Inicializamos el indice para recorrer el texto caracter por caracter.
    while (origen[i] != '\0') {                      //  Bucle que recorre la cadena hasta encontrar el caracter nulo de fin.
        destino[i] = origen[i] + k;                  //  Al caracter actual le sumamos k para encriptarlo
        i++;                                         //  Avanzamos a la siguiente posicion del texto.
    }
    destino[i] = '\0';                               //  Colocamos el fin de cadena al final del texto ya encriptado.
}

// Creamos esta funcion para actualizar el stock directamente en el archivo
void actualizarStock(FILE* inventario, int codProd, int cantVendida) {
    Producto p;
    int encontrado = 0; 
    fseek(inventario, 0, SEEK_SET); //Ponemos el cursor al inicio del primer registro del archivo
      
    while (encontrado == 0 && fread(&p, sizeof(Producto), 1, inventario) == 1) {//Entra si leyo un registro y si aun no encontro el producto
        
        if (p.codigo == codProd) {
            p.stockActual -= cantVendida;  //Si encontramos el producto le restamos al stock la cantidad vendida
            fseek(inventario, -(long)sizeof(Producto), SEEK_CUR); //Retrocedemos el tama√±o de un registro      
            fwrite(&p, sizeof(Producto), 1, inventario); //Escribimos lo que acabamos de modificar
            
            encontrado = 1; //Termina el while
        }
    }
}

int main (){
    FILE* f = fopen("comandas_historicas.dat", "rb");
    if (f == NULL) {
        cout << "No se pudo abrir el archivo." << endl;
        return 0;
    }

    // Declaramos un vec para cargar todo el archivo de comandas dado 
    ComandaHistorica v[500];
    // Variable para llevar la cuenta exacta de cuantas comandas leamos del archivo.
    int cantComandas = 0;
    ComandaHistorica c;

    // Leemos el archivo completo y lo guardamos en el vector v para ordenarlo despues
    while (fread(&c, sizeof(ComandaHistorica), 1, f) == 1 && cantComandas < 500) {
        v[cantComandas] = c;                             //  Almacenamos cada comanda leida en la posicion del vector
        cantComandas++;                                  // Incrementamos el contador de comandas almacenadas
    }
    fclose(f);

    // Vemos si el vector quedo vacio para evitar errores 
    if (cantComandas == 0) {
    	return 0;
    }

    //Aplicamos ordenamiento burbuja para ordenar el arreglo primero por fecha y luego por nombre de mozo
    for (int i = 0; i < cantComandas - 1; i++) {
        for (int j = 0; j < cantComandas - i - 1; j++) {
            int comp = strcmp(v[j].fecha, v[j+1].fecha); //  Comparamos las fechas de dos comandas consecutivas
            if (comp > 0 || (comp == 0 && strcmp(v[j].nombreMozo, v[j+1].nombreMozo) > 0)) { 
                ComandaHistorica aux = v[j];               //Guardamos el primer elemeto en un aux
				v[j] =  v[j+1];                          // Pasamos el elemento siguiente al lugar actual.
                v[j+1] = aux;                            // Asignamos el auxiliar a la siguiente posicion.
            }
        }
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
            
            // Creamos un texto temporal para guardar el ID numerico del mozo
            char passTemp[20];
            sprintf(passTemp, "%d", lista[cantidadMozos].idMozo); // Convertimos el ID entero a formato de texto para usarlo como clave inicial
            encriptarPassword(passTemp, lista[cantidadMozos].password); // Encriptamos esa contrasenia inicial y la guardamos en el mozo.

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
    
    // Abrimos el inventario en modo de lectura y escritura para actualizarlo
    FILE* inventario = fopen("inventario.dat", "rb+");
    if (inventario == NULL) {
        cout << "No se pudo abrir inventario.dat para actualizacion" << endl;
        return 0;
    }

    // Declaramos una variable para almacenar la fecha actual que se esta procesando en el corte de control
    char fechaActual[11] = "";
    
    FILE* fDia = NULL;

    // Recorremos nuevamente el vec ordenado para hacer el corte de control por fecha
    for (int i = 0; i < cantComandas; i++) {
        // Comprobamos si cambio la fecha respecto a la iteracion anterior.
        if (strcmp(v[i].fecha, fechaActual) != 0) {
            if (fDia != NULL) 
			{ 
			   fclose(fDia);
		     	}               //  Si ya habia un archivo diario abierto, lo cerramos antes de cambiar de dia
            strcpy(fechaActual, v[i].fecha);     //  Actualizamos la fecha actual con la nueva fecha encontrada
            
            char nombreArch[30];    // Variable temporal para construir el nombre del archivo diario
           strcpy(nombreArch, "comandas_"); //  Copiamos el texto que siempre es igual al inicio
           strcat(nombreArch, fechaActual); // Le pegamos la fecha al final de lo anterior
           strcat(nombreArch, ".dat");      // Le pegamos la extensiÛn al final de todo que tambien es siempre igual        
		    fDia = fopen(nombreArch, "wb");  //  Abrimos el nuevo archivo diario 
        }

        //  Buscamos el ID numerico que le corresponde al nombre del mozo actual
        int idMozoEncontrado = 0;
        for (int j = 0; j < cantidadMozos; j++) {
            if (strcmp(lista[j].nombre, v[i].nombreMozo) == 0) {
                idMozoEncontrado = lista[j].idMozo;       // Guardamos el ID numerico hallado
                break;
            }
        }

        // Creamos una comanda con la estructura reducida 
        Comanda comandaDiaria;
        comandaDiaria.idMozo = idMozoEncontrado;         //  Asignamos el ID del mozo.
        comandaDiaria.codigoProducto = v[i].codigoProducto; //  Copiamos el codigo del producto.
        comandaDiaria.cantidad = v[i].cantidad;         // : Copiamos la cantidad vendida
        comandaDiaria.comision = v[i].comision;         //  Copiamos la comision de la venta

        if (fDia != NULL) {
            fwrite(&comandaDiaria, sizeof(Comanda), 1, fDia); //  Escribimos la comanda reducida en el archivo del dia correspondiente
        }
        
        //  Llamamos a la funcion para descontar la cantidad vendida en el inventario
          actualizarStock(inventario, comandaDiaria.codigoProducto, comandaDiaria.cantidad);    }

    //  Cerramos el ultimo archivo diario que haya quedado abierto al finalizar el bucle
    if (fDia != NULL) fclose(fDia);
    // Cerramos el archivo que mantuvimos abierto durante todo el proceso
    if (inventario != NULL) {
    	fclose(inventario);
    }

    return 0;
}
