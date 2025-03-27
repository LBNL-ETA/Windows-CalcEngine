
#include <cmath>
#include <vector>
#include <memory>
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <iostream>

#include "IGU.hpp"
#include "IGUSolidLayer.hpp"
#include "IGUGapLayer.hpp"
#include "Surface.hpp"
#include "IGUSolidDeflection.hpp"
#include "IGUVentilatedGapLayer.hpp"
#include "BaseShade.hpp"
#include "Environment.hpp"
#include "WCECommon.hpp"

using FenestrationCommon::Side;


namespace Tarcog::ISO15099
{
    CIGU::CIGU(double const t_Width, double const t_Height, double const t_Tilt) :
        m_Width(t_Width), m_Height(t_Height), m_Tilt(t_Tilt)
    {}

    CIGU::CIGU(CIGU const & t_IGU)
    {
        operator=(t_IGU);
    }

    CIGU & CIGU::operator=(CIGU const & t_IGU)
    {
        m_Width = t_IGU.m_Width;
        m_Height = t_IGU.m_Height;
        m_Tilt = t_IGU.m_Tilt;
        m_Layers.clear();
        for(auto & layer : t_IGU.m_Layers)
        {
            const auto aLayer{layer->clone()};
            addLayer(aLayer);
        }

        m_DeflectionFromE1300Curves = t_IGU.m_DeflectionFromE1300Curves;

        return *this;
    }

    CIGU::~CIGU()
    {
        auto layers = getSolidLayers();
        for(std::shared_ptr<CBaseLayer> layer : layers)
        {
            layer->tearDownConnections();
        }
    }

    void CIGU::addLayer(const std::shared_ptr<CBaseLayer> & t_Layer)
    {
        // pushes only solid layers to array. Gap layers are connected via linked list
        // In case this is first layer then it must be a solid layer in order to create IGU
        if(getNumOfLayers() == 0)
        {
            if(std::dynamic_pointer_cast<CIGUSolidLayer>(t_Layer) != nullptr)
            {
                m_Layers.push_back(t_Layer);
            }
            else
            {
                throw std::runtime_error("First inserted layer must be a solid layer.");
            }
        }
        else
        {
            auto lastLayer = m_Layers.back();
            if(std::dynamic_pointer_cast<CIGUSolidLayer>(t_Layer)
               != std::dynamic_pointer_cast<CIGUSolidLayer>(lastLayer))
            {
                m_Layers.push_back(t_Layer);
                lastLayer->connectToBackSide(t_Layer);
            }
            else
            {
                throw std::runtime_error(
                  "Two adjecent layers in IGU cannot be of same type. "
                  "IGU must be constructed of array of solid and gap layers.");
            }
        }

        checkForLayerUpgrades(t_Layer);

        t_Layer->setTilt(m_Tilt);
        t_Layer->setWidth(m_Width);
        t_Layer->setHeight(m_Height);
    }

    void CIGU::addLayers(const std::initializer_list<std::shared_ptr<CBaseLayer>> & layers)
    {
        for(const auto & layer : layers)
        {
            addLayer(layer);
        }
    }

    void CIGU::setTilt(double const t_Tilt)
    {
        for(auto & layer : m_Layers)
        {
            layer->setTilt(t_Tilt);
        }
        m_Tilt = t_Tilt;

        if(m_DeflectionFromE1300Curves.has_value())
        {
            m_DeflectionFromE1300Curves->setIGUTilt(t_Tilt);
        }
    }

    void CIGU::setWidth(double const t_Width)
    {
        for(auto & layer : m_Layers)
        {
            layer->setWidth(t_Width);
        }
        m_Width = t_Width;

        if(m_DeflectionFromE1300Curves.has_value())
        {
            m_DeflectionFromE1300Curves->setDimensions(m_Width, m_Height);
        }
    }

