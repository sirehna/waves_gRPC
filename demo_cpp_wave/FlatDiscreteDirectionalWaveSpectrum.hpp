#include <vector>

/** \brief Used by the wave models (eg. Airy, Stokes, etc.)
 *  \details No loop on frequency & space is necessary.
 *  \ingroup wave_models
 *  \section ex1 Example
 *  \snippet environment_models/unit_tests/src/DirectionalWaveSpectrumTest.cpp DirectionalWaveSpectrumTest example
 *  \section ex2 Expected output
 *  \snippet environment_models/unit_tests/src/DirectionalWaveSpectrumTest.cpp DirectionalWaveSpectrumTest expected output
 */
struct FlatDiscreteDirectionalWaveSpectrum
{
    FlatDiscreteDirectionalWaveSpectrum();
    //std::vector<double> a;
    double a;       //!< Amplitude (in m)
    double omega;   //!< Angular frequencies the spectrum was discretized at (in rad/s)
    double psi;     //!< Directions between 0 & 2pi the spatial spreading was discretized at (in rad)
    double k;       //!< Discretized wave number (for each frequency) in rad/m
    double phase;   //!< Random phases, for each (direction,frequency) couple (but time invariant) in radian
};


FlatDiscreteDirectionalWaveSpectrum::FlatDiscreteDirectionalWaveSpectrum