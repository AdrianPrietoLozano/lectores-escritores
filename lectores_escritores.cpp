#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <mutex> // para el semáforo
//#include <Windows.h>
#include <chrono>
#include <cstdlib> // contiene el prototipo de las funciones srand y rand
using namespace std;


// semaforos
mutex x, y, z, sescr, slect, prin;

const int NUM_CICLOS = 5;
const int NUM_LECTORES = 5;
const int NUM_ESCRITORES = 1;

// generar numeros aleatorios
random_device rd;
mt19937 mt(rd());
uniform_int_distribution<uint32_t> uint_dist10(1, 5);

// dato compartido
vector<int> datoCompartido = {1, 2, 3, 4, 5};

class Lector
{

	public:
		Lector();
		void ejecutar();
		void leerDato();
		int id;
		static int cuenta;
		static int total;
};

int Lector::cuenta = 0;
int Lector::total = 0;

Lector::Lector()
{
	id = Lector::total;
	Lector::total++;
}

void Lector::leerDato()
{
	prin.lock();
	cout << "Lector: " << id << ", comenzo a leer" << endl;
	prin.unlock();

	int num = 0;
	for(int i = 0; i < datoCompartido.size(); i++)
		num = datoCompartido.at(i);

    int segundos = uint_dist10(mt);
    std::this_thread::sleep_for(std::chrono::seconds(segundos));
    prin.lock();
	cout << "Lector: " << id << ", termino de leer" << endl;
	prin.unlock();
    
}

void Lector::ejecutar()
{
	for(int i = 0; i < NUM_CICLOS; i++)
	{
		prin.lock();
		cout << "Lector: " << id << ", solicito el recurso" << endl;
		prin.unlock();

		z.lock();
		slect.lock();
		x.lock();
		Lector::cuenta++;
		if(Lector::cuenta == 1)
			sescr.lock();
		x.unlock();
		slect.unlock();
		z.unlock();
		leerDato();
		x.lock();
		Lector::cuenta--;
		if(Lector::cuenta == 0)
			sescr.unlock();
		x.unlock();

		// espera un tiempo antes de volver a soliciar el recurso
		int segundos = uint_dist10(mt);
    	std::this_thread::sleep_for(std::chrono::seconds(segundos));
	}
}



class Escritor
{
	public:
		Escritor();
		void escribirDato();
		void ejecutar();
		static int cuenta;
};

int Escritor::cuenta = 0;

Escritor::Escritor()
{
	
}

void Escritor::escribirDato()
{
    prin.lock();
	cout << endl << "Escritor comenzo a escribir" << endl;
    prin.unlock();

	for(int i = 0; i < datoCompartido.size(); i++)
		datoCompartido[i] = datoCompartido[i] * 2;

    int segundos = uint_dist10(mt);
    std::this_thread::sleep_for(std::chrono::seconds(segundos));
    
    prin.lock();
    cout << "Escritor termino de escribir" << endl << endl;
    prin.unlock();

}

void Escritor::ejecutar()
{
	for(int i = 0; i < NUM_CICLOS; i++)
	{
		prin.lock();
		cout << "Escritor solicito el recurso" << endl;
		prin.unlock();

		y.lock();
		Escritor::cuenta++;
		if(Escritor::cuenta == 1)
			slect.lock();
		y.unlock();
		sescr.lock();
		escribirDato();
		sescr.unlock();
		y.lock();
		Escritor::cuenta--;
		if(Escritor::cuenta == 0)
			slect.unlock();
		y.unlock();

		// espera un tiempo antes de volver a soliciar el recurso
		int segundos = uint_dist10(mt);
    	std::this_thread::sleep_for(std::chrono::seconds(segundos));
	}
}


int main()
{

	srand( static_cast<unsigned int>( time( 0 ) ) ); // para producir numero aleatorios sin errores

	vector<thread> threads;
    for(int i = 0; i < NUM_LECTORES; i++)
        threads.push_back(thread(&Lector::ejecutar, new Lector()));

    for(int i = 0; i < NUM_ESCRITORES; i++)
        threads.push_back(thread(&Escritor::ejecutar, new Escritor()));


    for(auto& thread: threads)
    {
        thread.join();
    }

	return 0;
}