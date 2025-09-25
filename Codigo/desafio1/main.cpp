#include <iostream>
#include <fstream>

using namespace std;


void aplicarXOR(unsigned char* buffer, int size, unsigned char K) // XOR
{
    for (int i = 0; i < size; i++) {
        buffer[i] = buffer[i] ^ K;
    }
}


unsigned char rotarDerecha(unsigned char byte, int n) // Rotacion derecha byte
{

    for (int i = 0; i < n; i++) {
        unsigned char ultimoBit = byte % 2;
        byte = byte / 2;
        if (ultimoBit == 1) {
            byte = byte + 128;
        }
    }
    return byte;
}

void aplicarRotacion(unsigned char* buffer, int size, int n) // Aplica rotación dereca a buffer
{
    for (int i = 0; i < size; i++) {
        buffer[i] = rotarDerecha(buffer[i], n);
    }
}

int descomprimirRLE(unsigned char* entrada, int sizeEntrada, unsigned char* salida) {
    int posicion = 0;
    for (int i = 0; i < sizeEntrada; i += 2) {
        unsigned char repeticiones = entrada[i];
        unsigned char valor = entrada[i + 1];
        for (int j = 0; j < repeticiones; j++) {
            salida[posicion] = valor;
            posicion++;
        }
    }

    return posicion;
}

int contarTamano(const char* nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        cout << "Error abriendo archivo: " << nombreArchivo << endl;
        return -1;
    }

    int contador = 0;
    char c;
    while (archivo.get(c)) {
        contador++;
    }

    archivo.close();
    return contador;
}


int main() {

    // Lectura de archivos enseñada en nuestro laboratorio

    const char* ruta = "debug/datasetDesarrollo/Encriptado3.txt";
    int size = contarTamano(ruta);
    if (size <= 0) {
        return 1;
    }

    unsigned char* buffer = new unsigned char[size];

    ifstream archivo(ruta, ios::binary);
    if (!archivo) {
        cout << "Error abriendo Encriptado3.txt" << endl;
        delete[] buffer;

        return 1;
    }

    archivo.read((char*)buffer, size);
    archivo.close();

    cout << "Archivo Encriptado3.txt cargado, Tamano: " << size << " bytes" << endl;

    unsigned char clave = 0x40;  // Clave de README del profe
    int n = 3;                   // rotacion de 3 bits

    aplicarXOR(buffer, size, clave);      // primero XOR
    aplicarRotacion(buffer, size, 8 - n); // luego rotacion inversa

    cout << "Primeros 50 caracteres despues de desencriptar:" << endl;
    for (int i = 0; i < 50 && i < size; i++) {
        cout << buffer[i];
    }
    cout << endl;

    delete[] buffer;

    return 0;

}

/*int prueba() {

    unsigned char mensaje[5] = { 'A', 'B', 'C', 'D', 'E' };
    int size = 5;

    cout << "Original: ";
    for (int i = 0; i < size; i++) {
        cout << mensaje[i] << " ";
    }
    cout << endl;

    aplicarXOR(mensaje, size, 5);
    aplicarRotacion(mensaje, size, 3);

    cout << "Encriptado: ";
    for (int i = 0; i < size; i++) {
        cout << (int)mensaje[i] << " ";
    } cout << endl;

    aplicarRotacion(mensaje, size, 5); // Rotar a la izquierda 3 veces, es lo mismo que rotar a la derecha 5, reutilizo la funcion
    aplicarXOR(mensaje, size, 5);

    cout << "Desencriptado: ";
    for (int i = 0; i < size; i++) {
        cout << mensaje[i] << " ";
    }
    cout << endl;


    return 0;

} */
