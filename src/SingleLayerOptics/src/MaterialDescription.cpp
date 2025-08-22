#include <cassert>
#include <stdexcept>
#include <sstream>

#include <mutex>

#include "MaterialDescription.hpp"
#include "WCECommon.hpp"
#include "OpticalSurface.hpp"

std::mutex materialWL;
std::mutex baseMaterialMutex;
std::mutex materialSampleWL;
std::mutex dualBandMaterialMutexRatio;
std::mutex dualBandMaterialMutexRadiation;

using namespace FenestrationCommon;
using namespace SpectralAveraging;

namespace SingleLayerOptics
{
    double modifyProperty(const double t_Range, const double t_Solar, const double t_Fraction)
    {
        // If t_fraction == 1 that means a dual-band material is evaluated only for partial range
        if(isEqual(t_Fraction, 1.0))
        {
            return t_Range;
        }

        auto ratio{(t_Solar - t_Fraction * t_Range) / (1 - t_Fraction)};
        if(ratio < 0)
            ratio = 0;
        return ratio;
    }

    std::vector<std::vector<double>>
      modifyProperties(std::vector<std::vector<double>> const & t_PartialRange,
                       std::vector<std::vector<double>> const & t_FullRange,
                       const double t_Fraction)
    {
        size_t outgoingSize = t_PartialRange.begin()->size();

        // Creating a vector with the correct size already reserved
        std::vector<double> outgoing(outgoingSize);
        size_t incomingSize = t_PartialRange.size();
        std::vector<std::vector<double>> modifiedValues(incomingSize, outgoing);
        for(size_t i = 0; i < incomingSize; ++i)
        {
            std::vector<double> const & partialOutgoing = t_PartialRange.at(i);
            std::vector<double> const & fullOutgoing = t_FullRange.at(i);
            std::vector<double> & modifiedOutgoing = modifiedValues.at(i);

            for(size_t j = 0; j < outgoingSize; ++j)
            {
                modifiedOutgoing[j] =
                  modifyProperty(partialOutgoing[j], fullOutgoing[j], t_Fraction);
            }
        }
        return modifiedValues;
    }

    std::shared_ptr<CMaterial> createNIRRange(const std::shared_ptr<CMaterial> & t_PartialRange,
                                              const std::shared_ptr<CMaterial> & t_FullRange,
                                              const double t_Fraction)
    {
        double Tf_nir = modifyProperty(t_PartialRange->getProperty(Property::T, Side::Front),
                                       t_FullRange->getProperty(Property::T, Side::Front),
                                       t_Fraction);
        double Tb_nir = modifyProperty(t_PartialRange->getProperty(Property::T, Side::Back),
                                       t_FullRange->getProperty(Property::T, Side::Back),
                                       t_Fraction);

        double Rf_nir = modifyProperty(t_PartialRange->getProperty(Property::R, Side::Front),
                                       t_FullRange->getProperty(Property::R, Side::Front),
                                       t_Fraction);
        double Rb_nir = modifyProperty(t_PartialRange->getProperty(Property::R, Side::Back),
                                       t_FullRange->getProperty(Property::R, Side::Back),
                                       t_Fraction);

        return std::make_shared<CMaterialSingleBand>(Tf_nir, Tb_nir, Rf_nir, Rb_nir);
    }