    void CIGU::setHeight(double const t_Height)
    {
        for(auto & layer : m_Layers)
        {
            layer->setHeight(t_Height);
        }
        m_Height = t_Height;

        if(m_DeflectionFromE1300Curves.has_value())
        {
            m_DeflectionFromE1300Curves->setDimensions(m_Width, m_Height);
        }
    }

    void CIGU::setSolarRadiation(double const t_SolarRadiation) const
    {
        auto layers = getSolidLayers();
        for(auto & layer : layers)
        {
            layer->setSolarRadiation(t_SolarRadiation);
        }
    }

    std::shared_ptr<CBaseLayer> CIGU::getEnvironment(Environment t_Environment) const
    {
        std::shared_ptr<CBaseLayer> aLayer = nullptr;
        switch(t_Environment)
        {
            case Environment::Indoor:
                aLayer = m_Layers.back();
                break;
            case Environment::Outdoor:
                aLayer = m_Layers.front();
                break;
        }
        return aLayer;
    }

    std::vector<double> CIGU::getState() const
    {
        std::vector<double> aState;

        auto layers = getSolidLayers();
        for(auto & layer : layers)
        {
            // State must be filled in this exact order.
            aState.push_back(layer->surfaceTemperature(Side::Front));
            aState.push_back(layer->J(Side::Front));
            aState.push_back(layer->J(Side::Back));
            aState.push_back(layer->surfaceTemperature(Side::Back));
        }

        return aState;
    }

    void CIGU::setState(const std::vector<double> & t_State) const
    {
        size_t i = 0;
        auto layers = getSolidLayers();
        for(const auto & aLayer : layers)
        {
            const auto Tf = t_State[4 * i];
            const auto Jf = t_State[4 * i + 1];
            const auto Jb = t_State[4 * i + 2];
            const auto Tb = t_State[4 * i + 3];
            aLayer->setLayerState(Tf, Tb, Jf, Jb);
            ++i;
        }
    }

    std::vector<double> CIGU::getTemperatures() const
    {
        std::vector<double> aTemperatures;

        auto layers = getSolidLayers();
        for(auto & layer : layers)
        {
            FenestrationCommon::EnumSide sides;
            for(auto aSide : sides)
            {
                aTemperatures.push_back(layer->surfaceTemperature(aSide));
            }
        }

        return aTemperatures;
    }

    std::vector<double> CIGU::getRadiosities() const
    {
        std::vector<double> aRadiosities;

        auto layers = getSolidLayers();
        for(auto & layer : layers)
        {
            FenestrationCommon::EnumSide sides;
            for(auto aSide : sides)
            {
                aRadiosities.push_back(layer->J(aSide));
            }
        }

        return aRadiosities;
    }

    std::vector<double> CIGU::getMaxLayerDeflections() const
    {
        std::vector<double> aMaxDeflections;

        auto layers = getSolidLayers();
        for(auto & layer : layers)
        {
            aMaxDeflections.push_back(layer->getMaxDeflection());
        }

        return aMaxDeflections;
    }

    std::vector<double> CIGU::getMeanLayerDeflections() const
    {
        std::vector<double> aMeanDeflections;

        auto layers = getSolidLayers();
        for(auto & layer : layers)
        {
            aMeanDeflections.push_back(layer->getMeanDeflection());
        }

        return aMeanDeflections;
    }

    std::vector<double> CIGU::getMaxGapWidth() const
    {
        std::vector<double> aMaxWidths;
        auto gaps = getGapLayers();
        for(auto const & layer : gaps)
        {
            aMaxWidths.push_back(layer->getMaxDeflection());
        }

        return aMaxWidths;
    }

    std::vector<double> CIGU::getMeanGapWidth() const
    {
        std::vector<double> aMeanWidths;

        auto gaps = getGapLayers();
        for(auto const & layer : gaps)
        {
            aMeanWidths.push_back(layer->getMeanDeflection());
        }

        return aMeanWidths;
    }

