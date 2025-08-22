#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <mutex>

#include <WCECommon.hpp>

#include "AngularSpectralSample.hpp"
#include "SpectralSampleData.hpp"
#include "SpectralSample.hpp"
#include "AngularProperties.hpp"

std::mutex findAngularSample;

using namespace FenestrationCommon;

namespace SpectralAveraging
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //// CAngularSpectralProperties
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    CAngularSpectralProperties::CAngularSpectralProperties(
      std::shared_ptr<CSpectralSample> const & t_SpectralSample,
      double const t_Angle,
      MaterialType const t_Type,
      double const t_Thickness) :
        m_Angle(t_Angle), m_Thickness(t_Thickness)
    {
        m_AngularData = std::make_shared<CSpectralSampleData>();
        calculateAngularProperties(t_SpectralSample, t_Type);
    }

    double CAngularSpectralProperties::angle() const
    {
        return m_Angle;
    }

    std::shared_ptr<CSpectralSampleData> CAngularSpectralProperties::properties() const
    {
        return m_AngularData;
    }

    inline double calcHaze(const double direct, const double diffuse, const double tol = 1e-8)
    {
        const double sum = direct + diffuse;
        if(std::abs(sum) <= tol)
        {
            return 0.0;
        }
        return diffuse / sum;
    }

    void CAngularSpectralProperties::calculateAngularProperties(
      std::shared_ptr<CSpectralSample> const & t_SpectralSample, MaterialType const t_Type)
    {
        assert(t_SpectralSample != nullptr);

        auto md = t_SpectralSample->getMeasuredData();
        auto wl = t_SpectralSample->getWavelengths();

        // Spectral sample is not initialized yet, use wavelengths from measured data instead.
        if(wl.empty())
        {
            wl = md->getWavelengths();
        }

        if(m_Angle != 0)
        {
            const auto aTf =
              md->properties(Property ::T, Side::Front, ScatteringType::Total).interpolate(wl);
            assert(aTf.size() == wl.size());

            const auto aTb =
              md->properties(Property ::T, Side::Back, ScatteringType::Total).interpolate(wl);
            assert(aTb.size() == wl.size());

            const auto aRf =
              md->properties(Property::R, Side::Front, ScatteringType::Total).interpolate(wl);
            assert(aRf.size() == wl.size());

            const auto aRb =
              md->properties(Property::R, Side::Back, ScatteringType::Total).interpolate(wl);
            assert(aRb.size() == wl.size());

            using PT = ScatteringType;
            // Direct/Diffuse (for haze only)
            // clang-format off
            const auto Tf_dir = md->properties(Property::T, Side::Front, PT::Direct).interpolate(wl);
            const auto Tf_dif = md->properties(Property::T, Side::Front, PT::Diffuse).interpolate(wl);
            const auto Tb_dir = md->properties(Property::T, Side::Back, PT::Direct).interpolate(wl);
            const auto Tb_dif = md->properties(Property::T, Side::Back, PT::Diffuse).interpolate(wl);
            const auto Rf_dir = md->properties(Property::R, Side::Front, PT::Direct).interpolate(wl);
            const auto Rf_dif = md->properties(Property::R, Side::Front, PT::Diffuse).interpolate(wl);
            const auto Rb_dir = md->properties(Property::R, Side::Back, PT::Direct).interpolate(wl);
            const auto Rb_dif = md->properties(Property::R, Side::Back, PT::Diffuse).interpolate(wl);
            // clang-format on

            for(size_t i = 0; i < wl.size(); ++i)
            {
                const auto aSurfaceType = coatingType.at(t_Type);

                auto aFrontFactory =
                  CAngularPropertiesFactory(aTf[i].value(), aRf[i].value(), m_Thickness);
                auto aBackFactory =
                  CAngularPropertiesFactory(aTb[i].value(), aRb[i].value(), m_Thickness);

                const auto aFrontProperties = aFrontFactory.getAngularProperties(aSurfaceType);
                const auto aBackProperties = aBackFactory.getAngularProperties(aSurfaceType);

                const auto ww = wl[i] * 1e-6;

                const auto Tfangle = aFrontProperties->transmittance(m_Angle, ww);
                const auto Tbangle = aBackProperties->transmittance(m_Angle, ww);
                const auto Rfangle = aFrontProperties->reflectance(m_Angle, ww);
                const auto Rbangle = aBackProperties->reflectance(m_Angle, ww);

                OpticalProperties direct;
                direct.Tf = (1 - calcHaze(Tf_dir[i].value(), Tf_dif[i].value())) * Tfangle;
                direct.Tb = (1 - calcHaze(Tb_dir[i].value(), Tb_dif[i].value())) * Tbangle;
                direct.Rf = (1 - calcHaze(Rf_dir[i].value(), Rf_dif[i].value())) * Rfangle;
                direct.Rb = (1 - calcHaze(Rb_dir[i].value(), Rb_dif[i].value())) * Rbangle;

                OpticalProperties diffuse;
                diffuse.Tf = calcHaze(Tf_dir[i].value(), Tf_dif[i].value()) * Tfangle;
                diffuse.Tb = calcHaze(Tb_dir[i].value(), Tb_dif[i].value()) * Tbangle;
                diffuse.Rf = calcHaze(Rf_dir[i].value(), Rf_dif[i].value()) * Rfangle;
                diffuse.Rb = calcHaze(Rb_dir[i].value(), Rb_dif[i].value()) * Rbangle;

                m_AngularData->addRecord(ww * 1e6, direct, diffuse);
            }
        }
        else
        {
            m_AngularData = md;
            m_AngularData->interpolate(wl);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //// CSpectralSampleAngle
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    CSpectralSampleAngle::CSpectralSampleAngle(std::shared_ptr<CSpectralSample> const & t_Sample,
                                               double const t_Angle) :
        m_Sample(t_Sample), m_Angle(t_Angle)
    {}

    double CSpectralSampleAngle::angle() const
    {
        return m_Angle;
    }

    std::shared_ptr<CSpectralSample> CSpectralSampleAngle::sample() const
    {
        return m_Sample;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //// CAngularSpectralSample
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    CAngularSpectralSample::CAngularSpectralSample(
      std::shared_ptr<CSpectralSample> const & t_SpectralSample,
      double const t_Thickness,
      FenestrationCommon::MaterialType const t_Type) :
        m_SpectralSampleZero(t_SpectralSample), m_Thickness(t_Thickness), m_Type(t_Type)
    {}

    void CAngularSpectralSample::setSourceData(const CSeries & t_SourceData)
    {
        m_SpectralSampleZero->setSourceData(t_SourceData);
        m_SpectralProperties.clear();
    }

    void CAngularSpectralSample::setDetectorData(const CSeries & t_DetectorData)
    {
        m_SpectralSampleZero->setDetectorData(t_DetectorData);
        m_SpectralProperties.clear();
    }

    double CAngularSpectralSample::getProperty(const double minLambda,
                                               const double maxLambda,
                                               const Property t_Property,
                                               const Side t_Side,
                                               const double t_Angle,
                                               const ScatteringType t_Scatter)
    {
        auto aSample = findSpectralSample(t_Angle);
        return aSample->getProperty(minLambda, maxLambda, t_Property, t_Side, t_Scatter);
    }

    std::vector<double> CAngularSpectralSample::getWavelengthProperties(const Property t_Property,
                                                                        const Side t_Side,
                                                                        const double t_Angle,
                                                                        const ScatteringType t_Scatter)
    {
        auto aSample = findSpectralSample(t_Angle);

        auto aProperties = aSample->getWavelengthsProperty(t_Property, t_Side, t_Scatter);

        return aProperties.getYArray();
    }

    std::vector<double> CAngularSpectralSample::getBandWavelengths() const
    {
        return m_SpectralSampleZero->getWavelengthsFromSample();
    }

    void CAngularSpectralSample::setBandWavelengths(const std::vector<double> & wavelegths)
    {
        m_SpectralSampleZero->setWavelengths(WavelengthSet::Custom, wavelegths);

        // All previous spectral properties are calculated with different wavelengths
        m_SpectralProperties.clear();
    }

    void CAngularSpectralSample::Flipped(bool flipped)
    {
        m_SpectralSampleZero->Flipped(flipped);
        for(auto & val : m_SpectralProperties)
        {
            val->sample()->Flipped(flipped);
        }
    }

    std::shared_ptr<CSpectralSample>
      CAngularSpectralSample::findSpectralSample(double const t_Angle)
    {
        std::lock_guard<std::mutex> lock(findAngularSample);

        std::shared_ptr<CSpectralSample> aSample = nullptr;

        const auto it = find_if(m_SpectralProperties.begin(),
                                m_SpectralProperties.end(),
                                [&t_Angle](std::shared_ptr<CSpectralSampleAngle> const & obj) {
                                    return std::abs(obj->angle() - t_Angle) < 1e-6;
                                });

        if(it != m_SpectralProperties.end())
        {
            aSample = (*it)->sample();
        }
        else
        {
            auto aAngularData =
              CAngularSpectralProperties(m_SpectralSampleZero, t_Angle, m_Type, m_Thickness);

            aSample = std::make_shared<CSpectralSample>(aAngularData.properties(),
                                                        m_SpectralSampleZero->getSourceData());
            aSample->assignDetectorAndWavelengths(m_SpectralSampleZero);
            const auto aSpectralSampleAngle =
              std::make_shared<CSpectralSampleAngle>(aSample, t_Angle);
            m_SpectralProperties.push_back(aSpectralSampleAngle);
        }

        return aSample;
    }

}   // namespace SpectralAveraging
