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
        if (i + 1 >= sizeEntrada) break; // <-- evita crash si tamaño impar
        unsigned char repeticiones = entrada[i];
        unsigned char valor = entrada[i + 1];
        for (int j = 0; j < repeticiones; j++) {
            salida[posicion++] = valor;
        }
    }

    return posicion;
}

int descomprimirLZ78(unsigned char* buffer, int tamanoBuffer, unsigned char* salida) {

    const int MAX = 1000;
    unsigned char* dic[MAX];
    int largo[MAX];
    int total = 0;

    int posSalida = 0;

    for (int i = 0; i < tamanoBuffer; i += 3) {
        if (i + 2 >= tamanoBuffer) break;

        unsigned short indice = (buffer[i] << 8) | buffer[i + 1];
        unsigned char letra = buffer[i + 2];

        if (indice == 0) {
            salida[posSalida++] = letra;

            dic[total] = new unsigned char[1];
            dic[total][0] = letra;
            largo[total] = 1;
            total++;
        } else {
            int pos = indice - 1;

            for (int j = 0; j < largo[pos]; j++) {
                salida[posSalida++] = dic[pos][j];
            }

            salida[posSalida++] = letra;

            int nuevoLargo = largo[pos] + 1;
            dic[total] = new unsigned char[nuevoLargo];
            for (int j = 0; j < largo[pos]; j++) {
                dic[total][j] = dic[pos][j];
            }
            dic[total][nuevoLargo - 1] = letra;
            largo[total] = nuevoLargo;
            total++;
        }
    }

    return posSalida;
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

bool contienePista(unsigned char* texto, int sizeTexto,
                   unsigned char* pista, int sizePista) {

    for (int i = 0; i <= sizeTexto - sizePista; i++) {
        int j = 0;
        while (j < sizePista and texto[i + j] == pista[j]) {
            j++;
        }
        if (j == sizePista) {

            return true; // pista igual
        }
    }
    return false;
}

int main() {

    const char* ruta = "debug/datasetDesarrollo/Encriptado3.txt";
    int size = contarTamano(ruta);

    if (size <= 0) return 1;

    unsigned char* buffer = new unsigned char[size];
    ifstream archivo(ruta, ios::binary);
    if (!archivo) {
        cout << "Error abriendo archivo" << endl;
        delete[] buffer;
        return 1;
    }
    archivo.read((char*)buffer, size);
    archivo.close();

    cout << "Archivo cargado, tamano: " << size << " bytes" << endl;

    // Aplicar XOR y rotación inversa

    unsigned char clave = 0x40;
    int n = 3;
    aplicarXOR(buffer, size, clave);
    aplicarRotacion(buffer, size, 8 - n);

    // Leer pista

    const char* rutaPista = "debug/datasetDesarrollo/pista3.txt";

    int sizePista = contarTamano(rutaPista);
    unsigned char* bufferPista = new unsigned char[sizePista];
    ifstream archPista(rutaPista, ios::binary);
    archPista.read((char*)bufferPista, sizePista);
    archPista.close();

    unsigned char* salida = new unsigned char[size * 50];


    // Intentar RLE

    int sizeSalida = descomprimirRLE(buffer, size, salida);
    if (contienePista(salida, sizeSalida, bufferPista, sizePista)) {
        cout << "Metodo correcto: RLE" << endl;
        for (int i = 0; i < 200 && i < sizeSalida; i++) cout << salida[i];
        cout << endl;
    } else {

        // Intentar LZ78

        int sizeSalida2 = descomprimirLZ78(buffer, size, salida);

        if (contienePista(salida, sizeSalida2, bufferPista, sizePista)) {
            cout << "Metodo correcto: LZ78" << endl;
            for (int i = 0; i < 200 && i < sizeSalida2; i++) cout << salida[i];
            cout << endl;
        } else {
            cout << "No se encontro la pista en ninguno de los dos metodos." << endl;
        }
    }

    delete[] buffer;
    delete[] salida;
    delete[] bufferPista;

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
