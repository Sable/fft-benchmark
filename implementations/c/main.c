#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fft.h"
#include "common.h"
#include "common_rand.h"

typedef struct index_2d_struct {
   int row;
   int col;
} index_2d;

complex* random_complex_vector(int n){
    complex *x = malloc(sizeof(complex)*n);
    int i;

    for(i=0;i<n; ++i){
        x[i].re = common_randJS()*2 - 1;
        x[i].im = common_randJS()*2 - 1;
    }
    return x;
}

// n is the number of elements in the vector
// amplitude is the amplitude of the sinusoidal signal
// period_divisor is the length of the sinusoidal signal as a divisor of the number of samples (ex: 1 means the period spans n samples, 2 means the period spans n/2 samples, etc.)
complex* sin_complex_vector(int n, double amplitude, int period_divisor) {
    complex *x = malloc(sizeof(complex)*n);
    int i;

    double step = ((double)period_divisor * 4 * M_PI) / ((double)n-1); 

    for(i=0;i<n; ++i){
        x[i].re = amplitude * sin(step * i);
        x[i].im = 0;
    }
    return x;
}


index_2d index_of_max_real (complex** m, int n) {
    int i;
    int j;

    double max_value = -INFINITY;
    index_2d index;
    index.row = 0;
    index.col = 0;

    for(i=0; i<n; ++i) {
        for(j=0; j<n; ++j) {
            if (m[i][j].re > max_value) {
                max_value = m[i][j].re;
                index.row = i;
                index.col = j;
            } 
        }
    }
    return index;
}

int nb_of_significant_signals (complex** m, int n, double threshold) {
    int i;
    int j;

    int count = 0;

    for(i=0; i<n; ++i) {
        for(j=0; j<n; ++j) {
            if (m[i][j].re > threshold) {
                count++;
            } 
        }
    }
    return count;
}

void print_complex_array(complex *x, int n){
    int i;
    for(i = 0; i < n; ++i) fprintf(stderr, "%.6f + %.6fi\n", x[i].re, x[i].im);
}

void print_complex_matrix(complex **x, int n){
    int i;
    for(i=0; i < n; ++i) { print_complex_array(x[i], n); fprintf(stderr, "\n"); }
    fprintf(stderr, "\n");
}

int main(int argc, char** argv){
    complex *x, **m;
    int n = 1024;
    int i;
    stopwatch sw;
    int c;
    int two_exp = 10;

    if (argc > 1) {
        two_exp = atoi(argv[1]);
        n = 1 << two_exp;
        if (two_exp < 0 || two_exp > 30) {
            fprintf(stderr, "ERROR: invalid exponent of '%d' for input size\n",  n);
            exit(1);
        }

    } else {

        fprintf(stderr, "Invalid number of inputs, expecting one numbers\n");
        exit(1);
    }


    // Test 1D arrays
    /*
    x = random_complex_vector(n);
    stopwatch_start(&sw);
    complex *results = FFT_simple(x,n);
    stopwatch_stop(&sw);
    fprintf(stderr, "The total 1D FFT time for %d size was %lf seconds!\n", n,
      get_interval_by_sec(&sw));
    */

    // Test 2D arrays
    m = malloc(sizeof(complex*)*n);
    for(i=0; i<n; ++i) m[i] = sin_complex_vector(n, 1, 1);

    stopwatch_start(&sw);
    complex **results2D = FFT_2D(m,n);
    stopwatch_stop(&sw);

    double amplitude = results2D[0][2].re;
    printf("nb of significant signals %d\n", nb_of_significant_signals(results2D,n,10));
    printf("value at (0,2): %f\n", amplitude);

    double exp = log10(amplitude);

    if (exp > 0) {
        exp = ceil(exp);
    } else {
        exp = floor(exp);
    }

    double normalized_amplitude = floor((1000/pow(10,exp)) * amplitude);

    printf("{ \"status\": %d, \"options\": \"%d\", \"time\": %f, \"output\": { \"nb-significant-signals\": %d, \"normalized-amplitude\": %d } }\n", 1, two_exp, get_interval_by_sec(&sw), nb_of_significant_signals(results2D,n,10), (int)normalized_amplitude);

    for(i=0; i<n; ++i) free(m[i]);
    free(m);

    for(i=0; i<n; ++i) free(results2D[i]);
    free(results2D);
}
