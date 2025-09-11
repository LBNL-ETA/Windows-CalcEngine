#ifndef ANGULARPROPERTIES_H
#define ANGULARPROPERTIES_H

#include <memory>

namespace FenestrationCommon
{
    enum class SurfaceType;
}

namespace SpectralAveraging
{
    // Calculates angular propertes at given angle
    class CAngularProperties
    {
    public:
        virtual ~CAngularProperties() = default;
        CAngularProperties(double const t_TransmittanceZero, double const t_ReflectanceZero);

        virtual double transmittance(double const t_Angle, double const t_Wavelength = 0) = 0;
        virtual double reflectance(double const t_Angle, double const t_Wavelength = 0) = 0;

    protected:
        double cosAngle(double const t_Angle) const;
        virtual void checkStateProperties(double const t_Angle, double const t_Wavelength);

        double m_Transmittance0;
        double m_Reflectance0;

        double m_Transmittance;
        double m_Reflectance;
        double m_StateAngle;
        double m_StateWavelength;
    };

    // Angular properties for uncoated type of glass layer
    class CAngularPropertiesUncoated : public CAngularProperties
    {
    public:
        CAngularPropertiesUncoated(const double t_Thicknes,
                                   const double t_TransmittanceZero,
                                   const double t_ReflectanceZero);

        double transmittance(const double t_Angle, const double t_Wavelength) override;
        double reflectance(const double t_Angle, const double t_Wavelength) override;

    protected:
        void checkStateProperties(const double t_Angle, const double t_Wavelength) override;

    private:
        double m_Thickness;
        double m_Beta;
        double m_Rho0;
    };

    class CAngularPropertiesCoated : public CAngularProperties
    {
    public:
        CAngularPropertiesCoated(double t_Transmittance, double t_Reflectance);

        double transmittance(double t_Angle, double t_Wavelength = 0) override;
        double reflectance(double t_Angle, double t_Wavelength = 0) override;

    protected:
        void checkStateProperties(double t_Angle, double t_Wavelength) override;
    };

    enum class CoatingProperty
    {
        T,
        R
    };

    enum class CoatingType
    {
        Clear,
        Bronze
    };

    class Coefficients
    {
    public:
        Coefficients() = default;
        Coefficients(double t_C0, double t_C1, double t_C2, double t_C3, double t_C4);
        [[nodiscard]] double interpolation(double t_Value) const;

    private:
        double C0{0};
        double C1{0};
        double C2{0};
        double C3{0};
        double C4{0};
    };

    // creates coating coefficients according to property and type.
    namespace CoatingCoefficients
    {
        [[nodiscard]] Coefficients getCoefficients(CoatingProperty t_Property, CoatingType t_Type);
    };

    class CAngularPropertiesFactory
    {
    public:
        CAngularPropertiesFactory(double t_Transmittance0,
                                  double t_Reflectance0,
                                  double t_Thickness = 0);

        std::shared_ptr<CAngularProperties>
          getAngularProperties(FenestrationCommon::SurfaceType t_SurfaceType);

    private:
        double m_Thickness;
        double m_Transmittance0;
        double m_Reflectance0;
    };

}   // namespace SpectralAveraging

#endif
