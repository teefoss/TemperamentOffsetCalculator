#include <stdio.h>
#include <math.h>

#define NUM_NOTES 12

// calculate a frequency (Hz) that is 'n' cents offset from frequenct 'a' (hz)
double frequency_offset(double a, double n)
{
    return a * pow(2.0, n / 1200.0);
}

// returns the difference, in cents, of frequencies 'lower' and 'higher' (Hz)
double cents_delta(double lower, double higher)
{
    return 1200.0 * log2(lower / higher);
}

// round to two decimal places
double round2(double freq)
{
    return round(freq * 100.0) / 100.0;
}

int main(int argc, char ** argv)
{
    if ( argc < 2 ) {
        printf("usage: %s [input file] ([output file])\n", argv[0]);
        printf("Data is output to console if no output file is specified.\n");
        return 1;
    }
        
    FILE * input = fopen(argv[1], "r");
    if ( input == NULL ) {
        printf("could not open %s!\n", argv[1]);
        return 1;
    }
    
    FILE * output;
    if ( argc == 3 ) { // user specified output file
        output = fopen(argv[2], "w");
        if ( output == NULL ) {
            fprintf(stderr, "Error: failed to create file %s\n", argv[2]);
            return 1;
        }
    } else {
        output = stdout;
    }
    
    // calculate ET frequencies
    
    double et_freq[NUM_NOTES]; // from A4 to G sharp 4
    for ( int i = 0; i < NUM_NOTES; i++ ) {
        int index = (i + 9) % NUM_NOTES;
        et_freq[index] = 440.0 * pow(2.0, (double)i / 12.0);
    }
    
    // calculate temperament freqencies given offsets in file
    
    double freq[NUM_NOTES];
    for ( int i = 0; i < NUM_NOTES - 1; i++ ) {
        int lower;
        int upper;
        double offset_cents;
        
        fscanf(input, "%d-%d %lf\n", &lower, &upper, &offset_cents);
        if ( i == 0 ) {
            freq[lower] = et_freq[lower];
        }
        
        freq[upper] = frequency_offset(freq[lower] * 1.5, offset_cents);
    }
    
    // calculate and print cent offsets from ET of temperament frequencies
    
    for ( int i = 0; i < NUM_NOTES; i++ ) {
        
        // move all frequencies into one octave range
        while ( freq[i] < 440.0 )
            freq[i] *= 2.0;
        while ( freq[i] >= 880.0 )
            freq[i] /= 2.0;
        
        const char * format = "pitch %2d: (ET: %3.2lf Hz) (T: %3.2lf Hz), delta: %+3.2lf cents\n";
        double rounded_delta = round2(cents_delta(freq[i], et_freq[i]));
        
        fprintf(output, format, i, et_freq[i], freq[i], rounded_delta);
    }
    
    fclose(input);
    fclose(output);
    return 0;
}
