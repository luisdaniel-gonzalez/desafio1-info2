#include <iostream>
#include <fstream>

using namespace std;

void aplicarXOR(unsigned char* buffer, int size, unsigned char K) {
    for (int i = 0; i < size; i++) buffer[i] ^= K;
}

unsigned char rotarDerecha(unsigned char byte, int n) {
    for (int i = 0; i < n; i++) {
        unsigned char ultimo = byte % 2;
        byte /= 2;
        if (ultimo) {
            byte += 128;
        }
    }
    return byte;
}

void aplicarRotacion(unsigned char* buffer, int size, int n) {
    for (int i = 0; i < size; i++) buffer[i] = rotarDerecha(buffer[i], n);
}

int descomprimirRLE(unsigned char* entrada, int sizeEntrada, unsigned char* salida, int maxSize) {
    int pos = 0;
    for (int i = 0; i + 1 < sizeEntrada; i += 2) {
        int rep = entrada[i] - '0';
        unsigned char val = entrada[i + 1];

        if (pos + rep > maxSize) rep = maxSize - pos;

        for (int j = 0; j < rep; j++)
            salida[pos++] = val;
    }
    return pos;
}

int descomprimirLZ78(unsigned char* buffer, int sizeBuffer, unsigned char* salida) {

    const int MAX = 50000;
    unsigned char* dic[MAX];
    int largo[MAX];
    int total = 0;
    int posSalida = 0;

    for (int i = 0; i + 2 < sizeBuffer; i += 3) {
        int indice = (buffer[i] << 8) | buffer[i + 1];
        unsigned char letra = buffer[i + 2];

        if (indice == 0) {
            salida[posSalida++] = letra;
            dic[total] = new unsigned char[1];
            dic[total][0] = letra;
            largo[total] = 1;
            total++;
        } else {
            int pos = indice - 1;

            if (pos < 0 || pos >= total) {
                break;
            }

            for (int j = 0; j < largo[pos]; j++) salida[posSalida++] = dic[pos][j];
            salida[posSalida++] = letra;
            dic[total] = new unsigned char[largo[pos] + 1];
            for (int j = 0; j < largo[pos]; j++) dic[total][j] = dic[pos][j];
            dic[total][largo[pos]] = letra;
            largo[total] = largo[pos] + 1;
            total++;
        }
    }

    for (int i = 0; i < total; i++) delete[] dic[i];
    return posSalida;
}


bool contienePista(unsigned char* texto, int sizeTexto, unsigned char* pista, int sizePista) {

    for (int i = 0; i <= sizeTexto - sizePista; i++) {
        int j = 0;
        while (j < sizePista && texto[i + j] == pista[j]) j++;
        if (j == sizePista) {
            return true;
        }
    }
    return false;
}

