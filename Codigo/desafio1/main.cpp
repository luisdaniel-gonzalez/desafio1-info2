#include <iostream>

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

void descomprimirRLE()
{
    cout << "En proceso en bloc de nota" << endl;
}

int main()
{
    cout << "Desafio 1 - Informatica 2. Descomprimo y desencripto." << endl;
    cout << "Prueba de XOR y rotacion" << endl;

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

    aplicarRotacion(mensaje, size, 5);
    aplicarXOR(mensaje, size, 5);

    cout << "Desencriptado: ";
    for (int i = 0; i < size; i++) {
        cout << mensaje[i] << " ";
    }
    cout << endl;


    return 0;
}
