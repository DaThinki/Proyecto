// ArbolProductos.h
#pragma once

#include <string>

using namespace std;

// Estructura del nodo para el producto en el arbol binario
struct NodoProducto {
    string nombre;  
    double precioBase; 
    int cantidadDisponible;  
    int cantidadInicial; 
    NodoProducto* ant;  
    NodoProducto* sig; 

    // El CONSTRUCTOR para inicializar el nodo del producto
    NodoProducto(string nombre, double precioBase, int cantidadInicial);
};

//  El arbol Binario de Productos
class ArbolProductos {
private:
    NodoProducto* raiz;  // Nodo raiz

    // Función recursiva para insertar un producto en el arbol
    NodoProducto* insertarProductoRec(NodoProducto* nodo, const string& nombre, double precioBase, int cantidadInicial);

    // Función recursiva para buscar un producto en el arbol
    NodoProducto* buscarProductoRec(NodoProducto* nodo, const string& nombre);

    // Función recursiva para reabastecer productos
    void reabastecerProductos(NodoProducto* nodo);

public:
    // Constructor para inicializar el arbolito
    ArbolProductos();

    // Función para insertar un nuevo producto en el arbol
    void insertarProducto(const string& nombre, double precioBase, int cantidadInicial);

    // Función para buscar un producto en el arbol
    NodoProducto* buscarProducto(const string& nombre);

    // Función para iniciar el reabastecimiento de todos los productos desde la raiz
    void iniciarReabastecimiento();
};
