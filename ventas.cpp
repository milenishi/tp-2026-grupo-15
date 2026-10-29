#include <iostream>
#include <cstdio>
using namespace std;

// Declaramos una variable global K fuera del main para usarla como corrimiento fijo en claves
const int K = 5;

struct Mozo {
    int idMozo;             // Identificador unico del mozo.
    char nombre[50];        // Nombre completo del mozo.
    char password[20];      // Contrasenia almacenada con el corrimiento definido por K.
    float totalComision;    // Comision acumulada desde que comenzo a trabajar.
};

//  Creamos esta estructura para manejar el inventario y poder actualizar el stock
struct Producto {
    int codigo;         //  Código único del producto
    char descripcion[50]; // Descripción del producto.
    float precio;       // Precio unitario del product.
    int stockActual;    // Stock disponible que iremos descontando
};

// Registro que se guarda en la planilla diaria, usando el ID del mozo en lugar de su nombre.
struct Comanda {
    int idMozo;             // Mozo que realizo la venta.
    int codigoProducto;     // Codigo del producto vendido.
    int cantidad;           // Unidades vendidas.
    float comision;         // Comision generada por esta venta.
};

struct NodoComanda {
    Comanda comanda;
    NodoComanda *siguiente;
};

// Aplica un corrimiento fijo a cada caracter de la contrasenia.
void encriptarContrasenia(const char origen[], char destino[]) {
    int i = 0; // Inicializamos el índice para recorrer el texto carácter por carácter.
    while (origen[i] != '\0') { //  Bucle que recorre la cadena hasta encontrar el carácter nulo de fin.
        destino[i] = origen[i] + K; //  Al carácter actual le sumamos K para encriptarlo
        i++; //  Avanzamos a la siguiente posición del texto.
    }
    destino[i] = '\0'; //  Colocamos el fin de cadena al final del texto ya encriptado.
}

bool cadenasIguales(const char a[], const char b[]) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return false;
        }
        i++;
    }
    return a[i] == b[i];
}

// Carga en memoria los mozos existentes en mozos.dat.
void cargarMozos(Mozo mozos[], int &cantidadMozos) { 
    FILE *f = fopen("mozos.dat", "rb"); //abre el archivo
    cantidadMozos = 0; //contador de mozos que se va a actualizar a medida que se carguen los mozos

    if (f == NULL) { //si no hay mozos no se puede continuar asi que termina el programa
        cout << "No existe mozos.dat. Es necesario generar mozos.dat antes de cargar ventas." << endl;
        return;
    }
	
  Mozo aux; //aux para no perder los datos fuera del while
    while (fread(&aux, sizeof(Mozo), 1, f) == 1 && cantidadMozos < 50) { //lectura del .dat y cuenta la cantidad de mozos
        mozos[cantidadMozos++] = aux;
    }
    fclose(f); //cierra mozos.dat
}

// Actualiza en mozos.dat unicamente el registro del mozo indicado.
bool actualizarMozo(const Mozo &mozo, int indice) {
    FILE *f = fopen("mozos.dat", "rb+");
    if (f == NULL) {
        cout << "No se pudo actualizar mozos.dat." << endl;
        return false;
    }

    bool guardado = fseek(f, (long)indice * (long)sizeof(Mozo), SEEK_SET) == 0 &&
                    fwrite(&mozo, sizeof(Mozo), 1, f) == 1;
    if (fclose(f) != 0) {
        guardado = false;
    }
    if (!guardado) {
        cout << "Error al escribir mozos.dat." << endl;
    }
    return guardado;
}

// Carga en memoria los productos y el stock actual de inventario.dat.
void cargarInventario(Producto inventario[], int &cantidadProductos) {
    FILE *f = fopen("inventario.dat", "rb"); //abre el archivo inventario.dat
    cantidadProductos = 0; //contador de productos

    if (f == NULL) { //se asegura de que exista invetario.dat sino finaliza el programa
        cout << "No existe inventario.dat. Debe generarse antes de registrar ventas." << endl;
        return;
    }

    Producto aux; // auxiliar para extraer los productos del while (para inventario.dat)
    while (fread(&aux, sizeof(Producto), 1, f) == 1 && cantidadProductos < 100) {
        inventario[cantidadProductos++] = aux;
    }
    fclose(f);
}

