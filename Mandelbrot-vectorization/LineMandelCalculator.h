/**
 * @file LineMandelCalculator.h
 * @author MATEJ OTCENAS <xotcen01@stud.fit.vutbr.cz>
 * @brief Implementation of Mandelbrot calculator that uses SIMD paralelization over lines
 * @date DATE 11.11.2021
 */

#include <BaseMandelCalculator.h>

class LineMandelCalculator : public BaseMandelCalculator
{
public:

    LineMandelCalculator(unsigned matrixBaseSize, unsigned limit);
    ~LineMandelCalculator();
    int *calculateMandelbrot();

private:
    int* data;
    float* real;
    float* imag;
    float* zReal;
    float* zImag;
};
