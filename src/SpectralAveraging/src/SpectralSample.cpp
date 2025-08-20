#include <stdexcept>
#include <mutex>

#include <WCECommon.hpp>

#include "SpectralSample.hpp"
#include "SpectralSampleData.hpp"

std::mutex spectralSampleMutex;

using namespace FenestrationCommon;

namespace SpectralAveraging
{
    //////////////////////////////////////////////////////////////////////////////////////
    ////  CSample
    //////////////////////////////////////////////////////////////////////////////////////

    CSample::CSample(const CSeries & t_SourceData) :
        m_SourceData(t_SourceData), m_WavelengthSet(WavelengthSet::Data), m_StateCalculated(false)
    {
        CSample::reset();
    }

    CSample::CSample() : m_WavelengthSet(WavelengthSet::Data), m_StateCalculated(false)
    {
        CSample::reset();
    }

    CSample & CSample::operator=(const CSample & t_Sample)
    {
        if(this == &t_Sample)
        {
            return *this;
        }

        m_StateCalculated = t_Sample.m_StateCalculated;
        m_WavelengthSet = t_Sample.m_WavelengthSet;
        m_IncomingSource = t_Sample.m_IncomingSource;
        for(const auto & prop : allProperties())
        {
            for(const auto & side : allSides())
            {
                m_EnergySource[{prop, side}] = t_Sample.m_EnergySource.at({prop, side});
            }
        }

        return *this;
    }

    CSample::CSample(const CSample & t_Sample)
    {
        operator=(t_Sample);
    }

    CSeries & CSample::getSourceData()
    {
        calculateState(IntegrationType::Trapezoidal,
                       1);   // must interpolate data to same wavelengths
        return m_SourceData;
    }

    void CSample::setSourceData(const CSeries & t_SourceData)
    {
        m_SourceData = t_SourceData;
        reset();
    }

    void CSample::setDetectorData(const CSeries & t_DetectorData)
    {
        m_DetectorData = t_DetectorData;
        reset();
    }

    void CSample::assignDetectorAndWavelengths(std::shared_ptr<CSample> const & t_Sample)
    {
        m_DetectorData = t_Sample->m_DetectorData;
        m_Wavelengths = t_Sample->m_Wavelengths;
        m_WavelengthSet = t_Sample->m_WavelengthSet;
    }

    void CSample::setWavelengths(WavelengthSet const t_WavelengthSet,
                                 const std::vector<double> & t_Wavelenghts)
    {
        m_WavelengthSet = t_WavelengthSet;
        switch(t_WavelengthSet)
        {
            case WavelengthSet::Custom:
                m_Wavelengths = t_Wavelenghts;
                break;
            case WavelengthSet::Source:
                if(m_SourceData.size() == 0)
                {
                    throw std::runtime_error(
                      "Cannot extract wavelenghts from source. Source is empty.");
                }
                m_Wavelengths = m_SourceData.getXArray();
                break;
            case WavelengthSet::Data:
                m_Wavelengths = getWavelengthsFromSample();
                break;
            default:
                throw std::runtime_error("Incorrect definition of wavelength set source.");
                break;
        }
        reset();
    }

    double CSample::getEnergy(double const minLambda,
                              double const maxLambda,
                              Property const t_Property,
                              Side const t_Side)
    {
        calculateState(IntegrationType::Trapezoidal, 1);
        return m_EnergySource.at({t_Property, t_Side}).sum(minLambda, maxLambda);
    }

    std::vector<double> CSample::getWavelengths() const
    {
        return m_Wavelengths;
    }

    double CSample::getProperty(double const minLambda,
                                double const maxLambda,
                                Property const t_Property,
                                Side const t_Side)
    {
        calculateState(IntegrationType::Trapezoidal, 1);
        auto Prop = 0.0;
        // Incoming energy can be calculated only if user has defined incoming source.
        // Otherwise just assume zero property.
        if(m_IncomingSource.size() > 0)
        {
            auto incomingEnergy = m_IncomingSource.sum(minLambda, maxLambda);
            double propertyEnergy =
              m_EnergySource.at({t_Property, t_Side}).sum(minLambda, maxLambda);
            Prop = propertyEnergy / incomingEnergy;
        }
        return Prop;
    }

