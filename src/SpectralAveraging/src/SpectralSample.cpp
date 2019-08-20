#include <stdexcept>
#include <cassert>

#include "SpectralSample.hpp"
#include "MeasuredSampleData.hpp"
#include "WCECommon.hpp"


using namespace FenestrationCommon;

namespace SpectralAveraging
{
    //////////////////////////////////////////////////////////////////////////////////////
    ////  CSample
    //////////////////////////////////////////////////////////////////////////////////////

    CSample::CSample(std::shared_ptr<CSeries> const & t_SourceData,
                     IntegrationType integrationType,
                     double t_NormalizationCoefficient) :
        m_SourceData(t_SourceData),
        m_DetectorData(nullptr),
        m_WavelengthSet(WavelengthSet::Data),
        m_IntegrationType(integrationType),
        m_NormalizationCoefficient(t_NormalizationCoefficient),
        m_StateCalculated(false)
    {
        CSample::reset();
    }

    CSample::CSample() :
        m_SourceData(nullptr),
        m_DetectorData(nullptr),
        m_WavelengthSet(WavelengthSet::Data),
        m_IntegrationType(IntegrationType::Trapezoidal),
        m_NormalizationCoefficient(1),
        m_StateCalculated(false)
    {
        CSample::reset();
    }

    CSample & CSample::operator=(const CSample & t_Sample)
    {
        m_StateCalculated = t_Sample.m_StateCalculated;
        m_IntegrationType = t_Sample.m_IntegrationType;
        m_NormalizationCoefficient = t_Sample.m_NormalizationCoefficient;
        m_WavelengthSet = t_Sample.m_WavelengthSet;
        m_IncomingSource = wce::make_unique<CSeries>(*t_Sample.m_IncomingSource);
        for(const auto & prop : EnumProperty())
        {
            for(const auto & side : EnumSide())
            {
                m_EnergySource[std::make_pair(prop, side)] = wce::make_unique<CSeries>(
                  *t_Sample.m_EnergySource.at(std::make_pair(prop, side)));
            }
        }

        return *this;
    }

    CSample::CSample(const CSample & t_Sample)
    {
        operator=(t_Sample);
    }

    std::shared_ptr<CSeries> CSample::getSourceData()
    {
        calculateState();   // must interpolate data to same wavelengths
        return m_SourceData;
    }

    void CSample::setSourceData(std::shared_ptr<CSeries> t_SourceData)
    {
        m_SourceData = t_SourceData;
        reset();
    }

    void CSample::setDetectorData(const std::shared_ptr<CSeries> & t_DetectorData)
    {
        m_DetectorData = t_DetectorData;
        reset();
    }

    FenestrationCommon::IntegrationType CSample::getIntegrator() const
    {
        return m_IntegrationType;
    }

    double CSample::getNormalizationCoeff() const
    {
        return m_NormalizationCoefficient;
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
                if(m_SourceData == nullptr)
                {
                    throw std::runtime_error(
                      "Cannot extract wavelenghts from source. Source is empty.");
                }
                m_Wavelengths = m_SourceData->getXArray();
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
        calculateState();
        return m_EnergySource.at(std::make_pair(t_Property, t_Side))->sum(minLambda, maxLambda);
    }

    double CSample::getProperty(double const minLambda,
                                double const maxLambda,
                                Property const t_Property,
                                Side const t_Side)
    {
        calculateState();
        auto Prop = 0.0;
        // Incoming energy can be calculated only if user has defined incoming source.
        // Otherwise just assume zero property.
        if(m_IncomingSource != nullptr)
        {
            auto incomingEnergy = m_IncomingSource->sum(minLambda, maxLambda);
            double propertyEnergy =
              m_EnergySource.at(std::make_pair(t_Property, t_Side))->sum(minLambda, maxLambda);
            Prop = propertyEnergy / incomingEnergy;
        }
        return Prop;
    }

    CSeries * CSample::getEnergyProperties(Property const t_Property, Side const t_Side)
    {
        calculateState();
        return m_EnergySource.at(std::make_pair(t_Property, t_Side)).get();
    }

    size_t CSample::getBandSize() const
    {
        return m_Wavelengths.size();
    }

    void CSample::reset()
    {
        m_StateCalculated = false;
        m_IncomingSource = nullptr;
        for(const auto & prop : EnumProperty())
        {
            for(const auto & side : EnumSide())
            {
                m_EnergySource[std::make_pair(prop, side)] = nullptr;
            }
        }
    }

