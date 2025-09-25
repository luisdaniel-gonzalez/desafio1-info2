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
    for (int i = 0; i < sizeEntrada; i += 2) {
        if (i + 1 >= sizeEntrada) break;
        int rep = entrada[i];           // convertir a int
        unsigned char val = entrada[i + 1];
        for (int j = 0; j < rep && pos < maxSize; j++)
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

            // <<< Aquí se pone la verificación
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
    const char* ruta = "debug/datasetDesarrollo/Encriptado2.txt";
    const char* rutaPista = "debug/datasetDesarrollo/pista2.txt";

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

    //unsigned char clave = 0x5A; // Para 1 y 2
    unsigned char clave = 0x40; // Para 3 y 4
    int n = 3;  // rotación inversa de 3

    aplicarXOR(buffer, size, clave);      // aplicar XOR
    aplicarRotacion(buffer, size, n); // invertir rotación

    unsigned char* salida = new unsigned char[size * 50];

    // Hago copias para que LZ no mueva lo que haga RLE

    unsigned char* bufferRLE = new unsigned char[size];
    unsigned char* bufferLZ78 = new unsigned char[size];
    memcpy(bufferRLE, buffer, size);
    memcpy(bufferLZ78, buffer, size);

    int sizeSalida = 0;

    cout << "Descomprimiendo RLE..." << endl;

    int sizeSalidaRLE = descomprimirRLE(bufferRLE, size, salida, size * 50);
    if (contienePista(salida, sizeSalidaRLE, bufferPista, sizePista)) {
        cout << "Metodo correcto: RLE" << endl;
        sizeSalida = sizeSalidaRLE;
    } else {
        cout << "Descomprimiendo LZ78..." << endl;
        int sizeSalidaLZ = descomprimirLZ78(bufferLZ78, size, salida);
        if (contienePista(salida, sizeSalidaLZ, bufferPista, sizePista)) {
            cout << "Metodo correcto: LZ78" << endl;
            sizeSalida = sizeSalidaLZ;
        } else {
            cout << "No se encontro la pista en ninguno de los metodos." << endl;
        }
    }

    // Guardar resultado (enseñado en laboratorio)

    ofstream resultado("resultado.txt", ios::binary);
    resultado.write((char*)salida, sizeSalida);
    resultado.close();
    cout << "Se guardo el resultado en resultado.txt" << endl;

    delete[] buffer;
    delete[] bufferPista;
    delete[] salida;

    return 0;
}

/*
int main () {

    unsigned char texto[] = "abcdefrrenosdesxyz";
    unsigned char pista[] = "rrenosdes";

    if (contienePista(texto, sizeof(texto)-1, pista, sizeof(pista)-1))
        cout << "Pista encontrada" << endl;
    else
        cout << "Pista NO encontrada" << endl;
}

int prueba() {

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
