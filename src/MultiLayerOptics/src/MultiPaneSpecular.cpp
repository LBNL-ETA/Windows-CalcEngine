#include <cassert>
#include <algorithm>
#include <cmath>

#include "MultiPaneSpecular.hpp"
#include "AbsorptancesMultiPane.hpp"
#include "EquivalentLayerSingleComponentMW.hpp"
#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"

using namespace FenestrationCommon;
using namespace SingleLayerOptics;

namespace MultiLayerOptics
{
    ////////////////////////////////////////////////////////////////////////////////////////////
    //  CEquivalentLayerSingleComponentMWAngle
    ////////////////////////////////////////////////////////////////////////////////////////////
    CEquivalentLayerSingleComponentMWAngle::CEquivalentLayerSingleComponentMWAngle(
      const std::shared_ptr<CEquivalentLayerSingleComponentMW> & t_Layer,
      const std::shared_ptr<CAbsorptancesMultiPane> & t_Abs,
      const double t_Angle) :
        m_Layer(t_Layer),
        m_Abs(t_Abs),
        m_Angle(t_Angle)
    {}

    double CEquivalentLayerSingleComponentMWAngle::angle() const
    {
        return m_Angle;
    }

    std::shared_ptr<CEquivalentLayerSingleComponentMW>
      CEquivalentLayerSingleComponentMWAngle::layer() const
    {
        return m_Layer;
    }

    std::shared_ptr<CSeries>
      CEquivalentLayerSingleComponentMWAngle::getProperties(const Side t_Side,
                                                            const Property t_Property)
    {
        return m_Layer->getProperties(t_Property, t_Side);
    }