// Actualiza en inventario.dat unicamente el registro del producto indicado.
bool actualizarProducto(const Producto &producto, int indice) {
    FILE *f = fopen("inventario.dat", "rb+");
    if (f == NULL) {
        cout << "No se pudo guardar el inventario." << endl;
        return false;
    }

    bool guardado = fseek(f, (long)indice * (long)sizeof(Producto), SEEK_SET) == 0 &&
                    fwrite(&producto, sizeof(Producto), 1, f) == 1;
    if (fclose(f) != 0) {
        guardado = false;
    }
    if (!guardado) {
        cout << "Error al escribir inventario.dat." << endl;
    }
    return guardado;
}

// Encripta la clave ingresada y comprueba que coincida con la del mozo indicado.
bool loginMozo(Mozo mozos[], int cantidadMozos, int idIngresado, const char passIngresada[], Mozo &mozoLogueado) {
    char passEncriptada[20];
    encriptarContrasenia(passIngresada, passEncriptada);

    for (int i = 0; i < cantidadMozos; i++) {
        if (mozos[i].idMozo == idIngresado &&
            cadenasIguales(mozos[i].password, passEncriptada)) {
            mozoLogueado = mozos[i];
            return true;
        }
    }

    return false;
}

Producto *buscarProducto(Producto inventario[], int cantidadProductos, int codigo) {
    for (int i = 0; i < cantidadProductos; i++) {
        if (inventario[i].codigo == codigo) {
            return &inventario[i];
        }
    }
    return NULL;
}

// Busca un mozo por ID y devuelve su direccion, o NULL si no existe.
Mozo *buscarMozo(Mozo mozos[], int cantidadMozos, int idMozo) {
    for (int i = 0; i < cantidadMozos; i++) {
        if (mozos[i].idMozo == idMozo) {
            return &mozos[i];
        }
    }
    return NULL;
}

// Comprueba que la fecha exista en el calendario, incluidos los anios bisiestos.
bool esFechaValida(int dia, int mes, int anio) {
   if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || anio < 1) { //si no pasa los criterios minimos para ser una fecha valida se retorna
        return false;
    }

    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; //dias que tiene "cada mes" sin contar años biciestos
    bool anioBisiesto = anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0); //verifico si hay año biciesto

    if (anioBisiesto == true && mes == 2) { //si es año biciesto aumento el contador de febrero en 1
        diasPorMes[1] = diasPorMes[1] + 1;
    }

    if (dia > diasPorMes[mes - 1]) { //me aseguro que ningun mes supere los dias respectivos a la lista
        return false; //si no verifica retorna
    }
	
    return true; //verifica que la fecha es valida y retorna
}

// Agrega una comanda a la lista, manteniendola ordenada por ID de mozo.
bool insertarComandaOrdenada(NodoComanda *&lista, const Comanda &comanda) {
    NodoComanda *nuevo = new NodoComanda;
    nuevo->comanda = comanda;
    nuevo->siguiente = NULL;

    if (lista == NULL || lista->comanda.idMozo > comanda.idMozo) {
        nuevo->siguiente = lista;
        lista = nuevo;
        return true;
    }

    NodoComanda *actual = lista;
    while (actual->siguiente != NULL &&
           actual->siguiente->comanda.idMozo <= comanda.idMozo) {
        actual = actual->siguiente;
    }

    nuevo->siguiente = actual->siguiente;
    actual->siguiente = nuevo;
    return true;
}

void liberarListaComandas(NodoComanda *lista) {
    while (lista != NULL) {
        NodoComanda *borrar = lista;
        lista = lista->siguiente;
        delete borrar;
    }
}