    std::vector<double> CIGU::getGapPressures() const
    {
        std::vector<double> aPressures;

        auto gaps = getGapLayers();
        for(auto const & layer : gaps)
        {
            aPressures.push_back(layer->getPressure());
        }

        return aPressures;
    }

    std::vector<double> CIGU::getPanesLoad()
    {
        std::vector<double> paneLoad(getSolidLayers().size());

        if(m_DeflectionFromE1300Curves.has_value())
        {
            paneLoad = m_DeflectionFromE1300Curves->results().paneLoad;
        }

        return paneLoad;
    }

    double CIGU::getThickness() const
    {
        auto totalWidth = 0.0;

        for(auto & layer : m_Layers)
        {
            totalWidth += layer->getThickness();
        }

        return totalWidth;
    }

    double CIGU::getTilt() const
    {
        return m_Tilt;
    }

    double CIGU::getWidth() const
    {
        return m_Width;
    }

    double CIGU::getHeight() const
    {
        return m_Height;
    }

    size_t CIGU::getNumOfLayers() const
    {
        return (m_Layers.size() + 1) / 2;
    }

    double CIGU::getVentilationFlow(const Environment t_Environment) const
    {
        // This is asking flow from the gap that is connected to the one of the environments.
        const auto size = m_Layers.size();
        auto result{0.0};
        if(size > 1u)
        {
            // This is important in order to get correct gap numbering. It will return the gap
            // that is connected with the environment.
            const std::map<Environment, size_t> envLayer = {{Environment::Indoor, size - 2},
                                                            {Environment::Outdoor, 1}};

            // Need to make sure that solid layer is actually permeable as well
            const std::map<Environment, size_t> solidLayerIndex = {{Environment::Indoor, size - 1},
                                                                   {Environment::Outdoor, 0}};

            if(m_Layers[solidLayerIndex.at(t_Environment)]->isPermeable())
            {
                result = m_Layers[envLayer.at(t_Environment)]->getGainFlow();
            }
        }
        return result;
    }

    void CIGU::setInitialGuess(std::vector<double> const & t_Guess) const
    {
        if(2 * getNumOfLayers() != t_Guess.size())
        {
            std::cout << "Number of temperatures in initial guess cannot fit number of layers."
                         "Program will use initial guess instead"
                      << std::endl;
        }
        else
        {
            size_t Index = 0;
            auto layers = getSolidLayers();
            for(auto & aLayer : layers)
            {
                FenestrationCommon::EnumSide sides;
                for(auto aSide : sides)
                {
                    aLayer->initializeStart(aSide, t_Guess[Index]);
                    ++Index;
                }
            }
        }
    }

    void CIGU::setDeflectionProperties(const double t_Tini,
                                       const double t_Pini,
                                       const double t_InsidePressure,
                                       const double t_OutsidePressure)
    {
        // m_DeflectionFromE1300Curves must be set when surfaces are in non-deflected state.
        // Since user might have called IGU previously with different deflection properties
        resetSurfaceDeflections();
        std::vector<Deflection::LayerData> layerData;
        auto layers = getSolidLayers();
        for(auto & layer : layers)
        {
            layerData.emplace_back(layer->getThickness(), layer->density(), layer->youngsModulus());
        }

        std::vector<Deflection::GapData> gapData;
        auto gaps = getGapLayers();
        for(auto const & gap : gaps)
        {
            gap->setSealedGapProperties(t_Tini, t_Pini);
            gapData.emplace_back(gap->getThickness(), t_Tini, t_Pini);
        }

        m_DeflectionFromE1300Curves =
          Deflection::DeflectionE1300(m_Width, m_Height, layerData, gapData);

        m_DeflectionFromE1300Curves->setIGUTilt(m_Tilt);
        m_DeflectionFromE1300Curves->setInteriorPressure(t_InsidePressure);
        m_DeflectionFromE1300Curves->setExteriorPressure(t_OutsidePressure);

        if(m_DeflectionAppliedLoad.size() == layerData.size())
        {
            m_DeflectionFromE1300Curves->setAppliedLoad(m_DeflectionAppliedLoad);
        }
    }

