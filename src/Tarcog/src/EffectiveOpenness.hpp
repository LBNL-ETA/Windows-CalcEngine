#pragma once

#include <map>

namespace EffectiveLayers
{
    //! \brief Structure to hold data about shading device opennes as it is defined from user point
    //! of view
    struct ShadeOpenness
    {
        ShadeOpenness(double ah, double dl, double dr, double dtop, double dbot);
        double Ah;
        double Dl;
        double Dr;
        double Dtop;
        double Dbot;
    };

    //! \brief Effective openness of shading layer that is necessary for thermal calculations.
    //!
    //! Thermal openness of shading layer will not match physical openness and because of that some
    //! calculations are required.
    struct EffectiveOpenness
    {
        EffectiveOpenness(
          double ah, double al, double ar, double atop, double abot, double frontPorosity);
        bool isClosed() const;
        double Ah;
        double Al;
        double Ar;
        double Atop;
        double Abot;
        // Geometrical openness used to calculate equivalent layer conductivity
        double FrontPorosity;
    };

    struct Coefficients
    {
        Coefficients(double c1, double c2, double c3, double c4);
        double C1;
        double C2;
        double C3;
        double C4;
    };

    //! \brief Abstract class that will be used to inherit effective openness calculations for
    //! different shade types
    class EffectiveLayer
    {
    public:
        virtual ~EffectiveLayer() = default;
        EffectiveLayer(double width,
                       double height,
                       double thickness,
                       const ShadeOpenness & openness,
                       Coefficients coefficients = {0.0, 0.0, 0.0, 0.0});

        virtual EffectiveOpenness getEffectiveOpenness() = 0;

        virtual double effectiveThickness() = 0;

    protected:
        double m_Width;
        double m_Height;
        double m_Thickness;

        ShadeOpenness m_ShadeOpenness;

        Coefficients coefficients;
    };

    class EffectiveVenetian : public EffectiveLayer
    {
    public:
        EffectiveVenetian(double width,
                          double height,
                          double thickness,
                          const ShadeOpenness & openness,
                          double slatAngle,
                          double slatWidth,
                          Coefficients coefficients);

        EffectiveOpenness getEffectiveOpenness() override;
        double effectiveThickness() override;

    private:
        double m_SlatAngleRad;
        double m_SlatWidth;
    };

    class EffectiveHorizontalVenetian : public EffectiveVenetian
    {
    public:
        EffectiveHorizontalVenetian(double width,
                                    double height,
                                    double thickness,
                                    const ShadeOpenness & openness,
                                    double slatAngle,
                                    double SlatWidth);
    };

    class EffectiveVerticalVenentian : public EffectiveVenetian
    {
    public:
        EffectiveVerticalVenentian(double width,
                                   double height,
                                   double thickness,
                                   const ShadeOpenness & openness,
                                   double slatAngle,
                                   double slatWidth);
    };

    //! \brief Used for effective calculations for Perforated, Woven, Diffuse shade and BSDF
    class EffectiveLayerCommonType : public EffectiveLayer
    {
    public:
        EffectiveLayerCommonType(double width,
                                 double height,
                                 double thickness,
                                 const ShadeOpenness & openness);

        EffectiveOpenness getEffectiveOpenness() override;
        double effectiveThickness() override;
    };

    class EffectiveLayerPerforated : public EffectiveLayerCommonType
    {
    public:
        EffectiveLayerPerforated(double width,
                                 double height,
                                 double thickness,
                                 const ShadeOpenness & openness);
    };

    class EffectiveLayerDiffuse : public EffectiveLayerCommonType
    {
    public:
        EffectiveLayerDiffuse(double width,
                              double height,
                              double thickness,
                              const ShadeOpenness & openness);
    };

    class EffectiveLayerWoven : public EffectiveLayerCommonType
    {
    public:
        EffectiveLayerWoven(double width,
                            double height,
                            double thickness,
                            const ShadeOpenness & openness);
    };

    class EffectiveLayerBSDF : public EffectiveLayerCommonType
    {
    public:
        EffectiveLayerBSDF(double width,
                           double height,
                           double thickness,
                           const ShadeOpenness & openness);
    };

    class EffectiveLayerOther : public EffectiveLayer
    {
    public:
        EffectiveLayerOther(double width,
                            double height,
                            double thickness,
                            const ShadeOpenness & openness);

        EffectiveOpenness getEffectiveOpenness() override;
        double effectiveThickness() override;
    };
}   // namespace EffectiveLayers