#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <array>
#include <time.h>
#include <cstdlib>
#include <Windows.h>
#include <thread>
#include <conio.h>


using namespace std; //u¿y³em tego dla w³asnej wygody ale wiem, ¿e nie powinienem
int ilosc_watkow;

float randomFloat(float min, float max) {
	static default_random_engine e{};
	uniform_real_distribution<float> d(min, max);
	return d(e);
}

std::vector<float> randomVector(unsigned int size, float min, float max) {
	std::vector<float>vec;
	for (int i = 0; i < size; i++) {
		float e = randomFloat(min, max);
		vec.push_back(e);
	}
	return vec;
}

void print(const std::vector<std::vector<float>> matrix)
{
	for (int i = 0; i < matrix.size(); i++)
	{

		for (int j = 0; j < matrix[0].size(); j++)
		{
			std::cout << matrix[i][j] << ", ";
		}
		std::cout << std::endl;
	}
}

std::vector<std::vector<float>> matrix;
std::vector<std::vector<float>> createMatrix(std::array<int, 2> shape)
{

	std::vector<float> mat;
	//#pragma omp for
	for (int j = 0; j < shape[1]; j++)
	{
		mat.push_back(0);
	}
	for (int i = 0; i < shape[0]; i++)
	{
		matrix.push_back(mat);
	}
	return matrix;
}

std::vector<std::vector<float>> random1;
std::vector<std::vector<float>> random2;

std::vector<std::vector<float>> randomMatrix1(const std::array<int, 2> shape, float min, float max)
{

	for (int i = 0; i < shape[0]; i++)
	{
		std::vector<float> ran = randomVector(shape[1], min, max);
		random1.push_back(ran);
	}
	return random1;
}


std::vector<std::vector<float>> randomMatrix2(const std::array<int, 2> shape, float min, float max)
{

	for (int i = 0; i < shape[0]; i++)
	{
		std::vector<float> ran = randomVector(shape[1], min, max);
		random2.push_back(ran);
	}
	return random2;
}

std::vector<std::vector<float>> C;

void multiply_t(std::vector<std::vector<float>> &A, std::vector<std::vector<float>> &B, std::vector<std::vector<float>> &C, int nr_watk, int m, int p, int n)
{

	/*maceirze sa symetryczne najczesciej!*/
	/*jesli przyjmiemy ze macierze sa syumetryczne czyli w kazdej bedzie zawsze tyle samo elementow*/



	const int il_elementow = (m * p);
	const int il_operacji = il_elementow / ilosc_watkow;
	const int resz_operacji = il_elementow % ilosc_watkow;

	int start_op, end_op,z,y;
	// omp pharrel for schedulle, dynamic,static, guided
	//dynamiczny wlasny, musi byc sekcja krytyczna
	//sekcja krytyczna minimalna
	//synchornizacja watkow
	//semafor SO! zamiast tego l
	//mutex lock

	//15*4 + 8*5
		//100/23 = 4 r 8

	//mnozenie macierzy za pomoca omp
	//na poziomie watkow zaimplementowac omp
	//49 i 9 
	//for (z = resz_operacji; z != 0; z--) {
	//	y = ilosc_watkow / resz_operacji;
	//}

	

	if (nr_watk < il_elementow % ilosc_watkow) {
		start_op = il_operacji * nr_watk;
		end_op = (il_operacji * (nr_watk + 1)) + resz_operacji;
	}
	else {
		start_op = il_operacji * nr_watk + resz_operacji;
		end_op = (il_operacji * (nr_watk + 1)) + resz_operacji/*+1*/;
	}


	for (int op = start_op; op < end_op; op++) {
		const int row = op % m;
		const int col = op / m;

		float s = 0.0f;

		for (int i = 0; i < m; i++) {
			const float e1 = A[row][i];
			const float e2 = B[i][col];
			s += e1 * e2;

		}
		C[row][col] = s;
	}
}


std::vector<thread> threads;

int main() {
	clock_t start, stop;
	double czas;
	std::array<int, 2> wspolrzedne1;
	std::array<int, 2> wspolrzedne2;
	int m, n, p;

	int x = 6/4;
	cout << "eloelo " << x << endl;

	std::cout << "Podaj wymiar m: ";
	std::cin >> m;
	std::cout << "Podaj wymiar n (wspolny parametr): ";
	std::cin >> n;
	std::cout << "Podaj wymiar p: ";
	std::cin >> p;
	wspolrzedne1[0] = m;
	wspolrzedne1[1] = n;
	wspolrzedne2[0] = n;
	wspolrzedne2[1] = p;

	std::array<int, 2> dane;
	dane[0] = m;
	dane[1] = p;
	C = createMatrix(dane);

	float min_w = 0 , maks_w = 0;

	cout << "Podaj minimalna wartosc dla macierzy: " << endl;
	cin >> min_w;
	cout << "Podaj maksymalna wartosc dla macierzy: " << endl;
	cin >> maks_w;
	cout << "Ile watkow chcesz utworzyc?" << endl;
	cin >> ilosc_watkow;

	//start = clock();
	/*std::thread threads[2];*/
	cout << "1 macierz*************************************************************************************************" << endl;
	thread thread1(randomMatrix1, wspolrzedne1, min_w, maks_w);
	thread thread2(randomMatrix2, wspolrzedne2, min_w, maks_w);
	thread1.join();
	thread2.join();

	print(random1);
	cout << endl;

	cout << "2 macierz*************************************************************************************************" << endl;

	print(random2);
	cout << endl;

	start = clock();
	for (int i = 0; i < ilosc_watkow; i++) {

		//threads[i] = thread(multiply_t, ref(random1), ref(random2), ref(C), i, m, p, n);
		threads.push_back(thread(multiply_t, ref(random1), ref(random2), ref(C), i, m, p, n));
	}
	for (int i = 0; i < ilosc_watkow; ++i) {
		threads[i].join();
	}

	cout << "mnozenie macierzy*************************************************************************************************" << endl;
	print(C);
	cout << endl;
	stop = clock();
	czas = (double)(stop - start) / CLOCKS_PER_SEC;
	cout << "Obliczanie czasu: " << czas << endl;

	cin.ignore();
	return 0;

}