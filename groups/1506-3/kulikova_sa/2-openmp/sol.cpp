#include <omp.h>
#include "Sole.h"

int size = 0;

double vec(double * x, double * y) {
	double res = 0;
#pragma omp parallel for reduction(+:res)
		for (int i = 0; i < size; i++)
			res += x[i] * y[i];
	return res;
}

void NonlinearConjugateGradient(Sole * S) {
	size = S->N;// Задание размера

	// Выделение памяти
	double * x = new double[size];// Вектор приближений
	double * d = new double[size];// Вектор направления
	double * g = new double[size];// Градиент
	double s;// Скалярный шаг, величина смещения по заданному направлению
	double coef;// Коэффициент во 2 шаге
	double * s_prev = new double[size];// Произведение вектора направления и матрицы
	double * x_prev = new double[size];// Предыдущее приближение
	double * d_prev = new double[size];// Предыдущий вектор направления
	double * g_prev = new double[size];// Предыдущий градиент
 
	// Перед выполнением первой итерации x0 и d0 полагаются равными нулю
	// Для вектора g0 устанавливается значение -b
#pragma omp parallel for
	for (int i = 0; i < size; i++) {
		x_prev[i] = 0;
		d_prev[i] = 0;
		g_prev[i] = S->b[i] * (-1);
	}

	// Цикл по количеству итераций
	for (int i = 0; i < size; i++) {
		// 1. Вычисление градиента
#pragma omp parallel for
		for (int j = 0; j < size; j++)
			g[j] = vec(S->A[j], x_prev) - S->b[j];

		// 2. Вычисление вектора направления
		coef = vec(g, g) / vec(g_prev, g_prev);
		if (isnan(coef)) coef = 0;
#pragma omp parallel for
		for (int j = 0; j < size; j++)
			d[j] = coef * d_prev[j] - g[j];

		// 3. Вычисление величины смещения по заданному направлению
#pragma omp parallel for
		for (int j = 0; j < size; j++)
			s_prev[j] = vec(S->A[j], d);
		s = vec(d, g) / vec(s_prev, d);
		if (isnan(s)) s = 0;

		// 4. Вычисление нового приближения
#pragma omp parallel for
		for (int j = 0; j < size; j++)
			x[j] = x_prev[j] - s * d[j];

		// Замена текущих на предыдущие
#pragma omp parallel for
		for (int j = 0; j < size; j++) {
			g_prev[j] = g[j];
			d_prev[j] = d[j];
			x_prev[j] = x[j];
		}
	}

	// Сохранение результата
#pragma omp parallel for
	for (int i = 0; i < size; i++)
		S->x[i] = x[i];

	// Освобождение памяти
	delete[] x;
	delete[] d;
	delete[] g;
	delete[] s_prev;
	delete[] x_prev;
	delete[] d_prev;
	delete[] g_prev;
}
