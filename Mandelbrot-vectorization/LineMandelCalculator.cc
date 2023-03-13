/**
 * @file LineMandelCalculator.cc
 * @author MATEJ OTCENAS <xotcen01@stud.fit.vutbr.cz>
 * @brief Implementation of Mandelbrot calculator that uses SIMD paralelization over lines
 * @date DATE 11.11.2021
 */
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <immintrin.h>

#include <stdlib.h>


#include "LineMandelCalculator.h"


LineMandelCalculator::LineMandelCalculator (unsigned matrixBaseSize, unsigned limit) :
	BaseMandelCalculator(matrixBaseSize, limit, "LineMandelCalculator")
{
	data = (int *)_mm_malloc(height * width * sizeof(int), 64);
	zImag = (float *)_mm_malloc(height * width * sizeof(float), 64);
	zReal = (float *)_mm_malloc(height * width * sizeof(float), 64);
	real = (float *)_mm_malloc(height * width * sizeof(float), 64);
	imag = (float *)_mm_malloc(height * width * sizeof(float), 64);

	// matrix values initialization
	#pragma omp simd simdlen(64) collapse(2)
	for(int i = 0; i < height; i++){
		
		for(int j = 0; j < width; j++) {
			int idx = i * width + j;

			real[idx] = x_start + j * dx;
			imag[idx] = y_start + i * dy;
			
			data[idx] = limit;
			zReal[idx] = real[idx];
			zImag[idx] = imag[idx];
		}
	}
}

LineMandelCalculator::~LineMandelCalculator() {
	_mm_free(data);
	_mm_free(zReal);
	_mm_free(zImag);
	_mm_free(real);
	_mm_free(imag);

	data = NULL;
	real = NULL;
	imag = NULL;
	zImag = NULL;
	zReal = NULL;
}

int * LineMandelCalculator::calculateMandelbrot () {
	int *pdata = data;
	int acc;

	for (int i = 0; i < height; i++)
	{
		for (int iter = 0; iter < limit; iter++)
		{
			acc = 0;
			
			#pragma omp simd aligned(pdata:64) simdlen(64) reduction(+:acc)
			for (int j = i*width; j < width + j; j++)
			{
				float r2 = zReal[j] * zReal[j];
				float i2 = zImag[j] * zImag[j];

				pdata[j] = (pdata[j] == limit) && (r2 + i2 > 4.0f) ? acc++, iter: pdata[j];
				
				zImag[j] = 2.0f * zReal[j] * zImag[j] + imag[j];
				zReal[j] = r2 - i2 + real[j];
			}
			
			if(acc == width) break;
		}
	}
	return data;
}
