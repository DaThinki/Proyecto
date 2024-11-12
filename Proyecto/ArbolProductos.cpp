// ArbolProductos.cpp
#include "ArbolProductos.h"

// Constructor del nodo del producto
NodoProducto::NodoProducto(string nombre, double precioBase, int cantidadInicial)
    : nombre(nombre), precioBase(precioBase), cantidadDisponible(cantidadInicial), cantidadInicial(cantidadInicial),
    ant(nullptr), sig(nullptr) {}

// Constructor para inicializar el árbol binario
ArbolProductos::ArbolProductos() : raiz(nullptr) {}

// Función recursiva para insertar un producto en el arbol
NodoProducto* ArbolProductos::insertarProductoRec(NodoProducto* nodo, const string& nombre, double precioBase, int cantidadInicial) {
    if (nodo == nullptr) {
        return new NodoProducto(nombre, precioBase, cantidadInicial);
    }

    if (nombre < nodo->nombre) {
        nodo->ant = insertarProductoRec(nodo->ant, nombre, precioBase, cantidadInicial);
    }
    else if (nombre > nodo->nombre) {
        nodo->sig = insertarProductoRec(nodo->sig, nombre, precioBase, cantidadInicial);
    }
    return nodo;
}

// Función recursiva para buscar un producto en el arbol
NodoProducto* ArbolProductos::buscarProductoRec(NodoProducto* nodo, const string& nombre) {
    if (nodo == nullptr || nodo->nombre == nombre) {
        return nodo;
    }

    if (nombre < nodo->nombre) {
        return buscarProductoRec(nodo->ant, nombre);
    }
    else {
        return buscarProductoRec(nodo->sig, nombre);
    }
}

// Funcion para insertar un nuevo producto
void ArbolProductos::insertarProducto(const string& nombre, double precioBase, int cantidadInicial) {
    raiz = insertarProductoRec(raiz, nombre, precioBase, cantidadInicial);
}

// Funcion para buscar productos
NodoProducto* ArbolProductos::buscarProducto(const string& nombre) {
    return buscarProductoRec(raiz, nombre);
}

// Funcion recursiva para reabastecer todos los productos a su cantidad inicial
void ArbolProductos::reabastecerProductos(NodoProducto* nodo) {
    if (nodo == nullptr) {
        return;
    }
    nodo->cantidadDisponible = nodo->cantidadInicial;
    reabastecerProductos(nodo->ant);
    reabastecerProductos(nodo->sig);
}

// Funcion para iniciar el reabastecimiento de todos los productos desde la raiz
void ArbolProductos::iniciarReabastecimiento() {
    reabastecerProductos(raiz);
}