int main() {

    int opcion;

    const char* ruta;
    const char* rutaPista;
    bool esRLE;

    cout << "Selecciona el encriptado que quieres ver:\n";
    cout << "1. Encriptado1.txt\n";
    cout << "2. Encriptado2.txt\n";
    cout << "3. Encriptado3.txt\n";
    cout << "4. Encriptado4.txt\n";
    cout << "Opcion: ";
    cin >> opcion;

    switch(opcion) {
    case 1:
        ruta = "debug/datasetDesarrollo/Encriptado1.txt";
        rutaPista = "debug/datasetDesarrollo/pista1.txt";
        esRLE = true;
        break;
    case 2:
        ruta = "debug/datasetDesarrollo/Encriptado2.txt";
        rutaPista = "debug/datasetDesarrollo/pista2.txt";
        esRLE = false;
        break;
    case 3:
        ruta = "debug/datasetDesarrollo/Encriptado3.txt";
        rutaPista = "debug/datasetDesarrollo/pista3.txt";
        esRLE = true;
        break;
    case 4:
        ruta = "debug/datasetDesarrollo/Encriptado4.txt";
        rutaPista = "debug/datasetDesarrollo/pista4.txt";
        esRLE = false;
        break;
    default:
        cout << "Opcion invalida" << endl;
        return 1;
    }

    ifstream archivo(ruta, ios::binary);
    if (!archivo) { cout << "No se pudo abrir el archivo"; return 1; }
    archivo.seekg(0, ios::end);
    int size = archivo.tellg();
    archivo.seekg(0, ios::beg);

    unsigned char* buffer = new unsigned char[size];
    archivo.read((char*)buffer, size);
    archivo.close();

    ifstream archPista(rutaPista, ios::binary);
    archPista.seekg(0, ios::end);
    int sizePista = archPista.tellg();
    archPista.seekg(0, ios::beg);
    unsigned char* bufferPista = new unsigned char[sizePista];
    archPista.read((char*)bufferPista, sizePista);
    archPista.close();

    unsigned char* salida = new unsigned char[size * 50];
    unsigned char* actual = new unsigned char[size];

    bool encontrado = false;
    int sizeSalida = 0;
    bool bandera = false; // false es RLE, true es LZ78
    int nEncontrada = 0;
    int claveEncontrada = 0;


    for (int n = 1; n < 8 && !encontrado; ++n) {
        for (int k = 0; k < 256 && !encontrado; ++k) {

            memcpy(actual, buffer, size);

            aplicarXOR(actual, size, (unsigned char)k);
            aplicarRotacion(actual, size, n);


            if (esRLE) {
                // intentar RLE primero, segun lo dado por el profe
                int sRLE = descomprimirRLE(actual, size, salida, size * 50);
                if (sRLE > 0 && contienePista(salida, sRLE, bufferPista, sizePista)) {
                    encontrado = true;
                    sizeSalida = sRLE;
                    bandera = false;
                    nEncontrada = n;
                    claveEncontrada = k;
                    break;
                }

                // si falla, intentar LZ78
                int sLZ = descomprimirLZ78(actual, size, salida);
                if (sLZ > 0 && contienePista(salida, sLZ, bufferPista, sizePista)) {
                    encontrado = true;
                    sizeSalida = sLZ;
                    bandera = true;
                    nEncontrada = n;
                    claveEncontrada = k;
                    break;
                }
            } else { // si se espera LZ78, probar LZ78 primero segun lo dado por el profe
                int sLZ = descomprimirLZ78(actual, size, salida);
                if (sLZ > 0 && contienePista(salida, sLZ, bufferPista, sizePista)) {
                    encontrado = true;
                    sizeSalida = sLZ;
                    bandera = true;
                    nEncontrada = n;
                    claveEncontrada = k;
                    break;
                }

                int sRLE = descomprimirRLE(actual, size, salida, size * 50);
                if (sRLE > 0 && contienePista(salida, sRLE, bufferPista, sizePista)) {
                    encontrado = true;
                    sizeSalida = sRLE;
                    bandera = false;
                    nEncontrada = n;
                    claveEncontrada = k;
                    break;
                }
            }
        }
    }

    cout << endl;

    if (encontrado) {

        if (bandera) {
            cout << "El metodo usado fue LZ78" << endl;
        } else {
            cout << "El metodo usado fue RLE" << endl;
        }
        cout << "Rotacion aplicada: " << nEncontrada << " a la derecha \n";
        cout << "La clave usada fue: 0x" << hex << claveEncontrada << dec << " (" << claveEncontrada << ")" << endl;

        cout << "\n=== DESENCRIPTADO ===" << endl;

        for (int i = 0; i < sizeSalida; i++) {
            cout << salida[i];
        }
        cout << "\n===============================\n" << endl;

        // Guardar resultado (enseñado en laboratorio)

        ofstream resultado("resultado.txt", ios::binary);
        resultado.write((char*)salida, sizeSalida);
        resultado.close();
        cout << "Se guardo el resultado en resultado.txt" << endl;
    } else {
        cout << "No se encontro la pista " << endl;
    }

    delete[] buffer;
    delete[] bufferPista;
    delete[] salida;
    delete[] actual;

    return 0;
}