    // Purpose of this function is not to convert solid layer twice since it is already
    // converted for the measured deflection
    CIGUSolidLayerDeflection convertToMeasuredDeflectionLayer(const CIGUSolidLayer & layer)
    {
        if(auto deflectionLayer = dynamic_cast<const CIGUDeflectionMeasured *>(&layer))
        {
            return *deflectionLayer;
        }
        else
        {
            return CIGUSolidLayerDeflection(layer);
        }
    }

    void CIGU::setDeflectionProperties(std::vector<double> const & t_MeasuredDeflections)
    {
        // In case user sets the deflection properties as pressure and temperature and then
        // reset this back to measured deflection should delete calculator for the deflection
        // from E1300 curves.
        m_DeflectionFromE1300Curves = std::nullopt;

        if(t_MeasuredDeflections.size() != getNumOfLayers() - 1)
        {
            throw std::runtime_error(
              "Number of measured deflection values must be equal to number of gaps.");
        }

        auto deflectionRatio = Ldmean() / Ldmax();
        auto LDefMax = calculateLDefMax(t_MeasuredDeflections);

        for(auto i = 0u; i < getNumOfLayers(); ++i)
        {
            auto LDefNMean = deflectionRatio * LDefMax[i];
            auto aLayer = getSolidLayers()[i];
            if(dynamic_cast<CIGUDeflectionMeasured *>(aLayer.get()) == nullptr)
            {
                auto aDefLayer = std::make_shared<CIGUSolidLayerDeflection>(*aLayer);
                aDefLayer =
                  std::make_shared<CIGUDeflectionMeasured>(aDefLayer, LDefNMean, LDefMax[i]);
                replaceLayer(aLayer, aDefLayer);
            }
        }
    }

    void CIGU::updateDeflectionState()
    {
        if(m_DeflectionFromE1300Curves.has_value())
        {
            const auto gapLayers{getGapLayers()};
            std::vector<double> gapTemperatures(gapLayers.size());
            for(size_t i = 0u; i < gapTemperatures.size(); ++i)
            {
                gapTemperatures[i] = gapLayers[i]->averageLayerTemperature();
            }
            m_DeflectionFromE1300Curves->setLoadTemperatures(gapTemperatures);

            auto deflectionResults{m_DeflectionFromE1300Curves->results()};

            // This is borrowed from Timschenko. It will be used till E1300 calculations are
            // actually doing this.
            const auto deflectionRatio = Ldmean() / Ldmax();

            auto solidLayers{getSolidLayers()};

            assert(deflectionResults.deflection.size() == solidLayers.size());

            for(size_t i = 0u; i < deflectionResults.deflection.size(); ++i)
            {
                auto def{deflectionResults.deflection[i]};
                solidLayers[i]->applyDeflection(deflectionRatio * def, def);
            }
        }
    }

    void CIGU::replaceLayer(const std::shared_ptr<CBaseLayer> & t_Original,
                            const std::shared_ptr<CBaseLayer> & t_Replacement)
    {
        size_t index = static_cast<size_t>(find(m_Layers.begin(), m_Layers.end(), t_Original)
                                           - m_Layers.begin());
        m_Layers[index] = t_Replacement;
        if(index > 0)
        {
            m_Layers[index - 1]->connectToBackSide(t_Replacement);
        }
        if(index < m_Layers.size() - 1)
        {
            t_Replacement->connectToBackSide(m_Layers[index + 1]);
        }
    }

