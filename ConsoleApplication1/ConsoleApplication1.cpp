// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <Windows.h>
#include <omp.h>
#include "timers.h"

#define N 100000000

void sequential_sum(double *, double *);
void openmp_partial_sums(double *, double *);
void openmp_shared_critical(double *, double *);
void openmp_shared_atomic(double *, double *);
void openmp_reduction(double *, double *);
void timing_wrapper(void (*func)(double *,double *), double *, const char *);

int main()
{
	double *a = (double *)malloc(N * sizeof(double));
	for (int i = 0; i < N; i++) {
		a[i] = 1;
	}
	
	timing_wrapper(sequential_sum, a, "Sequential");
	timing_wrapper(openmp_partial_sums, a, "Partial Sums");
	timing_wrapper(openmp_shared_critical, a, "Shared Critical");
	timing_wrapper(openmp_shared_atomic, a, "Shared Atomic");
	timing_wrapper(openmp_reduction, a, "Reduction");

	system("PAUSE");
    return 0;
}

void sequential_sum(double *arr, double *sum) {
	for (int i = 0; i < N; i++) {
		(*sum) += arr[i];
	}
}

void openmp_partial_sums(double *arr, double *sum) {
	#pragma omp parallel
	{
		double partial_sum = 0;

#pragma omp for
		for (int i = 0; i < N; i++) {
			partial_sum += arr[i];
		}

#pragma omp critical
		(*sum) += partial_sum;
	}
}

void openmp_shared_critical(double *arr, double *sum) {
#pragma omp parallel for shared(sum)
	for (int i = 0; i < N; i++) {
#pragma omp critical
		(*sum) += arr[i];
	}
}

void openmp_shared_atomic(double *arr, double *sum) {
#pragma omp parallel for shared(sum)
	for (int i = 0; i < N; i++) {
#pragma omp atomic
		(*sum) += arr[i];
	}
}

void openmp_reduction(double *arr, double *sum) {
	double temp_sum = 0;
#pragma omp parallel for reduction(+:temp_sum)
	for (int i = 0; i < N; i++) {
		temp_sum += arr[i];
	}
	(*sum) = temp_sum;
}

void timing_wrapper(void(*func)(double *,double *), double *arr, const char *desc) {
	double sum = 0;
	StartCounter();
	(*func)(arr, &sum);
	double timeLapse = GetCounter();
	printf("%s Result: %f\n", desc, sum);
	printf("Execution time: %f\n", timeLapse);
	Sleep(500);
}