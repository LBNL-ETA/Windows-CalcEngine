#pragma once

#include <cmath>
#include <limits>

namespace ConstantsData
{
    static const double WCE_PI = 4.0 * std::atan(1.0);
    static const double UNIVERSALGASCONSTANT = 8314.462175;
    static const double KELVINCONV = 273.15;
    static const double STEFANBOLTZMANN = 5.6697E-8;
    static const double GRAVITYCONSTANT = 9.807;
    static const double VACUUMPRESSURE = 13.3322;
    static const double SPEEDOFLIGHT = 299792458;
    static const double PLANKCONSTANT = 6.626196e-34;
    static const double BOLTZMANNCONSTANT = 1.380622e-23;
    static const double floatErrorTolerance = 1e-12;
    static const double wavelengthErrorTolerance = 1e-6;
    static const double ELECTRON_CHARGE = 1.502e-19;
    static const double EOGHeight = 0.0635;   // meters
    static const double DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT = 0.89;
    static const double DEFAULT_GAP_PRESSURE = 101325;

    //! Default ratio used in scaling of the materials that are defined only with solar and visible
    //! range.
    static const double NIRRatio = 0.499;

    //! This is used in conjuntion with materials that are defined with solar and visible range
    //! only. Since that material at the wavelength of 0.78 needs to return visible properties, this
    //! will be used to create small offset from the end of the visible range to make sure that
    //! visible properties are returned in that case.
    static const double VisibleRangeOffset = 2 * wavelengthErrorTolerance;

    static const double MINLAMBDAVALUE = 0;

    // Brackets around std::numeric_limits<double>::max() are needed to avoid a bug in MSVC
    static const double MAXLAMBDAVALUE = (std::numeric_limits<double>::max)();
}   // namespace ConstantsData