    CSeries &
      CSample::getEnergyProperties(const Property t_Property, const Side t_Side)
    {
        calculateState(IntegrationType::Trapezoidal, 1);
        return m_EnergySource.at({t_Property, t_Side});
    }

    size_t CSample::getBandSize() const
    {
        return m_Wavelengths.size();
    }

    void CSample::reset()
    {
        m_StateCalculated = false;
        m_IncomingSource = CSeries();
        for(const auto & prop : allProperties())
        {
            for(const auto & side : allSides())
            {
                m_EnergySource[{prop, side}] = CSeries();
            }
        }
    }

    void CSample::calculateState(FenestrationCommon::IntegrationType integrator,
                                 double normalizationCoefficient)
    {
        if(!m_StateCalculated)
        {
            if(m_WavelengthSet != WavelengthSet::Custom)
            {
                setWavelengths(m_WavelengthSet);
            }

            // In case source data are set then apply solar radiation to the calculations.
            // Otherwise, just use measured data.
            if(m_SourceData.size() > 0)
            {
                m_IncomingSource = m_SourceData.interpolate(m_Wavelengths);


                if(m_DetectorData.size() > 0)
                {
                    const auto interpolatedDetector = m_DetectorData.interpolate(m_Wavelengths);
                    m_IncomingSource = m_IncomingSource * interpolatedDetector;
                }

                calculateProperties(integrator, normalizationCoefficient);

                m_IncomingSource = m_IncomingSource.integrate(integrator, normalizationCoefficient);
                for(const auto & prop : allProperties())
                {
                    for(const auto & side : allSides())
                    {
                        m_EnergySource[{prop, side}] =
                          m_EnergySource.at({prop, side})
                            .integrate(integrator, normalizationCoefficient);
                    }
                }

                m_StateCalculated = true;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////
    ////  CSpectralSample
    //////////////////////////////////////////////////////////////////////////////////////

    CSpectralSample::CSpectralSample(std::shared_ptr<CSpectralSampleData> const & t_SampleData,
                                     const CSeries & t_SourceData) :
        CSample(t_SourceData), m_SampleData(t_SampleData)
    {
        if(t_SampleData == nullptr)
        {
            throw std::runtime_error("Sample must have measured data.");
        }

        for(const auto & prop : allProperties())
        {
            for(const auto & side : allSides())
            {
                for(const auto & mt : allMeasurements)
                {
                    m_Property[key(prop, side, mt)] = CSeries();
                }
            }
        }
    }

    CSpectralSample::CSpectralSample(std::shared_ptr<CSpectralSampleData> const & t_SampleData) :
        CSample(), m_SampleData(t_SampleData)
    {
        if(t_SampleData == nullptr)
        {
            throw std::runtime_error("Sample must have measured data.");
        }

        for(const auto & prop : allProperties())
        {
            for(const auto & side : allSides())
            {
                for(const auto & mt : allMeasurements)
                {
                    m_Property[{prop, side, mt}] = CSeries();
                }
            }
        }
    }

    std::shared_ptr<CSpectralSampleData> CSpectralSample::getMeasuredData()
    {
        calculateState(IntegrationType::Trapezoidal,
                       1);   // Interpolation is needed before returning the data
        return m_SampleData;
    }

    std::vector<double> CSpectralSample::getWavelengthsFromSample() const
    {
        return m_SampleData->getWavelengths();
    }

    CSeries CSpectralSample::getWavelengthsProperty(const Property t_Property,
                                                    const Side t_Side)
    {
        std::lock_guard lock(spectralSampleMutex);
        if(!m_StateCalculated)
        {
            calculateState(IntegrationType::Trapezoidal, 1);
        }

        return m_Property.at(key(t_Property, t_Side, MeasurementType::Direct));
    }

    void CSpectralSample::calculateProperties(IntegrationType integrator,
                                              double normalizationCoefficient)
    {
        std::ignore = integrator;
        std::ignore = normalizationCoefficient;
        for(const auto & prop : allProperties())
        {
            for(const auto & side : allSides())
            {
                for(const auto & mt : allMeasurements)
                {
                    m_Property[key(prop, side, mt)] = m_SampleData->properties(prop, side);
                    // No need to do interpolation if wavelength set is already from the data.
                    if(m_WavelengthSet != WavelengthSet::Data)
                    {
                        m_Property[key(prop, side, mt)] =
                          m_Property[key(prop, side, mt)].interpolate(m_Wavelengths);
                    }
                }
            }
        }

        if(m_WavelengthSet == WavelengthSet::Data)
        {
            m_Wavelengths = getWavelengthsFromSample();
        }

        // Calculation of energy balances
        for(const auto & prop : allProperties())
        {
            for(const auto & side : allSides())
            {
                for(const auto & mt : allMeasurements)
                {
                    m_EnergySource[{prop, side}] =
                      m_Property.at(key(prop, side, mt)) * m_IncomingSource;
                }
            }
        }
    }

    void CSpectralSample::calculateState(IntegrationType integrator,
                                         double normalizationCoefficient)
    {
        CSample::calculateState(integrator, normalizationCoefficient);

        if(m_SourceData.size() == 0)
        {
            for(const auto & prop : allProperties())
            {
                for(const auto & side : allSides())
                {
                    for(const auto & mt : allMeasurements)
                    {
                        m_Property[{prop, side, mt}] = m_SampleData->properties(prop, side);
                    }
                }
            }

            m_StateCalculated = true;
        }
    }

    void CSpectralSample::cutExtraData(const double minLambda, const double maxLambda)
    {
        m_SampleData->cutExtraData(minLambda, maxLambda);
    }

    void CSpectralSample::Flipped(bool flipped)
    {
        m_SampleData->Flipped(flipped);
    }

    FenestrationCommon::Limits CSpectralSample::getWavelengthLimits() const
    {
        return m_SampleData->getWavelengthLimits();
    }

    /////////////////////////////////////////////////////////////////////////////////////
    /// CPhotovoltaicSample
    /////////////////////////////////////////////////////////////////////////////////////

    CPhotovoltaicSample::CPhotovoltaicSample(
      const std::shared_ptr<PhotovoltaicSampleData> & t_PhotovoltaicData,
      const FenestrationCommon::CSeries & t_SourceData) :
        CSpectralSample(t_PhotovoltaicData, t_SourceData),
        m_JcsPrime{{Side::Front, CSeries()}, {Side::Back, CSeries()}}
    {}

    void CPhotovoltaicSample::calculateState(FenestrationCommon::IntegrationType integrator,
                                             double normalizationCoefficient)
    {
        CSpectralSample::calculateProperties(integrator, normalizationCoefficient);
        for(const auto & side : allSides())
        {
            CSeries eqe{getSample()->eqe(side)};
            eqe = eqe.interpolate(m_Wavelengths);
            CSeries jscPrime;
            for(auto i = 0u; i < m_Wavelengths.size(); ++i)
            {
                const double pceVal = jscPrimeCalc(m_Wavelengths[i], eqe[i].value());
                jscPrime.addProperty(m_Wavelengths[i], pceVal);
            }
            m_JcsPrime[side] = jscPrime;
        }
    }

    PhotovoltaicSampleData * CPhotovoltaicSample::getSample() const
    {
        return dynamic_cast<PhotovoltaicSampleData *>(m_SampleData.get());
    }

    double CPhotovoltaicSample::jscPrimeCalc(double wavelength, double eqe)
    {
        double constexpr microMeterToMeter{1e-6};
        return eqe * wavelength * ConstantsData::ELECTRON_CHARGE * microMeterToMeter
               / (ConstantsData::SPEEDOFLIGHT * ConstantsData::PLANKCONSTANT);
    }

    CSeries CPhotovoltaicSample::jscPrime(const Side side)
    {
        calculateState(IntegrationType::Trapezoidal, 1);
        return m_JcsPrime.at(side);
    }
}   // namespace SpectralAveraging
