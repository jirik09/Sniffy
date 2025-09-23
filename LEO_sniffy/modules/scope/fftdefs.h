#ifndef FFTDEFS_H
#define FFTDEFS_H

// Lightweight header containing only FFT enums to avoid pulling QThread and other
// heavy Qt headers into every translation unit that needs the enum types.
// If additional FFT-related plain-data types (POD) are needed, prefer adding
// them here rather than including fftengine.h in widely used headers.

enum FFTWindow { rectangular = 0, hamming = 1, hann = 2, blackman = 3, flatTop = 4};
enum FFTType { spectrum = 0, periodogram = 1};

#endif // FFTDEFS_H
