#ifndef WINDOWS_CALCENGINE_ISCATTERINGLAYER_HPP
#define WINDOWS_CALCENGINE_ISCATTERINGLAYER_HPP

#include <vector>

#include "CalculationProperties.hpp"

namespace FenestrationCommon
{
    enum class PropertySurface;
    enum class Side;
    enum class Scattering;
    enum class ScatteringSimple;

}   // namespace FenestrationCommon

namespace SingleLayerOptics
{
    class IScatteringLayer
    {
    public:
        virtual ~IScatteringLayer() = default;

        virtual double getPropertySimple(const double minLambda,
                                         const double maxLambda,
                                         const FenestrationCommon::PropertySurface t_Property,
                                         const FenestrationCommon::Side t_Side,
                                         const FenestrationCommon::Scattering t_Scattering,
                                         const double t_Theta = 0,
                                         const double t_Phi = 0) = 0;

        virtual std::vector<double> getWavelengths() const = 0;

        virtual double getMinLambda() const = 0;
        virtual double getMaxLambda() const = 0;

        virtual std::vector<double>
          getAbsorptanceLayers(const double minLambda,
                               const double maxLambda,
                               FenestrationCommon::Side side,
                               FenestrationCommon::ScatteringSimple scattering,
                               const double theta,
                               const double phi) = 0;

        virtual std::vector<double>
          getAbsorptanceLayersHeat(const double minLambda,
                                   const double maxLambda,
                                   FenestrationCommon::Side side,
                                   FenestrationCommon::ScatteringSimple scattering,
                                   const double theta,
                                   const double phi) = 0;

        virtual std::vector<double>
          getAbsorptanceLayersElectricity(const double minLambda,
                                          const double maxLambda,
                                          FenestrationCommon::Side side,
                                          FenestrationCommon::ScatteringSimple scattering,
                                          const double theta,
                                          const double phi) = 0;

        virtual void setCalculationProperties(const SingleLayerOptics::CalculationProperties & calcProperties) = 0;
    };

}   // namespace SingleLayerOptics


#endif   // WINDOWS_CALCENGINE_ISCATTERINGLAYER_HPP
