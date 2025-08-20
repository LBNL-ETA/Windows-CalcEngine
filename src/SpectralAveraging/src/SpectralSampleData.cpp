#include <array>
#include <utility>

#include <WCECommon.hpp>

#include "SpectralSampleData.hpp"

using namespace FenestrationCommon;

namespace SpectralAveraging
{
    namespace
    {
        // Data coming should already been checked. However, this is just additional prevention
        double clamp01(double v)
        {
            return std::min(1.0, std::max(0.0, v));
        }
    }   // namespace

    ////////////////////////////////////////////////////////////////////////////
    ////     SampleData
    ////////////////////////////////////////////////////////////////////////////

    bool SampleData::Flipped() const
    {
        return m_Flipped;
    }

    void SampleData::Flipped(bool t_Flipped)
    {
        m_Flipped = t_Flipped;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////     CSpectralSampleData
    ////////////////////////////////////////////////////////////////////////////

    CSpectralSampleData::CSpectralSampleData() : SampleData()
    {
        for(const auto & side : allSides())
        {
            for(auto m : allMeasurements)
            {
                m_Property[key(PropertySimple::T, side, m)] = CSeries();
                m_Property[key(PropertySimple::R, side, m)] = CSeries();
            }
        }
    }

    void CSpectralSampleData::addRecord(double const t_Wavelength,
                                        double const t_TransmittanceFront,
                                        double const t_TransmittanceBack,
                                        double const t_ReflectanceFront,
                                        double const t_ReflectanceBack)
    {
        m_Property.at(key(PropertySimple::T, Side::Front, MeasurementType::Direct))
          .addProperty(t_Wavelength, t_TransmittanceFront);
        m_Property.at(key(PropertySimple::T, Side::Back, MeasurementType::Direct))
          .addProperty(t_Wavelength, t_TransmittanceBack);
        m_Property.at(key(PropertySimple::R, Side::Front, MeasurementType::Direct))
          .addProperty(t_Wavelength, t_ReflectanceFront);
        m_Property.at(key(PropertySimple::R, Side::Back, MeasurementType::Direct))
          .addProperty(t_Wavelength, t_ReflectanceBack);

        m_Property.at(key(PropertySimple::T, Side::Front, MeasurementType::Diffuse))
          .addProperty(t_Wavelength, 0);
        m_Property.at(key(PropertySimple::T, Side::Back, MeasurementType::Diffuse))
          .addProperty(t_Wavelength, 0);
        m_Property.at(key(PropertySimple::R, Side::Front, MeasurementType::Diffuse))
          .addProperty(t_Wavelength, 0);
        m_Property.at(key(PropertySimple::R, Side::Back, MeasurementType::Diffuse))
          .addProperty(t_Wavelength, 0);
    }

    void CSpectralSampleData::addRecord(double t_Wavelength,
                                        const OpticalProperties & direct,
                                        const OpticalProperties & diffuse)
    {
        // --- Direct ---
        m_Property.at(key(PropertySimple::T, Side::Front, MeasurementType::Direct))
          .addProperty(t_Wavelength, direct.Tf);
        m_Property.at(key(PropertySimple::T, Side::Back, MeasurementType::Direct))
          .addProperty(t_Wavelength, direct.Tb);
        m_Property.at(key(PropertySimple::R, Side::Front, MeasurementType::Direct))
          .addProperty(t_Wavelength, direct.Rf);
        m_Property.at(key(PropertySimple::R, Side::Back, MeasurementType::Direct))
          .addProperty(t_Wavelength, direct.Rb);

        // --- Diffuse ---
        m_Property.at(key(PropertySimple::T, Side::Front, MeasurementType::Diffuse))
          .addProperty(t_Wavelength, diffuse.Tf);
        m_Property.at(key(PropertySimple::T, Side::Back, MeasurementType::Diffuse))
          .addProperty(t_Wavelength, diffuse.Tb);
        m_Property.at(key(PropertySimple::R, Side::Front, MeasurementType::Diffuse))
          .addProperty(t_Wavelength, diffuse.Rf);
        m_Property.at(key(PropertySimple::R, Side::Back, MeasurementType::Diffuse))
          .addProperty(t_Wavelength, diffuse.Rb);
    }


    CSpectralSampleData::CSpectralSampleData(const std::vector<MeasuredRow> & tValues) :
        CSpectralSampleData()
    {
        for(auto m : allMeasurements)
        {
            m_Property.at(key(PropertySimple::T, Side::Front, m)).clear();
            m_Property.at(key(PropertySimple::T, Side::Back, m)).clear();
            m_Property.at(key(PropertySimple::R, Side::Front, m)).clear();
            m_Property.at(key(PropertySimple::R, Side::Back, m)).clear();
        }

        for(const auto & val : tValues)
        {
            // DIRECT
            m_Property.at(key(PropertySimple::T, Side::Front, MeasurementType::Direct))
              .addProperty(val.wavelength, val.direct.Tf);
            m_Property.at(key(PropertySimple::T, Side::Back, MeasurementType::Direct))
              .addProperty(val.wavelength, val.direct.Tb);
            m_Property.at(key(PropertySimple::R, Side::Front, MeasurementType::Direct))
              .addProperty(val.wavelength, val.direct.Rf);
            m_Property.at(key(PropertySimple::R, Side::Back, MeasurementType::Direct))
              .addProperty(val.wavelength, val.direct.Rb);

            // DIFFUSE
            m_Property.at(key(PropertySimple::T, Side::Front, MeasurementType::Diffuse))
              .addProperty(val.wavelength, val.diffuse.Tf);
            m_Property.at(key(PropertySimple::T, Side::Back, MeasurementType::Diffuse))
              .addProperty(val.wavelength, val.diffuse.Tb);
            m_Property.at(key(PropertySimple::R, Side::Front, MeasurementType::Diffuse))
              .addProperty(val.wavelength, val.diffuse.Rf);
            m_Property.at(key(PropertySimple::R, Side::Back, MeasurementType::Diffuse))
              .addProperty(val.wavelength, val.diffuse.Rb);
        }
    }

    CSeries CSpectralSampleData::properties(Property prop, Side side)
    {
        auto aSide = getSide(side, Flipped());
        if(prop == Property::T || prop == Property::R)
        {
            PropertySimple simpleProp =
              (prop == Property::T) ? PropertySimple::T : PropertySimple::R;
            const auto & direct = m_Property.at(key(simpleProp, aSide, MeasurementType::Direct));
            const auto & diffuse = m_Property.at(key(simpleProp, aSide, MeasurementType::Diffuse));
            return direct + diffuse;
        }
        else if(prop == Property::Abs)
        {
            const auto & tDirect =
              m_Property.at(key(PropertySimple::T, aSide, MeasurementType::Direct));
            const auto & tDiffuse =
              m_Property.at(key(PropertySimple::T, aSide, MeasurementType::Diffuse));
            const auto & rDirect =
              m_Property.at(key(PropertySimple::R, aSide, MeasurementType::Direct));
            const auto & rDiffuse =
              m_Property.at(key(PropertySimple::R, aSide, MeasurementType::Diffuse));
            return 1.0 - (tDirect + tDiffuse + rDirect + rDiffuse);
        }

        return CSeries();
    }

    std::vector<double> CSpectralSampleData::getWavelengths() const
    {
        const auto & s =
          m_Property.at(key(PropertySimple::T, Side::Front, MeasurementType::Direct));
        return s.size() ? s.getXArray() : std::vector<double>{};
    }

    Limits CSpectralSampleData::getWavelengthLimits() const
    {
        const auto wl = getWavelengths();
        return wl.empty() ? Limits{0.0, 0.0} : Limits{wl.front(), wl.back()};
    }

    // Interpolate current sample data to new wavelengths set
    void CSpectralSampleData::interpolate(std::vector<double> const & t_Wavelengths)
    {
        // T/R for all measurements
        for(const auto & side : allSides())
        {
            for(auto m : allMeasurements)
            {
                m_Property[key(PropertySimple::T, side, m)] =
                  m_Property.at(key(PropertySimple::T, side, m)).interpolate(t_Wavelengths);
                m_Property[key(PropertySimple::R, side, m)] =
                  m_Property.at(key(PropertySimple::R, side, m)).interpolate(t_Wavelengths);
            }
        }
    }


    std::shared_ptr<CSpectralSampleData>
      CSpectralSampleData::create(const std::vector<MeasuredRow> & tValues)
    {
        return std::make_shared<CSpectralSampleData>(tValues);
    }

    std::shared_ptr<CSpectralSampleData> CSpectralSampleData::create()
    {
        return create({});
    }

    void CSpectralSampleData::cutExtraData(const double minLambda, const double maxLambda)
    {
        for(const auto & side : allSides())
        {
            for(auto m : allMeasurements)
            {
                m_Property.at(key(PropertySimple::T, side, m)).cutExtraData(minLambda, maxLambda);
                m_Property.at(key(PropertySimple::R, side, m)).cutExtraData(minLambda, maxLambda);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// PhotovoltaicData
    ///////////////////////////////////////////////////////////////////////////
    PhotovoltaicSampleData::PhotovoltaicSampleData(const CSpectralSampleData & spectralSampleData) :
        CSpectralSampleData(spectralSampleData),
        m_EQE{{Side::Front, CSeries()}, {Side::Back, CSeries()}}
    {}

    PhotovoltaicSampleData::PhotovoltaicSampleData(const CSpectralSampleData & spectralSampleData,
                                                   const CSeries & eqeValuesFront,
                                                   const CSeries & eqeValuesBack) :
        CSpectralSampleData(spectralSampleData),
        m_EQE{{Side::Front, eqeValuesFront}, {Side::Back, eqeValuesBack}}
    {}

    void PhotovoltaicSampleData::cutExtraData(double minLambda, double maxLambda)
    {
        CSpectralSampleData::cutExtraData(minLambda, maxLambda);
        for(const auto & side : allSides())
        {
            m_EQE.at(side).cutExtraData(minLambda, maxLambda);
        }
    }

    CSeries PhotovoltaicSampleData::eqe(const Side side) const
    {
        return m_EQE.at(side);
    }

}   // namespace SpectralAveraging