    void CSample::calculateState()
    {
        if(!m_StateCalculated)
        {
            if(m_WavelengthSet != WavelengthSet::Custom)
            {
                setWavelengths(m_WavelengthSet);
            }

            // In case source data are set then apply solar radiation to the calculations.
            // Otherwise, just use measured data.
            if(m_SourceData != nullptr)
            {
                m_IncomingSource = m_SourceData->interpolate(m_Wavelengths);


                if(m_DetectorData != nullptr)
                {
                    const auto interpolatedDetector = *m_DetectorData->interpolate(m_Wavelengths);
                    m_IncomingSource = m_IncomingSource->mMult(interpolatedDetector);
                }

                calculateProperties();

                m_IncomingSource =
                  m_IncomingSource->integrate(m_IntegrationType, m_NormalizationCoefficient);
                for(const auto & prop : EnumProperty())
                {
                    for(const auto & side : EnumSide())
                    {
                        m_EnergySource[std::make_pair(prop, side)] =
                          m_EnergySource.at(std::make_pair(prop, side))
                            ->integrate(m_IntegrationType, m_NormalizationCoefficient);
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
                                     std::shared_ptr<CSeries> const & t_SourceData,
                                     FenestrationCommon::IntegrationType integrationType,
                                     double NormalizationCoefficient) :
        CSample(t_SourceData, integrationType, NormalizationCoefficient),
        m_SampleData(t_SampleData)
    {
        if(t_SampleData == nullptr)
        {
            throw std::runtime_error("Sample must have measured data.");
        }

        for(const auto & prop : EnumProperty())
        {
            for(const auto & side : EnumSide())
            {
                m_Property[std::make_pair(prop, side)] = nullptr;
            }
        }
    }

    CSpectralSample::CSpectralSample(std::shared_ptr<CSpectralSampleData> const & t_SampleData) :
        CSample(),
        m_SampleData(t_SampleData)
    {
        if(t_SampleData == nullptr)
        {
            throw std::runtime_error("Sample must have measured data.");
        }

        for(const auto & prop : EnumProperty())
        {
            for(const auto & side : EnumSide())
            {
                m_Property[std::make_pair(prop, side)] = nullptr;
            }
        }
    }

    std::shared_ptr<CSpectralSampleData> CSpectralSample::getMeasuredData()
    {
        calculateState();   // Interpolation is needed before returning the data
        return m_SampleData;
    }

    std::vector<double> CSpectralSample::getWavelengthsFromSample() const
    {
        return m_SampleData->getWavelengths();
    }

    std::shared_ptr<CSeries> CSpectralSample::getWavelengthsProperty(Property const t_Property,
                                                                     Side const t_Side)
    {
        calculateState();

        return m_Property.at(std::make_pair(t_Property, t_Side));
    }

    void CSpectralSample::calculateProperties()
    {
        // No need to do interpolation if wavelength set is already from the data.
        if(m_WavelengthSet == WavelengthSet::Data)
        {
            for(const auto & prop : EnumProperty())
            {
                for(const auto & side : EnumSide())
                {
                    m_Property[std::make_pair(prop, side)] = m_SampleData->properties(prop, side);
                }
            }
        }
        else
        {
            for(const auto & prop : EnumProperty())
            {
                for(const auto & side : EnumSide())
                {
                    m_Property[std::make_pair(prop, side)] =
                      m_SampleData->properties(prop, side)->interpolate(m_Wavelengths);
                }
            }
        }

        assert(m_IncomingSource != nullptr);

        // Calculation of energy balances
        for(const auto & prop : EnumProperty())
        {
            for(const auto & side : EnumSide())
            {
                m_Property[std::make_pair(prop, side)] =
                  m_SampleData->properties(prop, side)->interpolate(m_Wavelengths);
            }
        }
        for(const auto & prop : EnumProperty())
        {
            for(const auto & side : EnumSide())
            {
                m_EnergySource[std::make_pair(prop, side)] =
                  m_Property.at(std::make_pair(prop, side))->mMult(*m_IncomingSource);
            }
        }
    }

    void CSpectralSample::calculateState()
    {
        CSample::calculateState();
        if(m_SourceData == nullptr)
        {
            for(const auto & prop : EnumProperty())
            {
                for(const auto & side : EnumSide())
                {
                    m_Property[std::make_pair(prop, side)] = m_SampleData->properties(prop, side);
                }
            }

            m_StateCalculated = true;
        }
    }

    void CSpectralSample::cutExtraData(const double minLambda, const double maxLambda)
    {
        m_SampleData->cutExtraData(minLambda, maxLambda);
    }

    //////////////////////////////////////////////////////////////////////////////////////
    ////  CSpectralAngleSample
    //////////////////////////////////////////////////////////////////////////////////////

    CSpectralAngleSample::CSpectralAngleSample(std::shared_ptr<CSpectralSample> const & t_Sample,
                                               double const t_Angle) :
        m_Sample(t_Sample),
        m_Angle(t_Angle)
    {}

    double CSpectralAngleSample::angle() const
    {
        return m_Angle;
    }

    std::shared_ptr<CSpectralSample> CSpectralAngleSample::sample() const
    {
        return m_Sample;
    }


}   // namespace SpectralAveraging