// Lee las comandas existentes y las carga en una lista ordenada.
bool cargarPlanillaEnLista(const char nombreArchivo[], NodoComanda *&lista) {
    FILE *f = fopen(nombreArchivo, "rb");
    if (f == NULL) {
        return true;
    }

    Comanda comanda;
    bool correcto = true;
    while (fread(&comanda, sizeof(Comanda), 1, f) == 1) {
        if (!insertarComandaOrdenada(lista, comanda)) {
            correcto = false;
            break;
        }
    }

    if (ferror(f)) {
        cout << "Error al leer la planilla diaria." << endl;
        correcto = false;
    }
    if (fclose(f) != 0) {
        correcto = false;
    }
    return correcto;
}

// Guarda la lista completa; cada comanda queda en orden por ID de mozo.
bool guardarListaComandas(const char nombreArchivo[], const NodoComanda *lista) {
    FILE *f = fopen(nombreArchivo, "wb");
    if (f == NULL) {
        cout << "No se pudo guardar la planilla diaria." << endl;
        return false;
    }

    bool correcto = true;
    const NodoComanda *actual = lista;
    while (actual != NULL && correcto) {
        if (fwrite(&actual->comanda, sizeof(Comanda), 1, f) != 1) {
            correcto = false;
        }
        actual = actual->siguiente;
    }

    if (fclose(f) != 0) {
        correcto = false;
    }
    if (!correcto) {
        cout << "Error al escribir la planilla diaria." << endl;
    }
    return correcto;
}

// Agrega la nueva comanda a la lista del dia y guarda la planilla ordenada.
bool guardarComandaDiaria(const char fecha[], const Comanda &comanda) {
    char nombreArchivo[40];
    int i = 0;
    const char prefijo[] = "comandas_";
    while (prefijo[i] != '\0') {
        nombreArchivo[i] = prefijo[i];
        i++;
    }

    int j = 0;
    while (fecha[j] != '\0') {
        nombreArchivo[i] = fecha[j];
        i++;
        j++;
    }

    const char extension[] = ".dat";
    j = 0;
    while (extension[j] != '\0') {
        nombreArchivo[i] = extension[j];
        i++;
        j++;
    }
    nombreArchivo[i] = '\0';

    NodoComanda *lista = NULL;
    if (!cargarPlanillaEnLista(nombreArchivo, lista)) {
        liberarListaComandas(lista);
        return false;
    }
    if (!insertarComandaOrdenada(lista, comanda)) {
        liberarListaComandas(lista);
        return false;
    }

    bool guardado = guardarListaComandas(nombreArchivo, lista);
    liberarListaComandas(lista);
    return guardado;
}

