#include <stdexcept>
#include <algorithm>
#include <cmath>

#include "Series.hpp"
#include "IntegratorStrategy.hpp"


namespace FenestrationCommon
{
    /////////////////////////////////////////////////////
    //  CSeriesPoint
    /////////////////////////////////////////////////////

    CSeriesPoint::CSeriesPoint() : m_x(0), m_Value(0)
    {}

    CSeriesPoint::CSeriesPoint(CSeriesPoint const & t_SeriesPoint)
    {
        *this = t_SeriesPoint;
    }

    CSeriesPoint::CSeriesPoint(double t_Wavelength, double t_Value) :
        m_x(t_Wavelength), m_Value(t_Value)
    {}

    double CSeriesPoint::x() const
    {
        return m_x;
    }

    void CSeriesPoint::setX(double x)
    {
        m_x = x;
    }

    double CSeriesPoint::value() const
    {
        return m_Value;
    }

    void CSeriesPoint::setValue(double const t_Value)
    {
        m_Value = t_Value;
    }

    CSeriesPoint & CSeriesPoint::operator=(const CSeriesPoint & t_Property)
    {
        if(this == &t_Property)
        {
            return *this;
        }

        m_x = t_Property.m_x;
        m_Value = t_Property.m_Value;
        return *this;
    }

    bool CSeriesPoint::operator<(const CSeriesPoint & t_Point) const
    {
        return m_x < t_Point.m_x;
    }

    /////////////////////////////////////////////////////
    //  CSeries
    /////////////////////////////////////////////////////

    CSeries::CSeries(size_t size) : m_Series(size)
    {}

    CSeries::CSeries(const std::vector<std::pair<double, double>> & t_values)
    {
        m_Series.reserve(t_values.size());
        for(auto & val : t_values)
        {
            m_Series.emplace_back(val.first, val.second);
        }
    }

    CSeries::CSeries(const std::initializer_list<std::pair<double, double>> & t_values)
    {
        m_Series.reserve(t_values.size());
        for(const auto & val : t_values)
        {
            m_Series.emplace_back(val.first, val.second);
        }
    }

    void CSeries::addProperty(const double t_x, const double t_Value)
    {
        m_Series.emplace_back(t_x, t_Value);
    }

    void CSeries::setPropertyAtIndex(size_t index, double x, double value)
    {
        m_Series[index].setX(x);
        m_Series[index].setValue(value);
    }

    void CSeries::insertToBeginning(double t_x, double t_Value)
    {
        m_Series.insert(m_Series.begin(), {t_x, t_Value});
    }

    void CSeries::setConstantValues(const std::vector<double> & t_Wavelengths, double const t_Value)
    {
        m_Series.clear();
        m_Series.reserve(t_Wavelengths.size());
        for(auto it = t_Wavelengths.begin(); it < t_Wavelengths.end(); ++it)
        {
            addProperty((*it), t_Value);
        }
    }

    CSeries CSeries::integrate(IntegrationType t_IntegrationType,
                               const double normalizationCoefficient,
                               const std::optional<std::vector<double>> & integrationPoints) const
    {
        if(!integrationPoints)
        {
            // No copy: pass *this
            return FenestrationCommon::integrate(
              t_IntegrationType, *this, normalizationCoefficient);
        }
        // Keep the temporary alive so its storage is valid during integration
        CSeries tmp = interpolate(*integrationPoints);
        return FenestrationCommon::integrate(t_IntegrationType, tmp, normalizationCoefficient);
    }

    std::optional<CSeriesPoint> CSeries::findLower(double const t_Wavelength) const
    {
        std::optional<CSeriesPoint> currentProperty;

        for(auto & spectralProperty : m_Series)
        {
            if(spectralProperty.x() > t_Wavelength)
            {
                break;
            }
            currentProperty = spectralProperty;
        }

        return currentProperty;
    }

    std::optional<CSeriesPoint> CSeries::findUpper(double const t_Wavelength) const
    {
        std::optional<CSeriesPoint> currentProperty;

        for(auto & spectralProperty : m_Series)
        {
            if(spectralProperty.x() > t_Wavelength)
            {
                currentProperty = spectralProperty;
                break;
            }
        }

        return currentProperty;
    }

    double CSeries::interpolate(const CSeriesPoint & t_Lower,
                                const CSeriesPoint & t_Upper,
                                double const t_Wavelength)
    {
        double w1 = t_Lower.x();
        double w2 = t_Upper.x();
        double v1 = t_Lower.value();
        double v2 = t_Upper.value();
        double vx = 0;
        if(w2 != w1)
        {
            vx = v1 + (t_Wavelength - w1) * (v2 - v1) / (w2 - w1);
        }
        else
        {
            vx = v1;   // extrapolating same value for all values out of range
        }

        return vx;
    }

    CSeries CSeries::interpolate(const std::vector<double> & t_Wavelengths) const
    {
        CSeries out;
        out.m_Series.reserve(t_Wavelengths.size());
        if(size() != 0)
        {
            for(double w : t_Wavelengths)
            {
                std::optional<CSeriesPoint> lower = findLower(w);
                std::optional<CSeriesPoint> upper = findUpper(w);
                if(!lower)
                    lower = upper;
                if(!upper)
                    upper = lower;
                out.addProperty(w, interpolate(lower.value(), upper.value(), w));
            }
        }
        return out;
    }