    void CIGU::checkForLayerUpgrades(const std::shared_ptr<CBaseLayer> & t_Layer)
    {
        if(std::dynamic_pointer_cast<CIGUShadeLayer>(t_Layer) != nullptr)
        {
            if(std::dynamic_pointer_cast<CIGUGapLayer>(t_Layer->getPreviousLayer()) != nullptr
               && std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(t_Layer->getPreviousLayer())
                    == nullptr)
            {
                auto newLayer = std::make_shared<CIGUVentilatedGapLayer>(
                  std::dynamic_pointer_cast<CIGUGapLayer>(t_Layer->getPreviousLayer()));
                replaceLayer(std::dynamic_pointer_cast<CIGUGapLayer>(t_Layer->getPreviousLayer()),
                             newLayer);
            }
        }
        if(std::dynamic_pointer_cast<CIGUGapLayer>(t_Layer) != nullptr
           && std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(t_Layer) == nullptr)
        {
            if(std::dynamic_pointer_cast<CIGUShadeLayer>(t_Layer->getPreviousLayer()) != nullptr)
            {
                auto newLayer = std::make_shared<CIGUVentilatedGapLayer>(
                  std::dynamic_pointer_cast<CIGUGapLayer>(t_Layer));
                replaceLayer(std::dynamic_pointer_cast<CIGUGapLayer>(t_Layer), newLayer);
            }
        }
    }

    double CIGU::Ldmean() const
    {
        using ConstantsData::WCE_PI;

        auto coeff = 16 / (pow(WCE_PI, 6));
        auto totalSum = 0.0;
        for(auto m = 1; m <= 5; m += 2)
        {
            for(auto n = 1; n <= 5; n += 2)
            {
                auto nomin = 4.0;
                auto denom = m * m * n * n * WCE_PI * WCE_PI
                             * pow(pow(m / m_Width, 2) + pow(n / m_Height, 2), 2);
                totalSum += nomin / denom;
            }
        }
        return coeff * totalSum;
    }

    double CIGU::Ldmax() const
    {
        using ConstantsData::WCE_PI;

        auto coeff = 16 / (pow(WCE_PI, 6));
        auto totalSum = 0.0;
        for(auto m = 1; m <= 5; m += 2)
        {
            for(auto n = 1; n <= 5; n += 2)
            {
                auto nomin = sin(m * WCE_PI / 2) * sin(n * WCE_PI / 2);
                auto denom = m * n * pow(pow(m / m_Width, 2) + pow(n / m_Height, 2), 2);
                totalSum += nomin / denom;
            }
        }
        return coeff * totalSum;
    }

    std::vector<std::shared_ptr<CIGUSolidLayer>> CIGU::getSolidLayers() const
    {
        std::vector<std::shared_ptr<CIGUSolidLayer>> aVect;
        for(auto const & aLayer : m_Layers)
        {
            if(std::dynamic_pointer_cast<CIGUSolidLayer>(aLayer) != nullptr)
            {
                aVect.push_back(std::dynamic_pointer_cast<CIGUSolidLayer>(aLayer));
            }
        }
        return aVect;
    }

    std::vector<std::shared_ptr<CIGUGapLayer>> CIGU::getGapLayers() const
    {
        std::vector<std::shared_ptr<CIGUGapLayer>> aVect;
        for(auto const & aLayer : m_Layers)
        {
            if(std::dynamic_pointer_cast<CIGUGapLayer>(aLayer) != nullptr)
            {
                aVect.push_back(std::dynamic_pointer_cast<CIGUGapLayer>(aLayer));
            }
        }
        return aVect;
    }

    std::vector<std::shared_ptr<CBaseLayer>> CIGU::getLayers() const
    {
        return m_Layers;
    }

    void CIGU::setAbsorptances(const std::vector<double> & absorptances, double solarRadiation)
    {
        auto solidLayers = getSolidLayers();
        if(solidLayers.size() != absorptances.size())
        {
            throw std::runtime_error(
              "Number of absorptances does not match number of solid layers.");
        }
        for(size_t i = 0; i < solidLayers.size(); ++i)
        {
            solidLayers[i]->setSolarHeatGain(absorptances[i], solarRadiation);
        }
    }

