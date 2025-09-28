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
        if (ultimo) byte += 128;
    }
    return byte;
}

void aplicarRotacion(unsigned char* buffer, int size, int n) {
    for (int i = 0; i < size; i++) buffer[i] = rotarDerecha(buffer[i], n);
}

int descomprimirRLE(unsigned char* entrada, int sizeEntrada, unsigned char* salida, int maxSize) {
    int pos = 0;
    for (int i = 0; i + 1 < sizeEntrada; i += 2) {
        unsigned char val = entrada[i];
        int rep = entrada[i + 1];

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
                cout << "Error: indice fuera de rango LZ78 en i=" << i << endl;
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


bool contienePista(unsigned char* texto, int sizeTexto,
                   unsigned char* pista, int sizePista) {
    for (int i = 0; i <= sizeTexto - sizePista; i++) {
        int j = 0;
        while (j < sizePista && texto[i + j] == pista[j]) j++;
        if (j == sizePista) return true;
    }
    return false;
}

int main() {

    int opcion;

    const char* ruta;
    unsigned char clave;
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
        ruta = "debug/datasetDesarrollo/encriptado.txt";
        clave = 0x5A;
        rutaPista = "debug/datasetDesarrollo/pista1.txt";
        esRLE = true;
        break;
    case 2:
        ruta = "debug/datasetDesarrollo/Encriptado2.txt";
        clave = 0x5A;
        rutaPista = "debug/datasetDesarrollo/pista2.txt";
        esRLE = false;
        break;
    case 3:
        ruta = "debug/datasetDesarrollo/Encriptado3.txt";
        clave = 0x40;
        rutaPista = "debug/datasetDesarrollo/pista3.txt";
        esRLE = true;
        break;
    case 4:
        ruta = "debug/datasetDesarrollo/Encriptado4.txt";
        clave = 0X5a;
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
    int prueba = descomprimirRLE(buffer, size, salida, size * 50);

    for (int i = 0; i < prueba; i++) {
        cout << salida[i];
    }
}

    /*int n = 3;  // rotación inversa de 3

    aplicarXOR(buffer, size, clave);      // aplicar XOR
    aplicarRotacion(buffer, size, n); // invertir rotación

    //unsigned char* salida = new unsigned char[size * 50];

    // Hago copias para que LZ no mueva lo que haga RLE

    unsigned char* bufferRLE = new unsigned char[size];
    unsigned char* bufferLZ78 = new unsigned char[size];
    memcpy(bufferRLE, buffer, size);
    memcpy(bufferLZ78, buffer, size);

    int sizeSalida = 0;
    bool bandera = false; // false es RLE, true es LZ78

    if (esRLE) {
        int sizeSalidaRLE = descomprimirRLE(bufferRLE, size, salida, size * 50);
        if (contienePista(salida, sizeSalidaRLE, bufferPista, sizePista)) {
        bandera = false;
        sizeSalida = sizeSalidaRLE;
        } else {
            cout << "No se encontro la pista con RLE." << endl;
        }
    } else {
        int sizeSalidaLZ = descomprimirLZ78(bufferLZ78, size, salida);
        if (contienePista(salida, sizeSalidaLZ, bufferPista, sizePista)) {
            sizeSalida = sizeSalidaLZ;
            bandera = true;
        } else {
            cout << "No se encontro la pista con LZ78." << endl;
        }
    }

    cout << endl;
    if (bandera) {
        cout << "El metodo usado fue LZ78" << endl;
    } else {
        cout << "El metodo usado fue RLE" << endl;
    }

    cout << "La clave usada fue: 0x" << hex << (int)clave << endl;
    cout << "Rotacion aplicada: 3 a la derecha" << endl;
    // Guardar resultado (enseñado en laboratorio)

    cout << "\n=== DESENCRIPTADO ===" << endl;
    for (int i = 0; i < sizeSalida; i++) {
        cout << salida[i];
    }
    cout << "\n===============================\n" << endl;


    ofstream resultado("resultado.txt", ios::binary);
    resultado.write((char*)salida, sizeSalida);
    resultado.close();
    cout << "Se guardo el resultado en resultado.txt" << endl;

    delete[] buffer;
    delete[] bufferPista;
    delete[] salida;

    return 0;
}
}
*/
