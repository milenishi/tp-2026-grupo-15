#include <iostream>
#include <cstdio> 
#include <cstring>

using namespace std;

struct Mozo { //de mozos.dat
    int idMozo; 
    char nombre[50];
    char password[20];
    float totalComision; 
};

struct Producto {//de inventario.dat 
int codigo;
char descripcion[50]; 
float precio; 
int stockActual;
};


int main (){
	int dia, mes, anio;

	 
	 cout <<"Ingrese la fecha."<<endl;
	 cout <<"Dia: ";
	 cin >> dia;
	 cout << "Mes: ";
	 cin >> mes;
	 cout << "Anio: ";
	 cin >> anio;

	return 0;
}
