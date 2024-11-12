// Proyecto.cpp : Define el punto de entrada de la aplicación.
//

#include "framework.h"
#include "Proyecto.h"
#include <string>
#include <ctime>
#include <fstream>
#include <windows.h>
#include <commctrl.h>  // Para el DateTimePicker
#pragma comment(lib, "Comctl32.lib")  // Linkear la librería
#include "ArbolProductos.h"

#define MAX_LOADSTRING 100

using namespace std;

// Variables globales:
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

// Declaraciones de funciones adelantadas incluidas en este módulo de c0digo:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Registro(HWND, UINT, WPARAM, LPARAM);

// Estructura para el usuario
struct nodo_usuario {
    string nombre;
    string contra;
    tm fecha_nacimiento;
    double dinero;
    nodo_usuario* siguiente = nullptr;
};

// Puntero a la lista de usuarios
nodo_usuario* lista_usuarios = nullptr;
// Variable global para almacenar el usuario actual que esta autenticado
nodo_usuario* usuario_actual = nullptr;
void guardar_todos_los_usuarios();

// Función para convertir wchar_t a std::string
string convertir_wchar_a_string(const wchar_t* wstr) {
    int buffer_size = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
    string str(buffer_size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &str[0], buffer_size, nullptr, nullptr);
    return str;
}

// Función para verificar si un nombre de usuario ya existe
bool nombre_usuario_existe(const string& nombre) {
    nodo_usuario* actual = lista_usuarios;
    while (actual != nullptr) {
        if (actual->nombre == nombre) {
            return true; // El nombre de usuario ya existe
        }
        actual = actual->siguiente;
    }
    return false; // El nombre de usuario no existe
}

// Función para autenticar un usuario basado en nombre y contraseña
bool autenticar_usuario(const string& nombre, const string& contra) {
    nodo_usuario* actual = lista_usuarios;
    while (actual != nullptr) {
        if (actual->nombre == nombre && actual->contra == contra) {
            return true; // Autenticación exitosa
        }
        actual = actual->siguiente;
    }
    return false; // Usuario o contraseña incorrecta
}

// Recuperar usuarios con el archivo binario o crearlo si no existe
void recuperar_usuarios_bin() {
    ifstream archivo("usuarios.bin", ios::in | ios::binary);

    // Verificar si el archivo no existe
    if (!archivo.is_open()) {
        // Crear el archivo binario si no existe
        ofstream nuevo_archivo("usuarios.bin", ios::out | ios::binary);
        if (nuevo_archivo.is_open()) {
            MessageBox(NULL, L"El archivo 'usuarios.bin' no existia y se ha creado correctamente.", L"Archivo Creado", MB_OK | MB_ICONINFORMATION);
            nuevo_archivo.close();
        }
        else {
            MessageBox(NULL, L"Error al crear el archivo 'usuarios.bin'.", L"Error", MB_OK | MB_ICONERROR);
        }
        return; // Salir de la función ya que no hay usuarios que recuperar
    }

    // Limpiar lista actual de usuarios y leer desde el archivo binario
    while (archivo.peek() != EOF) {  // Comprobar si no es el final del archivo
        nodo_usuario* usuario_recuperado = new nodo_usuario;
        archivo.read(reinterpret_cast<char*>(usuario_recuperado), sizeof(nodo_usuario));

        if (!archivo) {  // Verificar si la lectura fue exitosa
            delete usuario_recuperado;  // Liberar memoria si la lectura falla
            break;
        }

        nodo_usuario* nuevo_usuario = new nodo_usuario(*usuario_recuperado);
        nuevo_usuario->siguiente = lista_usuarios;
        lista_usuarios = nuevo_usuario;
    }
    archivo.close();
}


