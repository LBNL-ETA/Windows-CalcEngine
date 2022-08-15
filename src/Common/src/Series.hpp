#ifndef SERIES_H
#define SERIES_H

#include <vector>
#include <memory>
#include <optional>

namespace FenestrationCommon
{   // Implementation of spectral property interface
    class CSeriesPoint
    {
    public:
        CSeriesPoint();
        CSeriesPoint(CSeriesPoint const & t_SeriesPoint);
        CSeriesPoint(double t_Wavelength, double t_Value);
        [[nodiscard]] double x() const;
        void setX(double x);
        [[nodiscard]] double value() const;
        void setValue(double t_Value);
        CSeriesPoint & operator=(const CSeriesPoint & t_Point);
        bool operator<(const CSeriesPoint & t_Point) const;

    private:
        double m_x;
        double m_Value;
    };

    enum class IntegrationType;

    // Spectral properties for certain range of data. It holds common behavior like integration and
    // interpolation over certain range of data. class CSeries : public
    // std::enable_shared_from_this< CSeries > {
    class CSeries
    {
    public:
        CSeries() = default;

        explicit CSeries (size_t size);
        explicit CSeries(const std::vector<std::pair<double, double>> & t_values);
        CSeries(const std::initializer_list<std::pair<double, double>> & t_values);

        CSeries(const CSeries & t_Series) = default;
        void addProperty(double t_x, double t_Value);
        void setPropertyAtIndex(size_t index, double x, double value);
        void insertToBeginning(double t_x, double t_Value);

        // Create wavelength array with identical values over entire wavelength spectrum
        void setConstantValues(const std::vector<double> & t_x, double t_Value);

        [[nodiscard]] CSeries integrate(IntegrationType t_IntegrationType,
                          double normalizationCoefficient = 1,
                          const std::optional<std::vector<double>> & integrationPoints = std::nullopt) const;

        [[nodiscard]] CSeries interpolate(const std::vector<double> & t_Wavelengths) const;

        //! \brief Multiplication of values in spectral properties that have same wavelength.
        //!
        //! Function will work only if two spectral properties have identical wavelengths. Otherwise
        //! runtime error will be thrown. If two spectral properites do not have same wavelength
        //! range, then interpolation function should be called.
        CSeries operator*(const CSeries & other);

        //! \brief Subtraction of values in spectral properties that have same wavelength.
        //!
        //! Function will work only if two spectral properties have identical wavelengths. Otherwise
        //! runtime error will be thrown. If two spectral properites do not have same wavelength
        //! range, then interpolation function should be called.
        CSeries operator-(const CSeries & other) const;

        //! \brief Addition of values in spectral properties that have same wavelength.
        //!
        //! Function will work only if two spectral properties have identical wavelengths. Otherwise
        //! runtime error will be thrown. If two spectral properties do not have same wavelength
        //! range, then interpolation function should be called.
        CSeries operator+(const CSeries & other) const;

        // Return wavelength values for spectral properties.
        [[nodiscard]] std::vector<double> getXArray() const;
        [[nodiscard]] std::vector<double> getYArray() const;

        // Sum of all properties between two x values. Default arguments mean all items are sum
        double sum(double minX = 0, double maxX = 0) const;

        // Sort series by x values in ascending order
        void sort();

        [[nodiscard]] std::vector<CSeriesPoint>::const_iterator begin() const;
        [[nodiscard]] std::vector<CSeriesPoint>::const_iterator end() const;
        [[nodiscard]] size_t size() const;

        CSeries & operator=(const CSeries & t_Series) = default;
        const CSeriesPoint & operator[](size_t Index) const;

        void clear();

        void cutExtraData(double minWavelength, double maxWavelength);

    private:
        [[nodiscard]] std::optional<CSeriesPoint> findLower(double t_x) const;
        [[nodiscard]] std::optional<CSeriesPoint> findUpper(double t_x) const;
        static double interpolate(const CSeriesPoint & t_Lower,
                                  const CSeriesPoint & t_Upper,
                                  const double t_Wavelength);

        std::vector<CSeriesPoint> m_Series;
    };

    CSeries operator-(const double val, const CSeries & other);

}   // namespace FenestrationCommon

#endif