    std::shared_ptr<CSeries> CEquivalentLayerSingleComponentMWAngle::Abs(size_t const Index)
    {
        return m_Abs->Abs(Index);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    //  CMultiPaneSpecular
    ////////////////////////////////////////////////////////////////////////////////////////////
    CMultiPaneSpecular::CMultiPaneSpecular(
      std::vector<SingleLayerOptics::SpecularLayer> layers,
      const std::shared_ptr<FenestrationCommon::CSeries> & t_SolarRadiation) :
        m_Layers(std::move(layers)),
        m_SolarRadiation(t_SolarRadiation)
    {
        CCommonWavelengths aCommonWL;
        for(auto & layer : m_Layers)
        {
            aCommonWL.addWavelength(layer.getBandWavelengths());
        }

        // Finds combination of two wavelength sets without going outside of wavelenght range for
        // any of spectral samples.
        m_CommonWavelengths = aCommonWL.getCombinedWavelengths(Combine::Interpolate);

        m_SolarRadiation = m_SolarRadiation->interpolate(m_CommonWavelengths);

        for(auto & layer : m_Layers)
        {
            layer.setSourceData(m_SolarRadiation);
        }
    }

    CMultiPaneSpecular::CMultiPaneSpecular(std::vector<double> const & t_CommonWavelength,
                                           const std::shared_ptr<CSeries> & t_SolarRadiation,
                                           SpecularLayer & t_Layer) :
        m_CommonWavelengths(t_CommonWavelength),
        m_SolarRadiation(t_SolarRadiation)
    {
        m_SolarRadiation = m_SolarRadiation->interpolate(m_CommonWavelengths);
        addLayer(t_Layer);
    }

    void CMultiPaneSpecular::addLayer(SpecularLayer & t_Layer)
    {
        t_Layer.setSourceData(m_SolarRadiation);
        m_Layers.push_back(t_Layer);
    }

    std::unique_ptr<CMultiPaneSpecular> CMultiPaneSpecular::create(
      std::vector<SingleLayerOptics::SpecularLayer> layers,
      const std::shared_ptr<FenestrationCommon::CSeries> & t_SolarRadiation)
    {
        return std::unique_ptr<CMultiPaneSpecular>(
          new CMultiPaneSpecular(std::move(layers), t_SolarRadiation));
    }

    double CMultiPaneSpecular::getPropertySimple(
      PropertySimple t_Property, Side t_Side, Scattering t_Scattering, double t_Theta, double)
    {
        double result(0);
        const auto prop(toProperty(t_Property));
        switch(t_Scattering)
        {
            case Scattering::DirectDirect:
                result = getProperty(t_Side, prop, t_Theta, getMinLambda(), getMaxLambda());
                break;
            case Scattering::DiffuseDiffuse:
                result = getHemisphericalProperty(t_Side,
                                                  prop,
                                                  {0, 10, 20, 30, 40, 50, 60, 70, 80, 90},
                                                  getMinLambda(),
                                                  getMaxLambda());
                break;
            case Scattering::DirectDiffuse:
                result = 0;
                break;
        }

        return result;
    }

    double CMultiPaneSpecular::getMinLambda() const
    {
        return m_Layers[0].getMinLambda();
    }

    double CMultiPaneSpecular::getMaxLambda() const
    {
        return m_Layers[0].getMaxLambda();
    }

    std::vector<double> CMultiPaneSpecular::getWavelengths() const
    {
        return m_CommonWavelengths;
    }

    double CMultiPaneSpecular::getProperty(const Side t_Side,
                                           const Property t_Property,
                                           const double t_Angle,
                                           const double minLambda,
                                           const double maxLambda,
                                           const IntegrationType t_IntegrationType,
                                           double normalizationCoefficient)
    {
        CEquivalentLayerSingleComponentMWAngle & aAngularProperties = *getAngular(t_Angle);

        auto aProperties = aAngularProperties.getProperties(t_Side, t_Property);

        auto aMult = aProperties->mMult(*m_SolarRadiation);

        auto iIntegrated = aMult->integrate(t_IntegrationType, normalizationCoefficient);

        double totalProperty = iIntegrated->sum(minLambda, maxLambda);
        double totalSolar = m_SolarRadiation->integrate(t_IntegrationType, normalizationCoefficient)
                              ->sum(minLambda, maxLambda);

        assert(totalSolar > 0);

        return totalProperty / totalSolar;
    }

    double
      CMultiPaneSpecular::getHemisphericalProperty(Side t_Side,
                                                   Property t_Property,
                                                   const std::vector<double> & t_IntegrationAngles,
                                                   double minLambda,
                                                   double maxLambda,
                                                   IntegrationType t_IntegrationType,
                                                   double normalizationCoefficient)
    {
        size_t size = t_IntegrationAngles.size();
        std::shared_ptr<CSeries> aAngularProperties = std::make_shared<CSeries>();
        for(size_t i = 0; i < size; ++i)
        {
            double angle = t_IntegrationAngles[i];
            double aProperty = getProperty(t_Side,
                                           t_Property,
                                           angle,
                                           minLambda,
                                           maxLambda,
                                           t_IntegrationType,
                                           normalizationCoefficient);
            aAngularProperties->addProperty(angle, aProperty);
        }
        CHemispherical2DIntegrator aIntegrator = CHemispherical2DIntegrator(
          *aAngularProperties, t_IntegrationType, normalizationCoefficient);
        return aIntegrator.value();
    }

    double CMultiPaneSpecular::getAbsorptanceLayer(size_t index,
                                                   FenestrationCommon::Side,
                                                   FenestrationCommon::ScatteringSimple scattering,
                                                   double theta,
                                                   double)
    {
        auto result(0.0);
        if(scattering == ScatteringSimple::Direct)
        {
            result = Abs(index, theta, getMinLambda(), getMaxLambda());
        }
        return result;
    }

    double CMultiPaneSpecular::Abs(size_t const Index,
                                   const double t_Angle,
                                   const double minLambda,
                                   const double maxLambda,
                                   const IntegrationType t_IntegrationType,
                                   double normalizationCoefficient)
    {
        CEquivalentLayerSingleComponentMWAngle & aAngularProperties = *getAngular(t_Angle);
        auto aProperties = aAngularProperties.Abs(Index - 1);

        auto aMult = aProperties->mMult(*m_SolarRadiation);

        auto iIntegrated = aMult->integrate(t_IntegrationType, normalizationCoefficient);

        double totalProperty = iIntegrated->sum(minLambda, maxLambda);
        double totalSolar = m_SolarRadiation->integrate(t_IntegrationType, normalizationCoefficient)
                              ->sum(minLambda, maxLambda);

        assert(totalSolar > 0);

        return totalProperty / totalSolar;
    }

    double CMultiPaneSpecular::AbsHemispherical(size_t const Index,
                                                const std::vector<double> & t_IntegrationAngles,
                                                const double minLambda,
                                                const double maxLambda,
                                                const IntegrationType t_IntegrationType,
                                                double normalizationCoefficient)
    {
        size_t size = t_IntegrationAngles.size();
        std::shared_ptr<CSeries> aAngularProperties = std::make_shared<CSeries>();
        for(size_t i = 0; i < size; ++i)
        {
            double angle = t_IntegrationAngles[i];
            double aAbs = Abs(Index, angle, minLambda, maxLambda, t_IntegrationType);
            aAngularProperties->addProperty(angle, aAbs);
        }

        CHemispherical2DIntegrator aIntegrator = CHemispherical2DIntegrator(
          *aAngularProperties, t_IntegrationType, normalizationCoefficient);
        return aIntegrator.value();
    }

    std::shared_ptr<CEquivalentLayerSingleComponentMWAngle>
      CMultiPaneSpecular::getAngular(const double t_Angle)
    {
        std::shared_ptr<CEquivalentLayerSingleComponentMWAngle> aAngularProperties = nullptr;

        std::vector<std::shared_ptr<CEquivalentLayerSingleComponentMWAngle>>::iterator it;
        it =
          find_if(m_EquivalentAngle.begin(),
                  m_EquivalentAngle.end(),
                  [&t_Angle](const std::shared_ptr<CEquivalentLayerSingleComponentMWAngle> & obj) {
                      return std::abs(obj->angle() - t_Angle) < 1e-6;
                  });

        if(it != m_EquivalentAngle.end())
        {
            aAngularProperties = (*it);
        }
        else
        {
            aAngularProperties = createNewAngular(t_Angle);
        }

        return aAngularProperties;
    }

    std::shared_ptr<CEquivalentLayerSingleComponentMWAngle>
      CMultiPaneSpecular::createNewAngular(const double t_Angle)
    {
        // Create direction for specular. It is irrelevant what is Phi angle and it is chosen to be
        // zero in this case
        CBeamDirection aDirection = CBeamDirection(t_Angle, 0);
        std::shared_ptr<CEquivalentLayerSingleComponentMW> aEqLayer = nullptr;
        std::shared_ptr<CAbsorptancesMultiPane> aAbs = nullptr;
        for(size_t i = 0; i < m_Layers.size(); ++i)
        {
            std::vector<double> wl = m_Layers[i].getBandWavelengths();
            std::vector<double> Tv = m_Layers[i].T_dir_dir_band(Side::Front, aDirection);
            std::vector<double> Rfv = m_Layers[i].R_dir_dir_band(Side::Front, aDirection);
            std::vector<double> Rbv = m_Layers[i].R_dir_dir_band(Side::Back, aDirection);
            std::shared_ptr<CSeries> T = std::make_shared<CSeries>();
            std::shared_ptr<CSeries> Rf = std::make_shared<CSeries>();
            std::shared_ptr<CSeries> Rb = std::make_shared<CSeries>();
            for(size_t j = 0; j < wl.size(); ++j)
            {
                T->addProperty(wl[j], Tv[j]);
                Rf->addProperty(wl[j], Rfv[j]);
                Rb->addProperty(wl[j], Rbv[j]);
            }
            T = T->interpolate(m_CommonWavelengths);
            Rf = Rf->interpolate(m_CommonWavelengths);
            Rb = Rb->interpolate(m_CommonWavelengths);
            if(i == 0)
            {
                aEqLayer = std::make_shared<CEquivalentLayerSingleComponentMW>(T, T, Rf, Rb);
                aAbs = std::make_shared<CAbsorptancesMultiPane>(T, Rf, Rb);
            }
            else
            {
                assert(aEqLayer != nullptr);
                assert(aAbs != nullptr);
                aEqLayer->addLayer(T, T, Rf, Rb);
                aAbs->addLayer(T, Rf, Rb);
            }
        }
        assert(aEqLayer != nullptr);
        assert(aAbs != nullptr);

        std::shared_ptr<CEquivalentLayerSingleComponentMWAngle> newLayer =
          std::make_shared<CEquivalentLayerSingleComponentMWAngle>(aEqLayer, aAbs, t_Angle);

        m_EquivalentAngle.push_back(newLayer);

        return newLayer;
    }


}   // namespace MultiLayerOptics
