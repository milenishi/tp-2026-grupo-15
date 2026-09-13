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
 char contraseña[50];// en este guardamos la clave
 float totalComision; 
 };
 
 struct Comanda{
int idMozo; 
int codProd;
int cantidad;// cant vendida por producto
float comision; //comision x venta
};
 
struct Producto{//para manejar el inventario y poder actualizar el stock
int codigo; //cod del producto
char descripcion[50];
float precio;//precio unitario del prod
int stockActual;//stock disponible q va bajando
};

int k=5; // variable global (afuera del main), usamos para ir corriendo las claevs de los mozos


//funcion para generarle la contraseña a cada mozo, sumandole k a cada uno
void contraseña (char origen[], char destino []){//char origen recibe la palabra q queremos encriptar, destinodonde queda el texto odificado
	int i=0;
	while(origen[i]!= 0){//mientras q ek texto no sea nulo
	destino[i]=origen[i]+k;//toma el nro que esta en l apos y le suma el k
	i++;}
	destino[i]=0;// le ponemos el 0 a mano para que sea el fin
}
//funcion para actualizar stock
void actualizarStock(int codProd, int cantVendida, Producto inv[],int cantProductos){
		for(int i=0; i<cantProd; i++){// recorremos
		if (inv[i].codigo==codProd){//buscamos a traves del codigo el prod vendido
		inv[i].stockActual -=cantVendida;//restamos
		break;
			}
		}
	}
	


int main (){
	
	FILE* f= fopen("comandas_historicas.dat", "rb");
  if (f == NULL) {
     cout << "No se pudo abrir el archivo." << endl;
 return 0;
}

ComandaHistorica v[500];//cargamos en el vec todo el archivo y asi lo podemos ordenar
int cantComandas = 0;//para llevar en cuantas aomandas leemos
ComandaHistorica c;
while (fread(&v[cantComandas], sizeof(ComandaHistorica), 1, f) == 1 && cantComandas < 500) {
    cantComandas++;
}

//el vec quedo vacio?
if (cantComandas == 0) {
        cout << "El archivo histórico está vacío." << endl;
        return 0;
    }
    //ordenamiento burbuja, ordenamos el primero por fecha u luego por nombre de mozo
    for (int i = 0; i < cantComandas - 1; i++) {
        for (int j = 0; j < cantComandas - i - 1; j++) {
            int comp = strcmp(v[j].fecha, v[j+1].fecha);//comparando las fehcas de dos comandas 
            if (comp > 0 || (comp == 0 && strcmp(v[j].nombreMozo, v[j+1].nombreMozo) > 0)) {
                ComandaHistorica aux = v[j];//usamos el aux para realizar el cambio
                v[j] = v[j+1];//pasamos al sig
                v[j+1] = aux;//el aux a la sig pos
            }
        }
    }
    
    Producto inv[100];// vec para cargar el inventario del archivo y asi lo modificamo smas rapido
    int cantProductos = 0;//cuanros prod hay cargados 
    FILE* finv = fopen("inventario.dat", "rb");
    if (finv != NULL) {
        while (fread(&inv[cantProductos], sizeof(Producto), 1, finv) == 1 && cantProductos < 100) {//leemos y guarda el archivo en el vec 
            cantProductos++;// q se guarde ene l sig registro
        }
        fclose(finv);
    }

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
            
            char contraTemp[20];
            sprintf(contraTemp, "%d", lista[cantidadMozos].idMozo);//convierte un nro en formatotexto y lo guardo en el contra temp
            contraseña(contraTemp, lista[cantidadMozos].contraseña);
            
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
    
    char fechaActual[11]= ""; //variable para almacenar la fecha actual 
    FILE* fDia=NULL; 
    
    for(int i =0; i<cantComandas; i++){//recorremos nuevamente el arreglo ordenado para hacer el corte de control por fecha
	
	//comprobamos si cambió la fecha respecto a la iteración anterior
	if(strcmp(v[i].fecha, fechaActual)!=0){//si ya había un archivo diario abierto, lo cerramos antes de cambiar de día
	if(fDia!=NULL)fclose(fDia);//si ya había un archivo diario abierto, lo cerramos antes de cambiar de día
	strcpy(fechaActual, v[i].fecha);//actualizamos la fecha actual con la fecha encontrada
	
	char nombreArch[30];//variable temporal para construir el nombre del archivo diario
	sprintf(nombreArch, "comandas_%s.dat", fechaActual);//armamos el nombre exacto con el formato comandas_dd-mm-aaaa.data
	fDia = fopen(nombreArch, "wb");//abrimos el nuevo archivo diario en modo escritura binaria
		}
	}
	// Buscar el ID numérico correspondiente al mozo actual
        int idMozoEncontrado = 0;
        for (int j = 0; j < cantidadMozos; j++) {
            if (strcmp(lista[j].nombre, v[i].nombreMozo) == 0) {
                idMozoEncontrado = lista[j].idMozo;//abrimos el nuevo archivo diario en modo escritura binaria
                break;
            }
        }

        // Armar la comanda diaria reducida (con ID en vez de nombre)
        Comanda comandaDiaria;
        comandaDiaria.idMozo = idMozoEncontrado;// asignamos id al mozo
        comandaDiaria.codigoProducto = v[i].codProd;//copiamos el cod del prod
        comandaDiaria.cantidad = v[i].cantidad;//copiamos la cant vendida
        comandaDiaria.comision = v[i].comision;//copiamos la comision de la venta

        if (fDia != NULL) {
            fwrite(&comandaDiaria, sizeof(Comanda), 1, fDia);//escrinimos la comanda reducida en el archivo del dia correspondiente
        }
        
        // Descontar la cantidad vendida en el inventario cargado en el vec
actualizarStock(comandaDiaria.codProd, comandaDiaria.cantidad, inv, cantProductos);
    if (fDia != NULL) fclose(fDia);// cerramos el archivo diario que haya quedado abierto


    finv = fopen("inventario.dat", "wb");// abrimos el arch en modo escritura
    if (finv != NULL) {
        fwrite(inv, sizeof(Producto), cantProductos, finv);// toma el vec y escribimos el contenido ene l arch
        fclose(finv);
    }

    cout << "Normalizacion completada con exito." << endl;
	return 0;
}