// Función para registrar un nuevo usuario y guardarlo en archivo binario
void registrar_usuario(const string& nombre, const string& contra, const tm& fecha_nacimiento, double dinero_inicial) {
    // Verificar si el nombre de usuario ya existe
    if (nombre_usuario_existe(nombre)) {
        MessageBox(NULL, L"El nombre de usuario ya existe. Elija otro.", L"Error de registro", MB_OK | MB_ICONERROR);
        return;
    }

    nodo_usuario* nuevo_usuario = new nodo_usuario;
    nuevo_usuario->nombre = nombre;
    nuevo_usuario->contra = contra;
    nuevo_usuario->fecha_nacimiento = fecha_nacimiento;
    nuevo_usuario->dinero = dinero_inicial;  // Asignar el dinero inicial a la cuenta del usuario

    // Añadir el nuevo usuario a la lista
    nuevo_usuario->siguiente = lista_usuarios;
    lista_usuarios = nuevo_usuario;

    // Guardar en archivo binario
    ofstream archivo("usuarios.bin", ios::out | ios::binary | ios::app);
    if (archivo.is_open()) {
        archivo.write(reinterpret_cast<char*>(nuevo_usuario), sizeof(nodo_usuario));
        archivo.close();
        MessageBox(NULL, L"Usuario registrado con exito", L"Registro", MB_OK);
    }
    else {
        MessageBox(NULL, L"Error al guardar el usuario", L"Error", MB_OK | MB_ICONERROR);
    }
}

// Función para buscar un usuario por nombre y contraseña
nodo_usuario* buscar_usuario(const string& nombre, const string& contra) {
    nodo_usuario* actual = lista_usuarios;
    while (actual != nullptr) {
        if (actual->nombre == nombre && actual->contra == contra) {
            return actual;  // Devolver el usuario si la autenticación es exitosa
        }
        actual = actual->siguiente;
    }
    return nullptr;  // Usuario no encontrado o contraseña incorrecta
}


// Función para manejar el diálogo de registro de usuarios
INT_PTR CALLBACK Registro(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            // Obtener los datos del nombre de usuario y contraseña
            wchar_t nombre[100], contra[100], dinero[100];
            GetDlgItemText(hDlg, IDC_NOM, nombre, 100);
            GetDlgItemText(hDlg, IDC_CONTRA, contra, 100);
            GetDlgItemText(hDlg, IDC_DINERO, dinero, 100);  // Leer el valor del dinero desde el edit control

            // Validar que el valor de dinero sea numérico
            wstring dinero_wstr(dinero);
            bool es_numero_valido = true;
            for (wchar_t c : dinero_wstr) {
                if (!iswdigit(c) && c != L'.') {  // Permitir solo dígitos y un punto decimal
                    es_numero_valido = false;
                    break;
                }
            }

            if (!es_numero_valido) {
                MessageBox(hDlg, L"Por favor, ingrese un valor numérico valido para el dinero.", L"Error de entrada", MB_OK | MB_ICONERROR);
                return (INT_PTR)TRUE;  // Evitar el cierre del diálogo si la entrada es inválida
            }

            // Convertir el valor de dinero a un tipo double
            double dinero_inicial = _wtof(dinero);

            // Validar que el dinero sea mayor o igual a cero y esté dentro de un límite razonable
            if (dinero_inicial < 0 || dinero_inicial > 10000) {
                MessageBox(hDlg, L"Por favor, ingrese un valor de dinero entre 0 y 10,000.", L"Error de entrada", MB_OK | MB_ICONERROR);
                return (INT_PTR)TRUE;  // Evitar el cierre del diálogo si el valor está fuera de rango
            }

            // Obtener la fecha de nacimiento del DateTimePicker
            SYSTEMTIME fecha;
            tm fecha_nacimiento = {};
            DateTime_GetSystemtime(GetDlgItem(hDlg, IDC_DATETIMEPICKER1), &fecha);
            fecha_nacimiento.tm_mday = fecha.wDay;
            fecha_nacimiento.tm_mon = fecha.wMonth - 1;  // Enero es 0
            fecha_nacimiento.tm_year = fecha.wYear - 1900; // Contado desde 1900

            // Convertir los valores de wchar_t a std::string
            string nombre_str = convertir_wchar_a_string(nombre);
            string contra_str = convertir_wchar_a_string(contra);

            // Llamar a la función de registro con el nuevo dato de dinero
            registrar_usuario(nombre_str, contra_str, fecha_nacimiento, dinero_inicial);

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


// Función para manejar el diálogo de inicio de sesión de usuarios
INT_PTR CALLBACK Ingresar(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {  // Cuando se hace clic en el botón de aceptar
            wchar_t nombre[100], contra[100];
            GetDlgItemText(hDlg, IDC_NOM, nombre, 100);
            GetDlgItemText(hDlg, IDC_CONTRA, contra, 100);

            // Convertir los valores de wchar_t a std::string
            string nombre_str = convertir_wchar_a_string(nombre);
            string contra_str = convertir_wchar_a_string(contra);

            // Llamar a la función de autenticación y establecer el usuario actual si es exitoso
            nodo_usuario* usuario_encontrado = buscar_usuario(nombre_str, contra_str);
            if (usuario_encontrado != nullptr) {
                usuario_actual = usuario_encontrado;  // Establecer el usuario actual autenticado
                MessageBox(NULL, L"Inicio de sesi0n exitoso", L"Autenticacion", MB_OK);
                EndDialog(hDlg, LOWORD(wParam)); // Cierra el diálogo después del éxito
            }
            else {
                MessageBox(NULL, L"Nombre de usuario o contrasenia incorrectos", L"Error", MB_OK | MB_ICONERROR);
            }
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam)); // Cerrar el diálogo y volver al menú
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}