int main() {
    int dia, mes, anio;
    char fecha[11];

	// Se solicita una sola fecha para todas las ventas de esta tanda.
	 cout <<"Ingrese la fecha."<<endl;
	 cout <<"Dia: ";
	 cin >> dia;
	 cout << "Mes: ";
	 cin >> mes;
	 cout << "Anio: ";
	 cin >> anio;

	// Se valida la fecha antes de usarla para nombrar la planilla.
     if (esFechaValida(dia, mes, anio) == false) {
        cout << "Fecha invalida. Verifique que el dia exista para ese mes y anio." << endl;
        return 0; //si no es valida la fecha termina el programa, sino continua
    }

    sprintf(fecha, "%02d-%02d-%04d", dia, mes, anio); // Arma la fecha con dia y mes de dos digitos.

	Mozo mozos[50]; // Arreglo con el maximo de mozos que se cargan en memoria.
    int cantidadMozos = 0; // Cantidad de registros leidos desde mozos.dat.
	cargarMozos(mozos, cantidadMozos); // Carga los mozos para validar las ventas.

	if (cantidadMozos == 0) { //verifico si hay mozos
        cout << "No hay mozos cargados. Primero debe generarse mozos.dat." << endl;
        return 0; //si no hay mozos termino el programa, sino continua
    }

    Producto inventario[100]; // Arreglo con el maximo de productos que se cargan en memoria.
    int cantidadProductos = 0; // Cantidad de registros leidos desde inventario.dat.
    cargarInventario(inventario, cantidadProductos); // Carga productos y stock para validar cada venta.

	 if (cantidadProductos == 0) { //verifico si hay productos
        cout << "No hay productos cargados. Primero debe generarse inventario.dat." << endl;
        return 0; //si no hay productos termino el programa, sino continua
    }

    bool continuar = true;

    while (continuar) {
        int opcion;
        cout << "\n--- Menu de ventas ---" << endl;
        cout << "1. Registrar venta" << endl;
        cout << "0. Finalizar" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                // Cada venta requiere identificar y autenticar al mozo que la registra.
                int idMozo;
                char pass[20];
                int codigoProducto;
                int cantidadVendida;

                cout << "Ingrese el ID del mozo: ";
                cin >> idMozo;
                Mozo *mozoRegistro = buscarMozo(mozos, cantidadMozos, idMozo);
                if (mozoRegistro == NULL) {
                    cout << "No existe un mozo con ese ID." << endl;
                    break;
                }

                cout << "Ingrese la password: ";
                cin >> pass;
                Mozo mozoActual;
                if (!loginMozo(mozos, cantidadMozos, idMozo, pass, mozoActual)) {
                    cout << "Credenciales invalidas." << endl;
                    break;
                }

                cout << "Bienvenido/a: " << mozoActual.nombre << endl;
                cout << "Ingrese codigo del producto: ";
                cin >> codigoProducto;

                Producto *producto = buscarProducto(inventario, cantidadProductos, codigoProducto);
                if (producto == NULL) {
                    cout << "Producto inexistente." << endl;
                    break;
                }

                cout << "Descripcion: " << producto->descripcion << endl;
                cout << "Precio unitario: $" << producto->precio << endl;
                cout << "Stock actual: " << producto->stockActual << endl;
                cout << "Cantidad a vender: ";
                cin >> cantidadVendida;

                if (cantidadVendida <= 0) {
                    cout << "La cantidad debe ser mayor a cero." << endl;
                    break;
                }
                if (cantidadVendida > producto->stockActual) {
                    cout << "No hay stock suficiente para esa venta." << endl;
                    break;
                }

                // La comision se calcula como el 10% del subtotal de la venta.
                float subtotal = producto->precio * cantidadVendida;
                float comision = subtotal * 0.10f;

                // Se prepara el registro diario con el ID numerico del mozo.
                Comanda comandaDiaria;
                comandaDiaria.idMozo = mozoActual.idMozo;
                comandaDiaria.codigoProducto = codigoProducto;
                comandaDiaria.cantidad = cantidadVendida;
                comandaDiaria.comision = comision;

                if (!guardarComandaDiaria(fecha, comandaDiaria)) {
                    break;
                }

                // Solo despues de guardar la comanda se actualizan stock y comision acumulada.
                producto->stockActual -= cantidadVendida;
                mozoRegistro->totalComision += comision;
                mozoActual.totalComision = mozoRegistro->totalComision;

                int indiceProducto = (int)(producto - inventario);
                int indiceMozo = (int)(mozoRegistro - mozos);
                bool inventarioGuardado = actualizarProducto(*producto, indiceProducto);
                bool mozosGuardados = actualizarMozo(*mozoRegistro, indiceMozo);
                if (!inventarioGuardado || !mozosGuardados) {
                    cout << "La comanda diaria se guardo, pero no se pudieron guardar todos los archivos auxiliares. Se finaliza la carga para evitar inconsistencias." << endl;
                    continuar = false;
                    break;
                }

                cout << "Venta registrada correctamente." << endl;
                cout << "Subtotal: $" << subtotal << endl;
                cout << "Comision del mozo: $" << comision << endl;
                cout << "Comision total acumulada del mozo: $" << mozoActual.totalComision << endl;
                break;
            }

            case 0:
                continuar = false;
                break;

            default:
                cout << "Opcion invalida." << endl;
                break;
        }
    }

    cout << "\nSe cerro la carga de ventas del dia " << fecha << "." << endl;

    return 0;
}
