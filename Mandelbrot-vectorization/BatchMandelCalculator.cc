/**
 * @file BatchMandelCalculator.cc
 * @author MATEJ OTCENAS <xotcen01@stud.fit.vutbr.cz>
 * @brief Implementation of Mandelbrot calculator that uses SIMD paralelization over small batches
 * @date DATE 11.11.2021
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <immintrin.h>
#include <stdlib.h>
#include <stdexcept>

#include "BatchMandelCalculator.h"

BatchMandelCalculator::BatchMandelCalculator (unsigned matrixBaseSize, unsigned limit) :
	BaseMandelCalculator(matrixBaseSize, limit, "BatchMandelCalculator")
{
	data = (int *)_mm_malloc(height * width * sizeof(int), 64);
	zImag = (float *)_mm_malloc(height * width * sizeof(float), 64);
	zReal = (float *)_mm_malloc(height * width * sizeof(float), 64);
	real = (float *)_mm_malloc(height * width * sizeof(float), 64);
	imag = (float *)_mm_malloc(height * width * sizeof(float), 64);


	#pragma omp simd simdlen(64) collapse(2)
	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++) {
			int idx = i * width + j;

			data[idx] = limit;

			real[idx] = x_start + j * dx; 
			imag[idx] = y_start + i * dy;

			zReal[idx] = real[idx];
			zImag[idx] = imag[idx];
		}
	}
}

BatchMandelCalculator::~BatchMandelCalculator() {
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


int * BatchMandelCalculator::calculateMandelbrot () {
	int *pdata = data;
	int acc = 0;

	constexpr int blockSize = 128;

	for (int blockH = 0; blockH < height / blockSize; blockH++)
	{
		for (int blockW = 0; blockW < width / blockSize; blockW++)
		{
			for (int i = 0; i < blockSize; i++)
			{
				const size_t iGlobal = blockH * blockSize + i;

				for (int iter = 0; iter < limit; iter++)
				{
					acc = 0;

					#pragma omp simd aligned(pdata:64) simdlen(64) reduction(+:acc)
					for (int j = (blockW * blockSize) + width * iGlobal; j < blockSize + j; j++)
					{
						float r2 = zReal[j] * zReal[j];
						float i2 = zImag[j] * zImag[j];

						pdata[j] = (pdata[j] == limit) && (r2 + i2 > 4.0f) ? acc++, iter: pdata[j];
						
						zImag[j] = 2.0f * zReal[j] * zImag[j] + imag[j];
						zReal[j] = r2 - i2 + real[j];
					}
					
					if(acc == blockSize) break;
				}
			}	
		}
	}
	return data;
}


