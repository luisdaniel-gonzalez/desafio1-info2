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

int main()
{
    cout << "Desafio 1 - Informatica 2. Descomprimo y desencripto." << endl;
    return 0;
}
