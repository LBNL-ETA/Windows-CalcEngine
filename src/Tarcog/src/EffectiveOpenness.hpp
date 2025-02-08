#pragma once

#include <map>

#include <WCECommon.hpp>

#include "PermeabilityFactor.hpp"

namespace EffectiveLayers
{
    //! \brief Structure to hold data about shading device opennes as it is defined from user point
    //! of view
    struct ShadeOpenness
    {
        ShadeOpenness(double dl, double dr, double dtop, double dbot);

        //! \var Dl
        //! \brief Left side openness of the shading layer [m]
        double Dl;

        //! \var Dr
        //! \brief Right side openness of the shading layer [m]
        double Dr;

        //! \var Dtop
        //! \brief Top side openness of the shading layer [m]
        double Dtop;

        //! \var Dbot
        //! \brief Bottom side openness of the shading layer [m]
        double Dbot;
    };

    //! \brief Effective permeabilityFactor of shading layer that is necessary for thermal
    //! calculations.
    struct EffectiveOpenness
    {
        EffectiveOpenness(double effectiveFrontThermalOpennessArea,
                          double al,
                          double ar,
                          double atop,
                          double abot,
                          double permeabilityFactor);

        //! \var EffectiveFrontThermalOpennessArea
        //! \brief Effective front thermal openness area used for in between gap (or environment
        //! calculations) [m²]
        double EffectiveFrontThermalOpennessArea;

        //! \var Al
        //! \brief Area of the left side openness where the air is flowing between the gaps [m²]
        double Al;

        //! \var Ar
        //! \brief Area of the right side openness where the air is flowing between the gaps [m²]
        double Ar;

        //! \var Atop
        //! \brief Area of the top side openness where the air is flowing between the gaps [m²]
        double Atop;

        //! \var Abot
        //! \brief Area of the bottom side openness where the air is flowing between the gaps [m²]
        double Abot;

        //! \var PermeabilityFactor
        //! \brief Geometrical openness used to calculate equivalent layer conductivity
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

    //! \brief Abstract class that will be used to inherit effective permeabilityFactor calculations
    //! for different shade types
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

        [[nodiscard]] virtual EffectiveOpenness getEffectiveOpenness() = 0;

        [[nodiscard]] virtual double effectiveThickness() = 0;

        [[nodiscard]] double permeabilityFactor() const;

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
                          const FenestrationCommon::Venetian::Geometry & geometry,
                          const ShadeOpenness & openness,
                          const Coefficients & coefficients);

        EffectiveOpenness getEffectiveOpenness() override;
        double effectiveThickness() override;

    protected:
        FenestrationCommon::Venetian::Geometry m_Geometry;
    };

    class EffectiveHorizontalVenetian : public EffectiveVenetian
    {
    public:
        EffectiveHorizontalVenetian(double width,
                                    double height,
                                    double thickness,
                                    const FenestrationCommon::Venetian::Geometry & geometry,
                                    const ShadeOpenness & openness = {0, 0, 0, 0});
    };

    class EffectiveVerticalVenetian : public EffectiveVenetian
    {
    public:
        EffectiveVerticalVenetian(double width,
                                  double height,
                                  double thickness,
                                  const FenestrationCommon::Venetian::Geometry & geometry,
                                  const ShadeOpenness & openness = {0, 0, 0, 0});
    };

    class EffectiveLayerCommon : public EffectiveLayer
    {
    public:
        EffectiveLayerCommon(double width,
                             double height,
                             double thickness,
                             double permeabilityFactor = 0,
                             const ShadeOpenness & openness = {0, 0, 0, 0});

        EffectiveOpenness getEffectiveOpenness() override;
        double effectiveThickness() override;
    };

    class EffectiveLayerLouveredShutter : public EffectiveLayer
    {
    public:
        EffectiveLayerLouveredShutter(
          double width,
          double height,
          double thickness,
          const FenestrationCommon::LouveredShutter::Geometry & geometry,
          const ShadeOpenness & openness = {0, 0, 0, 0});

        [[nodiscard]] EffectiveOpenness getEffectiveOpenness() override;
        [[nodiscard]] double effectiveThickness() override;

    private:
        FenestrationCommon::LouveredShutter::Geometry m_Geometry;
    };

    class EffectiveLayerPerforated : public EffectiveLayerCommon
    {
    public:
        EffectiveLayerPerforated(double width,
                                 double height,
                                 double thickness,
                                 const FenestrationCommon::Perforated::Geometry & geometry,
                                 const ShadeOpenness & openness = {0, 0, 0, 0});
    };

    class EffectiveLayerWoven : public EffectiveLayerCommon
    {
    public:
        EffectiveLayerWoven(double width,
                            double height,
                            double thickness,
                            const FenestrationCommon::Woven::Geometry & geometry,
                            const ShadeOpenness & openness = {0, 0, 0, 0});
    };

    class EffectiveLayerUserDefined : public EffectiveLayer
    {
    public:
        EffectiveLayerUserDefined(double width,
                                  double height,
                                  double thickness,
                                  double permeabilityFactor = 0,
                                  double effectiveFrontThermalOpennessArea = 0,
                                  const ShadeOpenness & openness = {0, 0, 0, 0});

        EffectiveOpenness getEffectiveOpenness() override;
        double effectiveThickness() override;

    private:
        double m_EffectiveFrontThermalOpennessArea;
    };
}   // namespace EffectiveLayers