    std::shared_ptr<CMaterial>
      createNIRRange(const std::shared_ptr<CMaterialSingleBandBSDF> & t_PartialRange,
                     const std::shared_ptr<CMaterialSingleBandBSDF> & t_FullRange,
                     const double t_Fraction)
    {
        auto Tf_nir = modifyProperties(t_PartialRange->getBSDFMatrix(Property::T, Side::Front),
                                       t_FullRange->getBSDFMatrix(Property::T, Side::Front),
                                       t_Fraction);
        auto Tb_nir = modifyProperties(t_PartialRange->getBSDFMatrix(Property::T, Side::Back),
                                       t_FullRange->getBSDFMatrix(Property::T, Side::Back),
                                       t_Fraction);

        auto Rf_nir = modifyProperties(t_PartialRange->getBSDFMatrix(Property::R, Side::Front),
                                       t_FullRange->getBSDFMatrix(Property::R, Side::Front),
                                       t_Fraction);
        auto Rb_nir = modifyProperties(t_PartialRange->getBSDFMatrix(Property::R, Side::Back),
                                       t_FullRange->getBSDFMatrix(Property::R, Side::Back),
                                       t_Fraction);

        return std::make_shared<CMaterialSingleBandBSDF>(
          Tf_nir, Tb_nir, Rf_nir, Rb_nir, t_PartialRange->getHemisphere());
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////   RMaterialProperties
    ////////////////////////////////////////////////////////////////////////////////////

    RMaterialProperties::RMaterialProperties(const double aTf,
                                             const double aTb,
                                             const double aRf,
                                             const double aRb)
    {
        m_Surface[Side::Front] = std::make_shared<CSurface>(aTf, aRf);
        m_Surface[Side::Back] = std::make_shared<CSurface>(aTb, aRb);
    }

    double RMaterialProperties::getProperty(const Property t_Property, const Side t_Side) const
    {
        return m_Surface.at(t_Side)->getProperty(t_Property);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////   CMaterial
    ////////////////////////////////////////////////////////////////////////////////////

    CMaterial::CMaterial(const double minLambda, const double maxLambda) :
        m_MinLambda(minLambda), m_MaxLambda(maxLambda), m_WavelengthsCalculated(false)
    {}

    CMaterial::CMaterial(FenestrationCommon::Limits wavelengthRange) :
        CMaterial(wavelengthRange.minimum, wavelengthRange.maximum)
    {}

    void CMaterial::setSourceData(const CSeries &)
    {
        // Default material will not have source data
    }

    void CMaterial::setDetectorData(const CSeries &)
    {
        // Default material will not have detector data
    }

    std::vector<RMaterialProperties> CMaterial::getBandProperties()
    {
        std::vector<RMaterialProperties> aProperties;

        std::vector<double> Tf = getBandProperties(Property::T, Side::Front);
        std::vector<double> Tb = getBandProperties(Property::T, Side::Back);
        std::vector<double> Rf = getBandProperties(Property::R, Side::Front);
        std::vector<double> Rb = getBandProperties(Property::R, Side::Back);

        // It is necessary to skip calculations if solar properties are not assigned yet
        const size_t size = getBandSize();
        for(size_t i = 0; i < size; ++i)
        {
            RMaterialProperties aMaterial = RMaterialProperties(Tf[i], Tb[i], Rf[i], Rb[i]);
            aProperties.push_back(aMaterial);
        }

        return aProperties;
    }

    std::shared_ptr<CSpectralSample> CMaterial::getSpectralSample()
    {
        std::vector<double> Tf = getBandProperties(Property::T, Side::Front);
        std::vector<double> Tb = getBandProperties(Property::T, Side::Back);
        std::vector<double> Rf = getBandProperties(Property::R, Side::Front);
        std::vector<double> Rb = getBandProperties(Property::R, Side::Back);

        std::shared_ptr<CSpectralSampleData> aSampleData = std::make_shared<CSpectralSampleData>();

        size_t size = getBandSize();
        for(size_t i = 0; i < size; ++i)
        {
            aSampleData->addRecord(m_Wavelengths[i], Tf[i], Tb[i], Rf[i], Rb[i]);
        }

        return std::make_shared<CSpectralSample>(aSampleData);
    }

    const std::vector<double> & CMaterial::getBandWavelengths()
    {
        std::lock_guard<std::mutex> lock(materialWL);
        if(!m_WavelengthsCalculated)
        {
            m_Wavelengths = calculateBandWavelengths();
            m_WavelengthsCalculated = true;
        }
        return m_Wavelengths;
    }

    bool CMaterial::isWavelengthInRange(double wavelength) const
    {
        return ((m_MinLambda - ConstantsData::wavelengthErrorTolerance) <= wavelength)
               && ((m_MaxLambda + ConstantsData::wavelengthErrorTolerance) >= wavelength);
    }

    void CMaterial::setBandWavelengths(const std::vector<double> & wavelengths)
    {
        std::lock_guard<std::mutex> lock(baseMaterialMutex);
        m_Wavelengths = wavelengths;
        m_WavelengthsCalculated = true;
    }

    size_t CMaterial::getBandSize()
    {
        return getBandWavelengths().size();
    }

    int CMaterial::getBandIndex(const double t_Wavelength)
    {
        int aIndex = -1;
        size_t size = getBandSize();
        for(size_t i = 0; i < size; ++i)
        {
            if(m_Wavelengths[i] < (t_Wavelength + 1e-6))
            {
                ++aIndex;
            }
        }
        return aIndex;
    }

    double CMaterial::getMinLambda() const
    {
        return m_MinLambda;
    }

    double CMaterial::getMaxLambda() const
    {
        return m_MaxLambda;
    }

    void CMaterial::Flipped(bool)
    {
        // Does nothing so far. Needs to be virtual once shadings are resolved.
    }

    FenestrationCommon::CSeries CMaterial::jscPrime(FenestrationCommon::Side) const
    {
        return {};
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////   CMaterialSingleBand
    ////////////////////////////////////////////////////////////////////////////////////
    CMaterialSingleBand::CMaterialSingleBand(double t_Tf, double t_Tb, double t_Rf, double t_Rb) :
        CMaterial(ConstantsData::MINLAMBDAVALUE, ConstantsData::MAXLAMBDAVALUE)
    {
        m_Property[Side::Front] = std::make_shared<CSurface>(t_Tf, t_Rf);
        m_Property[Side::Back] = std::make_shared<CSurface>(t_Tb, t_Rb);
    }

    double CMaterialSingleBand::getProperty(Property t_Property,
                                            Side t_Side,
                                            const CBeamDirection &,
                                            const CBeamDirection &) const
    {
        return m_Property.at(t_Side)->getProperty(t_Property);
    }

    std::vector<double> CMaterialSingleBand::getBandProperties(const Property t_Property,
                                                               const Side t_Side,
                                                               const CBeamDirection &,
                                                               const CBeamDirection &) const
    {
        std::vector<double> aResult;
        const auto prop{getProperty(t_Property, t_Side)};
        aResult.push_back(prop);
        aResult.push_back(prop);
        return aResult;
    }

    double CMaterialSingleBand::getBandProperty(FenestrationCommon::Property t_Property,
                                                FenestrationCommon::Side t_Side,
                                                size_t wavelengthIndex,
                                                const CBeamDirection & t_IncomingDirection,
                                                const CBeamDirection & t_OutgoingDirection) const
    {
        std::ignore = wavelengthIndex;
        std::ignore = t_IncomingDirection;
        std::ignore = t_OutgoingDirection;
        return getProperty(t_Property, t_Side);
    }

    std::vector<double> CMaterialSingleBand::calculateBandWavelengths()
    {
        return {m_MinLambda, m_MaxLambda};
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////   IMaterialDualBand
    ////////////////////////////////////////////////////////////////////////////////////

    IMaterialDualBand::IMaterialDualBand(const std::shared_ptr<CMaterial> & visibleRange,
                                         const std::shared_ptr<CMaterial> & solarRange) :
        CMaterial(solarRange->getMinLambda(), solarRange->getMaxLambda()),
        m_MaterialSolarRange(solarRange),
        m_MaterialVisibleRange(visibleRange)
    {}

    void IMaterialDualBand::setSourceData(const CSeries & t_SourceData)
    {
        m_MaterialSolarRange->setSourceData(t_SourceData);
        m_MaterialVisibleRange->setSourceData(t_SourceData);
        createNIRRange(m_MaterialVisibleRange, m_MaterialSolarRange, ConstantsData::NIRRatio);
    }

    void IMaterialDualBand::setDetectorData(const CSeries & t_DetectorData)
    {
        m_MaterialSolarRange->setDetectorData(t_DetectorData);
        m_MaterialVisibleRange->setDetectorData(t_DetectorData);
    }

    double IMaterialDualBand::getProperty(Property t_Property,
                                          Side t_Side,
                                          const CBeamDirection & t_Incoming,
                                          const CBeamDirection & t_Outgoing) const
    {
        return m_MaterialSolarRange->getProperty(t_Property, t_Side, t_Incoming, t_Outgoing);
    }

    std::vector<double>
      IMaterialDualBand::getBandProperties(const Property t_Property,
                                           const Side t_Side,
                                           const CBeamDirection & t_Incoming,
                                           const CBeamDirection & t_Outgoing) const
    {
        std::vector<double> aResults;
        aResults.reserve(m_Wavelengths.size());

        for(const auto wl : m_Wavelengths)
        {
            aResults.emplace_back(getMaterialFromWavelength(wl)->getProperty(
              t_Property, t_Side, t_Incoming, t_Outgoing));
        }

        return aResults;
    }

    double IMaterialDualBand::getBandProperty(Property t_Property,
                                              Side t_Side,
                                              size_t wavelengthIndex,
                                              const CBeamDirection & t_IncomingDirection,
                                              const CBeamDirection & t_OutgoingDirection) const
    {
        return getMaterialFromWavelength(m_Wavelengths[wavelengthIndex])
          ->getProperty(t_Property, t_Side, t_IncomingDirection, t_OutgoingDirection);
    }

    std::vector<double> IMaterialDualBand::calculateBandWavelengths()
    {
        // Need to create small offsets on each side because spectral integration will create
        // linear interpolation between two points thus creating incorrect values.
        // Slopes are intentionally made to be just outside the visible range so
        // they will not produce problems in the visible range itself
        m_Wavelengths = {0.3,
                         0.38 - (ConstantsData::VisibleRangeOffset / 2),
                         0.38 + (ConstantsData::VisibleRangeOffset / 2),
                         0.78 - (ConstantsData::VisibleRangeOffset / 2),
                         0.78 + (ConstantsData::VisibleRangeOffset / 2),
                         2.5};
        return m_Wavelengths;
    }

    CMaterialDualBand::CMaterialDualBand(const std::shared_ptr<CMaterial> & t_PartialRange,
                                         const std::shared_ptr<CMaterial> & t_FullRange) :
        IMaterialDualBand(t_PartialRange, t_FullRange)
    {}

    void CMaterialDualBand::createNIRRange(const std::shared_ptr<CMaterial> & t_PartialRange,
                                           const std::shared_ptr<CMaterial> & t_FullRange,
                                           const double t_Fraction)
    {
        m_MaterialScaledRange =
          SingleLayerOptics::createNIRRange(t_PartialRange, t_FullRange, t_Fraction);
    }

    void IMaterialDualBand::createRangesFromRatio(double t_Ratio)
    {
        createNIRRange(m_MaterialVisibleRange, m_MaterialSolarRange, t_Ratio);

        std::lock_guard<std::mutex> lock(dualBandMaterialMutexRatio);
        if(!m_WavelengthsCalculated)
        {
            m_Wavelengths = calculateBandWavelengths();
            m_WavelengthsCalculated = true;
        }
    }

    void IMaterialDualBand::createRangesFromSolarRadiation(const CSeries &)
    {
        createNIRRange(m_MaterialVisibleRange, m_MaterialSolarRange, ConstantsData::NIRRatio);

        std::lock_guard<std::mutex> lock(dualBandMaterialMutexRadiation);
        if(!m_WavelengthsCalculated)
        {
            m_Wavelengths = calculateBandWavelengths();
            m_WavelengthsCalculated = true;
        }
    }

    std::vector<double> IMaterialDualBand::getWavelengthsFromMaterials() const
    {
        return m_Wavelengths;
    }

    std::shared_ptr<CMaterial>
      IMaterialDualBand::getMaterialFromWavelength(const double wavelength) const
    {
        std::shared_ptr<CMaterial> result;

        CWavelengthRange range{WavelengthRange::Visible};

        return range.isInRange(wavelength) ? m_MaterialVisibleRange : m_MaterialScaledRange;
    }


    ////////////////////////////////////////////////////////////////////////////////////
    ////   CMaterialSample
    ////////////////////////////////////////////////////////////////////////////////////

    CMaterialSample::CMaterialSample(
      const std::shared_ptr<SpectralAveraging::CSpectralSample> & t_SpectralSample,
      double t_Thickness,
      FenestrationCommon::MaterialType t_Type) :
        CMaterial(t_SpectralSample->getWavelengthLimits()),
        m_AngularSample(t_SpectralSample, t_Thickness, t_Type)
    {
        if(t_SpectralSample == nullptr)
        {
            throw std::runtime_error("Cannot create specular material from non-existing sample.");
        }
    }

    void CMaterialSample::setSourceData(const CSeries & t_SourceData)
    {
        m_AngularSample.setSourceData(t_SourceData);
    }

    void CMaterialSample::setDetectorData(const CSeries & t_DetectorData)
    {
        m_AngularSample.setDetectorData(t_DetectorData);
    }

    double CMaterialSample::getProperty(const Property t_Property,
                                        const Side t_Side,
                                        const CBeamDirection & t_IncomingDirection,
                                        const CBeamDirection & t_OutgoingDirection) const
    {
        return m_AngularSample.getProperty(m_MinLambda,
                                           m_MaxLambda,
                                           t_Property,
                                           t_Side,
                                           t_IncomingDirection.theta(),
                                           scatter(t_IncomingDirection, t_OutgoingDirection));
    }

    std::vector<double>
      CMaterialSample::getBandProperties(const Property t_Property,
                                         const Side t_Side,
                                         const CBeamDirection & t_IncomingDirection,
                                         const CBeamDirection & t_OutgoingDirection) const
    {
        std::lock_guard lock(m_CacheMutex);

        CacheKey key{t_Property,
                     t_Side,
                     t_IncomingDirection.theta(),
                     t_OutgoingDirection.theta(),
                     m_AngularSample.getBandWavelengths().size()};

        // Check if the result is already cached
        auto it = m_Cache.find(key);
        if(it != m_Cache.end())
        {
            return it->second;
        }

        // Perform the calculation
        auto result = m_AngularSample.getWavelengthProperties(
          t_Property,
          t_Side,
          t_IncomingDirection.theta(),
          scatter(t_IncomingDirection, t_OutgoingDirection));

        // Store the result in the cache
        m_Cache[key] = result;

        return result;
    }

    double CMaterialSample::getBandProperty(Property t_Property,
                                            Side t_Side,
                                            size_t wavelengthIndex,
                                            const CBeamDirection & t_IncomingDirection,
                                            const CBeamDirection & t_OutgoingDirection) const
    {
        return getBandProperties(
          t_Property, t_Side, t_IncomingDirection, t_OutgoingDirection)[wavelengthIndex];
    }


    std::vector<double> CMaterialSample::calculateBandWavelengths()
    {
        return m_AngularSample.getBandWavelengths();
    }

    ScatteringType scatter(const CBeamDirection & t_IncomingDirection,
                           const CBeamDirection & t_OutgoingDirection)
    {
        return t_IncomingDirection == t_OutgoingDirection ? ScatteringType::Total
                                                          : ScatteringType::Diffuse;
    }

    void CMaterialSample::setBandWavelengths(const std::vector<double> & wavelengths)
    {
        std::lock_guard<std::mutex> lock(materialSampleWL);

        CMaterial::setBandWavelengths(wavelengths);
        m_AngularSample.setBandWavelengths(m_Wavelengths);
        m_WavelengthsCalculated = true;
    }

    void CMaterialSample::Flipped(bool flipped)
    {
        m_AngularSample.Flipped(flipped);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////   CMaterialPhotovoltaicSample
    ////////////////////////////////////////////////////////////////////////////////////

    CMaterialPhotovoltaicSample::CMaterialPhotovoltaicSample(
      const std::shared_ptr<SpectralAveraging::CPhotovoltaicSample> & t_SpectralSample,
      double t_Thickness,
      FenestrationCommon::MaterialType t_Type) :
        CMaterialSample(t_SpectralSample, t_Thickness, t_Type), m_PVSample(t_SpectralSample)
    {}

    FenestrationCommon::CSeries
      CMaterialPhotovoltaicSample::jscPrime(FenestrationCommon::Side t_Side) const
    {
        return m_PVSample->jscPrime(t_Side);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////   CMaterialMeasured
    ////////////////////////////////////////////////////////////////////////////////////

    CMaterialMeasured::CMaterialMeasured(
      const std::shared_ptr<SpectralAveraging::CAngularMeasurements> & t_Measurements) :
        CMaterial(t_Measurements->getWavelengthLimits()), m_AngularMeasurements(t_Measurements)
    {
        if(t_Measurements == nullptr)
        {
            throw std::runtime_error(
              "Cannot create specular and angular material from non-existing sample.");
        }
    }

    void CMaterialMeasured::setSourceData(const CSeries & t_SourceData)
    {
        m_AngularMeasurements->setSourceData(t_SourceData);
    }

    double CMaterialMeasured::getProperty(const Property t_Property,
                                          const Side t_Side,
                                          const CBeamDirection & t_IncomingDirection,
                                          const CBeamDirection & t_OutgoingDirection) const
    {
        assert(m_AngularMeasurements);
        const std::shared_ptr<CSingleAngularMeasurement> aAngular =
          m_AngularMeasurements->getMeasurements(t_IncomingDirection.theta());
        const std::shared_ptr<CSpectralSample> aSample = aAngular->getData();
        return aSample->getProperty(m_MinLambda,
                                    m_MaxLambda,
                                    t_Property,
                                    t_Side,
                                    scatter(t_IncomingDirection, t_OutgoingDirection));
    }


    std::vector<double>
      CMaterialMeasured::getBandProperties(const Property t_Property,
                                           const Side t_Side,
                                           const CBeamDirection & t_IncomingDirection,
                                           const CBeamDirection &) const
    {
        assert(m_AngularMeasurements);
        std::shared_ptr<CSingleAngularMeasurement> aAngular =
          m_AngularMeasurements->getMeasurements(t_IncomingDirection.theta());
        std::shared_ptr<CSpectralSample> aSample = aAngular->getData();
        auto aProperties =
          aSample->getWavelengthsProperty(t_Property, t_Side, ScatteringType::Total);

        std::vector<double> aValues;

        for(const auto & aProperty : aProperties)
        {
            if(aProperty.x() >= m_MinLambda && aProperty.x() <= m_MaxLambda)
            {
                aValues.push_back(aProperty.value());
            }
        }

        return aValues;
    }

    double CMaterialMeasured::getBandProperty(FenestrationCommon::Property t_Property,
                                              FenestrationCommon::Side t_Side,
                                              size_t wavelengthIndex,
                                              const CBeamDirection & t_IncomingDirection,
                                              const CBeamDirection & t_OutgoingDirection) const
    {
        return getBandProperties(
          t_Property, t_Side, t_IncomingDirection, t_OutgoingDirection)[wavelengthIndex];
    }

    std::vector<double> CMaterialMeasured::calculateBandWavelengths()
    {
        CSingleAngularMeasurement aAngular = *m_AngularMeasurements->getMeasurements(0.0);
        auto aSample = aAngular.getData();

        return aSample->getWavelengthsFromSample();
    }

    CMaterialSingleBandBSDF::CMaterialSingleBandBSDF(std::vector<std::vector<double>> const & t_Tf,
                                                     std::vector<std::vector<double>> const & t_Tb,
                                                     std::vector<std::vector<double>> const & t_Rf,
                                                     std::vector<std::vector<double>> const & t_Rb,
                                                     BSDFHemisphere const & t_Hemisphere) :
        CMaterial(ConstantsData::MINLAMBDAVALUE, ConstantsData::MAXLAMBDAVALUE),
        m_Hemisphere(t_Hemisphere)
    {
        validateMatrix(t_Tf, m_Hemisphere);
        validateMatrix(t_Tb, m_Hemisphere);
        validateMatrix(t_Rf, m_Hemisphere);
        validateMatrix(t_Rb, m_Hemisphere);
        m_Property[std::make_pair(Property::T, Side::Front)] = t_Tf;
        m_Property[std::make_pair(Property::T, Side::Back)] = t_Tb;
        m_Property[std::make_pair(Property::R, Side::Front)] = t_Rf;
        m_Property[std::make_pair(Property::R, Side::Back)] = t_Rb;
        m_Wavelengths = calculateBandWavelengths();
    }

    double calcDirectHemispheric(std::vector<std::vector<double>> const & m,
                                 BSDFHemisphere const & hemisphere,
                                 size_t incomingIdx)
    {
        const auto outgoingLambdas =
          hemisphere.getDirections(BSDFDirection::Outgoing).lambdaVector();

        double result = 0;
        for(size_t outgoingIdx = 0; outgoingIdx < outgoingLambdas.size(); ++outgoingIdx)
        {
            result += m[outgoingIdx][incomingIdx] * outgoingLambdas[outgoingIdx];
        }
        return result;
    }


    double CMaterialSingleBandBSDF::getProperty(FenestrationCommon::Property t_Property,
                                                FenestrationCommon::Side t_Side,
                                                const CBeamDirection & t_IncomingDirection,
                                                const CBeamDirection & t_OutgoingDirection) const
    {
        const auto incomingIdx =
          m_Hemisphere.getDirections(BSDFDirection::Incoming)
            .getNearestBeamIndex(t_IncomingDirection.theta(), t_IncomingDirection.phi());

        if(t_Property == FenestrationCommon::Property::Abs)
        {
            double tHem = calcDirectHemispheric(
              m_Property.at({FenestrationCommon::Property::T, t_Side}), m_Hemisphere, incomingIdx);
            double rHem = calcDirectHemispheric(
              m_Property.at({FenestrationCommon::Property::R, t_Side}), m_Hemisphere, incomingIdx);
            return 1 - tHem - rHem;
        }
        else
        {
            const auto outgoingIdx =
              m_Hemisphere.getDirections(BSDFDirection::Outgoing)
                .getNearestBeamIndex(t_OutgoingDirection.theta(), t_OutgoingDirection.phi());

            auto lambda{m_Hemisphere.getDirections(BSDFDirection::Outgoing).lambdaVector()};

            const auto val = m_Property.at({t_Property, t_Side})[outgoingIdx][incomingIdx];

            return val * lambda[outgoingIdx];
        }
    }

    std::vector<double>
      CMaterialSingleBandBSDF::getBandProperties(FenestrationCommon::Property t_Property,
                                                 FenestrationCommon::Side t_Side,
                                                 const CBeamDirection & t_IncomingDirection,
                                                 const CBeamDirection & t_OutgoingDirection) const
    {
        double value = getProperty(t_Property, t_Side, t_IncomingDirection, t_OutgoingDirection);
        std::vector<double> bandProperties(m_Wavelengths.size(), value);
        return bandProperties;
    }

    double
      CMaterialSingleBandBSDF::getBandProperty(FenestrationCommon::Property t_Property,
                                               FenestrationCommon::Side t_Side,
                                               size_t wavelengthIndex,
                                               const CBeamDirection & t_IncomingDirection,
                                               const CBeamDirection & t_OutgoingDirection) const
    {
        std::ignore = wavelengthIndex;
        return getProperty(t_Property, t_Side, t_IncomingDirection, t_OutgoingDirection);
    }

    std::vector<std::vector<double>> const &
      CMaterialSingleBandBSDF::getBSDFMatrix(FenestrationCommon::Property const & t_Property,
                                             FenestrationCommon::Side const & t_Side) const
    {
        return m_Property.at({t_Property, t_Side});
    }

    BSDFHemisphere CMaterialSingleBandBSDF::getHemisphere() const
    {
        return m_Hemisphere;
    }

    std::vector<double> CMaterialSingleBandBSDF::calculateBandWavelengths()
    {
        return {m_MinLambda, m_MaxLambda};
    }

    void CMaterialSingleBandBSDF::validateMatrix(std::vector<std::vector<double>> const & matrix,
                                                 BSDFHemisphere const & hemisphere) const
    {
        size_t rowCt = matrix.size();
        size_t colCt = matrix[0].size();
        size_t hemisphereIncomingCt = hemisphere.getDirections(BSDFDirection::Incoming).size();
        size_t hemisphereOutgoingCt = hemisphere.getDirections(BSDFDirection::Outgoing).size();

        if(rowCt != hemisphereIncomingCt)
        {
            std::stringstream msg;
            msg << "Incompatible number of incoming directions.  BSDF matrix: << " << rowCt
                << " BSDF Hemispere: " << hemisphereIncomingCt;
            throw std::runtime_error(msg.str());
        }

        if(colCt != hemisphereOutgoingCt)
        {
            std::stringstream msg;
            msg << "Incompatible number of incoming directions.  BSDF matrix: << " << colCt
                << " BSDF Hemispere: " << hemisphereOutgoingCt;
            throw std::runtime_error(msg.str());
        }
    }

    CMaterialDualBandBSDF::CMaterialDualBandBSDF(
      const std::shared_ptr<CMaterialSingleBandBSDF> & t_PartialRange,
      const std::shared_ptr<CMaterialSingleBandBSDF> & t_FullRange) :
        IMaterialDualBand(t_PartialRange, t_FullRange)
    {}

    void CMaterialDualBandBSDF::createNIRRange(const std::shared_ptr<CMaterial> & t_PartialRange,
                                               const std::shared_ptr<CMaterial> & t_FullRange,
                                               const double t_Fraction)
    {
        m_MaterialScaledRange = SingleLayerOptics::createNIRRange(
          std::dynamic_pointer_cast<CMaterialSingleBandBSDF>(t_PartialRange),
          std::dynamic_pointer_cast<CMaterialSingleBandBSDF>(t_FullRange),
          t_Fraction);
    }

}   // namespace SingleLayerOptics
