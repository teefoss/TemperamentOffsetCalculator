#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_NOTES 12

const char * note_names[NUM_NOTES] = {
    "C ", "C♯", "D ", "E♭", "E ", "F ", "F♯", "G ", "A♭", "A ", "B♭", "B "
};

/// Calculate a frequency (Hz) that is offset from an other frequency by a
/// given value in cents.
double frequency_offset(double freq_hz, double offset_cents)
{
    return freq_hz * pow(2.0, offset_cents / 1200.0);
}

/// Returns the difference, in cents, of frequencies between 'lower' and
/// 'higher' (Hz).
double cents_delta(double lower, double higher)
{
    return 1200.0 * log2(lower / higher);
}

/// Round to two decimal places.
double round2(double freq)
{
    return round(freq * 100.0) / 100.0;
}

int main(int argc, char ** argv)
{
    if ( argc < 2 ) {
        printf("usage: %s [input file] ([output file])\n", argv[0]);
        printf("Data is output to console if no output file is specified.\n");
        return EXIT_FAILURE;
    }
        
    FILE * input = fopen(argv[1], "r");

    if ( input == NULL ) {
        printf("could not open %s!\n", argv[1]);
        return EXIT_FAILURE;
    }
    
    FILE * output = stdout;

    if ( argc == 3 ) { // User specified output file.
        output = fopen(argv[2], "w");
        if ( output == NULL ) {
            fprintf(stderr, "Error: failed to create file %s\n", argv[2]);
            return EXIT_FAILURE;
        }
    }
    
    // Calculate ET frequencies.

    double et_freq[NUM_NOTES]; // from A4 to G sharp 4
    for ( int i = 0; i < NUM_NOTES; i++ ) {
        int index = (i + 9) % NUM_NOTES;
        et_freq[index] = 440.0 * pow(2.0, (double)i / 12.0);
    }
    
    // Calculate temperament freqencies from given offsets in file.

    double freq[NUM_NOTES];

    for ( int i = 0; i < NUM_NOTES - 1; i++ ) {

        int lower, upper;
        double offset_cents;
        fscanf(input, "%d-%d %lf\n", &lower, &upper, &offset_cents);

        if ( i == 0 ) {
            freq[lower] = et_freq[lower]; // A is unchanged.
        }
        
        freq[upper] = frequency_offset(freq[lower] * 1.5, offset_cents);
    }
    
    // calculate and print cent offsets from ET of temperament frequencies
    
    for ( int i = 0; i < NUM_NOTES; i++ ) {
        
        // Move all frequencies into one octave range.
        while ( freq[i] < 440.0 ) {
            freq[i] *= 2.0;
        }

        while ( freq[i] >= 880.0 ) {
            freq[i] /= 2.0;
        }

        double rounded_delta = round2(cents_delta(freq[i], et_freq[i]));
        fprintf(output, "%s  %+3.2lf cents\n", note_names[i], rounded_delta);
    }
    
    fclose(input);
    fclose(output);

    return EXIT_SUCCESS;
}