    CSeries CSeries::operator*(const CSeries & other) const
    {
        const double WAVELENGTHTOLERANCE = 1e-10;
        const size_t minSize = std::min(m_Series.size(), other.m_Series.size());

        CSeries newProperty;
        newProperty.m_Series.reserve(minSize);

        for(size_t i = 0; i < minSize; ++i)
        {
            const double wv = m_Series[i].x();
            const double testWv = other.m_Series[i].x();

            if(std::abs(wv - testWv) > WAVELENGTHTOLERANCE)
            {
                throw std::runtime_error("The wavelengths of the two vectors are not the same. "
                                         "Cannot perform multiplication.");
            }
            newProperty.addProperty(wv, m_Series[i].value() * other.m_Series[i].value());
        }

        return newProperty;
    }

    CSeries CSeries::operator-(const CSeries & t_Series) const
    {
        const double WAVELENGTHTOLERANCE = 1e-10;
        const size_t minSize = std::min(m_Series.size(), t_Series.m_Series.size());

        CSeries newProperties;
        newProperties.m_Series.reserve(minSize);

        for(size_t i = 0; i < minSize; ++i)
        {
            const double wv = m_Series[i].x();
            const double testWv = t_Series.m_Series[i].x();

            if(std::abs(wv - testWv) > WAVELENGTHTOLERANCE)
            {
                throw std::runtime_error(
                  "Wavelengths of two vectors are not the same. Cannot preform subtraction.");
            }

            newProperties.addProperty(wv, m_Series[i].value() - t_Series.m_Series[i].value());
        }

        return newProperties;
    }

    CSeries operator-(const double val, const CSeries & other)
    {
        CSeries newProperties;

        for(const auto & ot : other)
        {
            double value = val - ot.value();
            double wv = ot.x();

            newProperties.addProperty(wv, value);
        }

        return newProperties;
    }

    CSeries CSeries::operator+(const CSeries & other) const
    {
        const double WAVELENGTHTOLERANCE = 1e-10;
        const size_t minSize = std::min(m_Series.size(), other.m_Series.size());

        CSeries newProperties;
        newProperties.m_Series.reserve(minSize);

        for(size_t i = 0; i < minSize; ++i)
        {
            const double wv = m_Series[i].x();
            const double testWv = other.m_Series[i].x();

            if(std::abs(wv - testWv) > WAVELENGTHTOLERANCE)
            {
                throw std::runtime_error(
                  "Wavelengths of two vectors are not the same. Cannot preform addition.");
            }

            newProperties.addProperty(wv, m_Series[i].value() + other.m_Series[i].value());
        }

        return newProperties;
    }

    namespace Helper
    {
        template<typename Func>
        std::vector<double> getArray(const CSeries & series, Func && extractor)
        {
            std::vector<double> aArray(series.size());
            std::transform(
              series.begin(), series.end(), aArray.begin(), std::forward<Func>(extractor));
            return aArray;
        }
    }   // namespace Helper

    std::vector<double> CSeries::getXArray() const
    {
        return Helper::getArray(*this,
                                [](const auto & spectralProperty) { return spectralProperty.x(); });
    }

    std::vector<double> CSeries::getYArray() const
    {
        return Helper::getArray(
          *this, [](const auto & spectralProperty) { return spectralProperty.value(); });
    }

    double CSeries::sum(double const minLambda, double const maxLambda) const
    {
        double const TOLERANCE = 1e-6;   // introduced because of rounding error
        double total = 0;
        for(auto & aPoint : m_Series)
        {
            double wavelength = aPoint.x();
            // Last point must be excluded because of ranges. Each wavelength represent range from
            // wavelength one to wavelength two. Summing value of the last wavelength in array would
            // be wrong because it would include one additional range after the end of spectrum. For
            // example, summing all the data from 0.38 to 0.78 would include visible range. However,
            // including 0.78 in sum would add extra value from 0.78 to 0.79.
            if(((wavelength >= (minLambda - TOLERANCE) && wavelength < (maxLambda - TOLERANCE))
                || (minLambda == 0 && maxLambda == 0)))
            {
                total += aPoint.value();
            }
        }
        return total;
    }

    void CSeries::sort()
    {
        std::sort(
          m_Series.begin(),
          m_Series.end(),
          [](CSeriesPoint const & l, CSeriesPoint const & r) -> bool { return l.x() < r.x(); });
    }

    std::vector<CSeriesPoint>::const_iterator CSeries::begin() const
    {
        return m_Series.cbegin();
    }

    std::vector<CSeriesPoint>::const_iterator CSeries::end() const
    {
        return m_Series.cend();
    }

    size_t CSeries::size() const
    {
        return m_Series.size();
    }

    const CSeriesPoint & CSeries::operator[](size_t Index) const
    {
        if(Index >= m_Series.size())
        {
            throw std::out_of_range("Index out of range.");
        }
        return m_Series[Index];
    }

    void CSeries::clear()
    {
        m_Series.clear();
    }

    void CSeries::cutExtraData(double minWavelength, double maxWavelength)
    {
        constexpr double eps = 1e-8;
        m_Series.erase(std::ranges::remove_if(m_Series,
                                              [&](const CSeriesPoint & v) {
                                                  return !(v.x() > (minWavelength - eps)
                                                           && v.x() < (maxWavelength + eps));
                                              })
                         .begin(),
                       m_Series.end());
    }

}   // namespace FenestrationCommon
