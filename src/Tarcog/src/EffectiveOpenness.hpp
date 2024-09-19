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

    //! \brief Effective frontOpenness of shading layer that is necessary for thermal calculations.
    //!
    //!
    struct EffectiveOpenness
    {
        EffectiveOpenness(double effectiveFrontThermalOpennessArea,
                          double al,
                          double ar,
                          double atop,
                          double abot,
                          double frontPorosity);

        //!< Effective openness of the layer for thermal calculations between the gaps
        double EffectiveFrontThermalOpennessArea;
        //!< Area of the left side openness where the air is flowing between the gaps
        double Al;
        //!< Area of the right side openness where the air is flowing between the gaps
        double Ar;
        //!< Area of the top side openness where the air is flowing between the gaps
        double Atop;
        //!< Area of the bottom side openness where the air is flowing between the gaps
        double Abot;
        // Geometrical openness used to calculate equivalent layer conductivity
        double PermeabilityFactor;
    };

    bool isClosed(const EffectiveOpenness & effectiveOpenness);

    struct Coefficients
    {
        Coefficients(double c1, double c2, double c3, double c4);
        double C1;
        double C2;
        double C3;
        double C4;
    };

    //! \brief Abstract class that will be used to inherit effective frontOpenness calculations for
    //! different shade types
    class EffectiveLayer
    {
    public:
        virtual ~EffectiveLayer() = default;
        EffectiveLayer(double width,
                       double height,
                       double thickness,
                       const ShadeOpenness & openness,
                       const Coefficients & coefficients = {0.0, 0.0, 0.0, 0.0},
                       double permeabilityFactor = 0.0);

        virtual EffectiveOpenness getEffectiveOpenness() = 0;

        virtual double effectiveThickness() = 0;

    protected:
        double m_Width;
        double m_Height;
        double m_Thickness;

        ShadeOpenness m_ShadeOpenness;

        Coefficients coefficients;

        double m_PermeabilityFactor;
    };

    class EffectiveVenetian : public EffectiveLayer
    {
    public:
        EffectiveVenetian(double width,
                          double height,
                          double thickness,
                          double slatAngle,
                          double slatWidth,
                          const ShadeOpenness & openness,
                          const Coefficients & coefficients);

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

    class EffectiveVerticalVenetian : public EffectiveVenetian
    {
    public:
        EffectiveVerticalVenetian(double width,
                                  double height,
                                  double thickness,
                                  const ShadeOpenness & openness,
                                  double slatAngle,
                                  double slatWidth);
    };

    //! \brief Used for effective calculations where permeability is linear with frontOpenness Ah
    class EffectiveLayerLinearPermeability : public EffectiveLayer
    {
    public:
        EffectiveLayerLinearPermeability(double width,
                                         double height,
                                         double thickness,
                                         const ShadeOpenness & openness);

        EffectiveOpenness getEffectiveOpenness() override;
        double effectiveThickness() override;
    };

    class EffectiveLayerCommonType : public EffectiveLayer
    {
    public:
        EffectiveLayerCommonType(double width,
                                 double height,
                                 double thickness,
                                 const ShadeOpenness & openness,
                                 double permeabilityFactor);

        EffectiveOpenness getEffectiveOpenness() override;
        double effectiveThickness() override;
    };

    class EffectiveLayerPerforated : public EffectiveLayerLinearPermeability
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
                              const ShadeOpenness & openness,
                              double permeabilityFactor);
    };

    class EffectiveLayerWoven : public EffectiveLayerLinearPermeability
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
                           const ShadeOpenness & openness,
                           double permeabilityFactor);
    };

    class EffectiveLayerOther : public EffectiveLayer
    {
    public:
        EffectiveLayerOther(double width,
                            double height,
                            double thickness,
                            const ShadeOpenness & openness,
                            double permeabilityFactor);

        EffectiveOpenness getEffectiveOpenness() override;
        double effectiveThickness() override;
    };
}   // namespace EffectiveLayers