// Función para agregar dinero a la cuenta del usuario
void agregar_dinero_a_usuario(nodo_usuario* usuario, double cantidad) {
    if (cantidad <= 0) {
        MessageBox(NULL, L"La cantidad debe ser un numero positivo.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Actualizar el saldo del usuario
    usuario->dinero += cantidad;

    // Actualizar el archivo binario para reflejar el nuevo saldo
    guardar_todos_los_usuarios();
    MessageBox(NULL, L"Dinero agregado exitosamente a la cuenta.", L"Transaccion Exitosa", MB_OK | MB_ICONINFORMATION);
}

// Función para sobrescribir el archivo binario con la lista completa de usuarios
void guardar_todos_los_usuarios() {
    ofstream archivo("usuarios.bin", ios::out | ios::binary); // Modo de escritura estándar (sobrescribe todo)

    if (archivo.is_open()) {
        nodo_usuario* actual = lista_usuarios;
        while (actual != nullptr) {
            archivo.write(reinterpret_cast<char*>(actual), sizeof(nodo_usuario));
            actual = actual->siguiente;
        }
        archivo.close();
    }
    else {
        MessageBox(NULL, L"Error al guardar los datos del usuario.", L"Error", MB_OK | MB_ICONERROR);
    }
}


// Funcion para manejar el dialog de agregar dinero a la cuenta del usuario
INT_PTR CALLBACK AgregarDinero(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            wchar_t dinero[100];
            GetDlgItemText(hDlg, IDC_DINERO, dinero, 100);

            // Validar que el valor de dinero sea numérico
            double cantidad = _wtof(dinero);
            if (cantidad <= 0) {
                MessageBox(hDlg, L"Ingrese una cantidad valida de dinero mayor a 0.", L"Error de entrada", MB_OK | MB_ICONERROR);
                return (INT_PTR)TRUE;
            }

            // Llamar a la funcion para agregar dinero usando la variable global "usuario_actual"
            agregar_dinero_a_usuario(usuario_actual, cantidad);

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Inicializar cadenas globales
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROYECTO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Inicializar common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_DATE_CLASSES;
    InitCommonControlsEx(&icex);

    // Recuperar los usuarios desde el archivo binario
    recuperar_usuarios_bin();

    // Realizar la inicialización de la aplicación:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROYECTO));

    MSG msg;

    // Bucle principal de mensajes:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROYECTO));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PROYECTO);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Almacenar identificador de instancia en una variable global

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

// Función para manejar el diálogo de compra de productos
INT_PTR CALLBACK CompraProducto(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static double dineroUsuario = 500.0; // Asumimos un saldo inicial para pruebas
    static double precioProducto = 50.0; // Precio fijo para todos los productos por ahora

    switch (message) {
    case WM_INITDIALOG:
        // Poblar el ComboBox con productos estáticos
        SendDlgItemMessage(hDlg, IDC_PRODUCTO_LISTA, CB_ADDSTRING, 0, (LPARAM)L"Producto A");
        SendDlgItemMessage(hDlg, IDC_PRODUCTO_LISTA, CB_ADDSTRING, 0, (LPARAM)L"Producto B");
        SendDlgItemMessage(hDlg, IDC_PRODUCTO_LISTA, CB_ADDSTRING, 0, (LPARAM)L"Producto C");
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            wchar_t nombreProducto[100];
            GetDlgItemText(hDlg, IDC_PRODUCTO_LISTA, nombreProducto, 100);
            int cantidad = GetDlgItemInt(hDlg, IDC_CANTIDAD, NULL, FALSE);

            // Validaciones
            if (cantidad <= 0) {
                MessageBox(hDlg, L"Por favor, ingrese una cantidad válida.", L"Error", MB_OK | MB_ICONERROR);
                return (INT_PTR)TRUE;
            }

            // Calcular costo total
            double costoTotal = cantidad * precioProducto;

            // Verificar si el usuario tiene suficiente dinero
            if (costoTotal > dineroUsuario) {
                MessageBox(hDlg, L"No tienes suficiente dinero.", L"Saldo Insuficiente", MB_OK | MB_ICONERROR);
            }
            else {
                // Restar dinero del usuario
                dineroUsuario -= costoTotal;
                MessageBox(hDlg, L"Compra realizada con éxito.", L"Compra Exitosa", MB_OK | MB_ICONINFORMATION);
            }

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Analizar las selecciones de menú:
        switch (wmId)
        {//ID_COMPRA_COMPRAR
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case ID_USUARIO_REGISTRARSE:  // Evento para el menú de registro
            DialogBox(hInst, MAKEINTRESOURCE(IDD_REGISTRO), hWnd, Registro);
            break;
        case ID_USUARIO_INGRESAR:  // Evento para el menú de inicio de sesión
            DialogBox(hInst, MAKEINTRESOURCE(IDD_INGRESAR), hWnd, Ingresar);
            break;
        case ID_USUARIO_AGREGAR_DINERO:  // Evento para el menú de agregar dinero
            if (usuario_actual != nullptr) {  // Verificar que el usuario esté autenticado antes de agregar dinero
                DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_AGREGAR_DINERO), hWnd, AgregarDinero, reinterpret_cast<LPARAM>(usuario_actual));
            }
            else {
                MessageBox(hWnd, L"No hay ningun usuario autenticado. Inicie sesión para agregar dinero.", L"Error", MB_OK | MB_ICONERROR);
            }
            break;
        case ID_USUARIO_CERRAR_SESION:  // Evento para cerrar sesión
            if (usuario_actual != nullptr) {
                int respuesta = MessageBox(hWnd, L"¿Estas seguro de que deseas cerrar sesión?", L"Confirmacion", MB_YESNO | MB_ICONQUESTION);
                if (respuesta == IDYES) {
                    usuario_actual = nullptr;  // Restablecer el usuario actual
                    MessageBox(hWnd, L"Has cerrado sesión exitosamente.", L"Sesión Cerrada", MB_OK | MB_ICONINFORMATION);
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_INGRESAR), hWnd, Ingresar);  // Mostrar la pantalla de inicio de sesión nuevamente
                }
            }
            else {
                MessageBox(hWnd, L"No hay ningun usuario autenticado.", L"Error", MB_OK | MB_ICONERROR);
            }
            break;
        case ID_COMPRA_ABRIR:  // Evento para abrir la ventana de compra
            DialogBox(hInst, MAKEINTRESOURCE(IDD_COMPRA), hWnd, CompraProducto);
            break;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Controlador de mensajes del cuadro Acerca de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