    void CIGU::clearDeflection()
    {
        m_DeflectionFromE1300Curves = std::nullopt;
    }

    void CIGU::setAppliedLoad(std::vector<double> t_AppliedLoad)
    {
        m_DeflectionAppliedLoad = t_AppliedLoad;
        if(m_DeflectionFromE1300Curves.has_value())
        {
            m_DeflectionFromE1300Curves->setAppliedLoad(std::move(t_AppliedLoad));
        }
    }

    void CIGU::precalculateLayerStates()
    {
        for(auto & layer : m_Layers)
        {
            layer->precalculateState();
        }
    }
    void CIGU::setSolidLayerConductivites(
      const std::vector<double> & t_SolidLayerThermalConductivities)
    {
        auto solidLayer = getSolidLayers();

        if(t_SolidLayerThermalConductivities.size() != getSolidLayers().size())
        {
            throw std::runtime_error(
              "Number of solid layer thermal conductivities does not match number of solid layers.");
        }

        for(size_t i = 0; i < getSolidLayers().size(); ++i)
        {
            solidLayer[i]->setConductivity(t_SolidLayerThermalConductivities[i]);
        }
    }

    void CIGU::setSolidLayerConductivity(size_t t_LayerIndex,
                                                double t_SolidLayerThermalConductivity)
    {
        auto solidLayer = getSolidLayers();

        if(t_LayerIndex >= getSolidLayers().size())
        {
            throw std::runtime_error(
              "Layer index is out of range for solid layer thermal conductivities.");
        }

        solidLayer[t_LayerIndex]->setConductivity(t_SolidLayerThermalConductivity);
    }

    double
      CIGU::calculateDeflectionNumerator(const std::vector<double> & t_MeasuredDeflections) const
    {
        auto numerator = 0.0;
        for(size_t i = 0; i < t_MeasuredDeflections.size(); ++i)
        {
            auto SumL = 0.0;
            for(auto j = i; j < t_MeasuredDeflections.size(); ++j)
            {
                SumL += getGapLayers()[j]->getThickness() - t_MeasuredDeflections[j];
            }
            auto aDefLayer = convertToMeasuredDeflectionLayer(*getSolidLayers()[i]);
            numerator += SumL * aDefLayer.flexuralRigidity();
        }
        return numerator;
    }

    double CIGU::calculateDeflectionDenominator() const
    {
        auto denominator = 0.0;
        for(auto i = 0u; i < getSolidLayers().size(); ++i)
        {
            auto aDefLayer = CIGUSolidLayerDeflection(*getSolidLayers()[i]);
            denominator += aDefLayer.flexuralRigidity();
        }
        return denominator;
    }

    std::vector<double>
      CIGU::calculateLDefMax(const std::vector<double> & t_MeasuredDeflections) const
    {
        auto nominator = calculateDeflectionNumerator(t_MeasuredDeflections);
        auto denominator = calculateDeflectionDenominator();

        auto LDefNMax = nominator / denominator;

        std::vector<double> LDefMax;
        LDefMax.push_back(LDefNMax);
        for(auto i = getNumOfLayers() - 1; i > 0; --i)
        {
            LDefNMax =
              t_MeasuredDeflections[i - 1] - getGapLayers()[i - 1]->getThickness() + LDefNMax;
            LDefMax.insert(LDefMax.begin(), LDefNMax);
        }
        return LDefMax;
    }

    void CIGU::resetSurfaceDeflections()
    {
        for(auto const & aLayer : m_Layers)
        {
            if(std::dynamic_pointer_cast<CIGUSolidLayer>(aLayer) != nullptr)
            {
                std::dynamic_pointer_cast<CIGUSolidLayer>(aLayer)->applyDeflection(0, 0);
            }
        }
    }

}   // namespace Tarcog::ISO15099
