#include <array>
#include <utility>

#include <WCECommon.hpp>

#include "SpectralSampleData.hpp"

using namespace FenestrationCommon;

namespace SpectralAveraging
{
    namespace
    {
        // Abs exists only for Total
        constexpr std::array<MeasurementType, 1> kAbsMeasurements{MeasurementType::Total};

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

    CSpectralSampleData::CSpectralSampleData() : SampleData(), m_absCalculated(false)
    {
        // T/R for Direct, Diffuse, Total
        for(const auto & side : allSides())
        {
            for(auto m : kTRMeasurements)
            {
                m_Property[key(Property::T, side, m)] = CSeries();
                m_Property[key(Property::R, side, m)] = CSeries();
            }
        }

        // Abs ONLY for Total
        for(const auto & side : allSides())
        {
            m_Property[key(Property::Abs, side, MeasurementType::Total)] = CSeries();
        }
    }

    void CSpectralSampleData::addRecord(double const t_Wavelength,
                                        double const t_TransmittanceFront,
                                        double const t_TransmittanceBack,
                                        double const t_ReflectanceFront,
                                        double const t_ReflectanceBack)
    {
        m_Property.at(key(Property::T, Side::Front, MeasurementType::Total))
          .addProperty(t_Wavelength, t_TransmittanceFront);
        m_Property.at(key(Property::T, Side::Back, MeasurementType::Total))
          .addProperty(t_Wavelength, t_TransmittanceBack);
        m_Property.at(key(Property::R, Side::Front, MeasurementType::Total))
          .addProperty(t_Wavelength, t_ReflectanceFront);
        m_Property.at(key(Property::R, Side::Back, MeasurementType::Total))
          .addProperty(t_Wavelength, t_ReflectanceBack);
        reset();
    }

    void CSpectralSampleData::addRecord(double t_Wavelength,
                                        const OpticalProperties & direct,
                                        const OpticalProperties & diffuse)
    {
        // --- Direct ---
        m_Property.at(key(Property::T, Side::Front, MeasurementType::Direct))
          .addProperty(t_Wavelength, direct.Tf);
        m_Property.at(key(Property::T, Side::Back, MeasurementType::Direct))
          .addProperty(t_Wavelength, direct.Tb);
        m_Property.at(key(Property::R, Side::Front, MeasurementType::Direct))
          .addProperty(t_Wavelength, direct.Rf);
        m_Property.at(key(Property::R, Side::Back, MeasurementType::Direct))
          .addProperty(t_Wavelength, direct.Rb);

        // --- Diffuse ---
        m_Property.at(key(Property::T, Side::Front, MeasurementType::Diffuse))
          .addProperty(t_Wavelength, diffuse.Tf);
        m_Property.at(key(Property::T, Side::Back, MeasurementType::Diffuse))
          .addProperty(t_Wavelength, diffuse.Tb);
        m_Property.at(key(Property::R, Side::Front, MeasurementType::Diffuse))
          .addProperty(t_Wavelength, diffuse.Rf);
        m_Property.at(key(Property::R, Side::Back, MeasurementType::Diffuse))
          .addProperty(t_Wavelength, diffuse.Rb);

        const auto Tf_tot = clamp01(direct.Tf + diffuse.Tf);
        const auto Tb_tot = clamp01(direct.Tb + diffuse.Tb);
        const auto Rf_tot = clamp01(direct.Rf + diffuse.Rf);
        const auto Rb_tot = clamp01(direct.Rb + diffuse.Rb);

        m_Property.at(key(Property::T, Side::Front, MeasurementType::Total))
          .addProperty(t_Wavelength, Tf_tot);
        m_Property.at(key(Property::T, Side::Back, MeasurementType::Total))
          .addProperty(t_Wavelength, Tb_tot);
        m_Property.at(key(Property::R, Side::Front, MeasurementType::Total))
          .addProperty(t_Wavelength, Rf_tot);
        m_Property.at(key(Property::R, Side::Back, MeasurementType::Total))
          .addProperty(t_Wavelength, Rb_tot);

        // Invalidate absorptance — will be recomputed lazily
        reset();
    }


    CSpectralSampleData::CSpectralSampleData(const std::vector<MeasuredRow> & tValues) :
        CSpectralSampleData()
    {
        // Clear all T/R series first (for all measurements)
        for(auto m : kTRMeasurements)
        {
            m_Property.at(key(Property::T, Side::Front, m)).clear();
            m_Property.at(key(Property::T, Side::Back, m)).clear();
            m_Property.at(key(Property::R, Side::Front, m)).clear();
            m_Property.at(key(Property::R, Side::Back, m)).clear();
        }

        for(const auto & val : tValues)
        {
            // DIRECT
            m_Property.at(key(Property::T, Side::Front, MeasurementType::Direct))
              .addProperty(val.wavelength, val.direct.Tf);
            m_Property.at(key(Property::T, Side::Back, MeasurementType::Direct))
              .addProperty(val.wavelength, val.direct.Tb);
            m_Property.at(key(Property::R, Side::Front, MeasurementType::Direct))
              .addProperty(val.wavelength, val.direct.Rf);
            m_Property.at(key(Property::R, Side::Back, MeasurementType::Direct))
              .addProperty(val.wavelength, val.direct.Rb);

            // DIFFUSE
            m_Property.at(key(Property::T, Side::Front, MeasurementType::Diffuse))
              .addProperty(val.wavelength, val.diffuse.Tf);
            m_Property.at(key(Property::T, Side::Back, MeasurementType::Diffuse))
              .addProperty(val.wavelength, val.diffuse.Tb);
            m_Property.at(key(Property::R, Side::Front, MeasurementType::Diffuse))
              .addProperty(val.wavelength, val.diffuse.Rf);
            m_Property.at(key(Property::R, Side::Back, MeasurementType::Diffuse))
              .addProperty(val.wavelength, val.diffuse.Rb);

            // TOTAL = DIRECT + DIFFUSE (component-wise, clamped)
            const auto Tf_tot = clamp01(val.direct.Tf + val.diffuse.Tf);
            const auto Tb_tot = clamp01(val.direct.Tb + val.diffuse.Tb);
            const auto Rf_tot = clamp01(val.direct.Rf + val.diffuse.Rf);
            const auto Rb_tot = clamp01(val.direct.Rb + val.diffuse.Rb);

            m_Property.at(key(Property::T, Side::Front, MeasurementType::Total))
              .addProperty(val.wavelength, Tf_tot);
            m_Property.at(key(Property::T, Side::Back, MeasurementType::Total))
              .addProperty(val.wavelength, Tb_tot);
            m_Property.at(key(Property::R, Side::Front, MeasurementType::Total))
              .addProperty(val.wavelength, Rf_tot);
            m_Property.at(key(Property::R, Side::Back, MeasurementType::Total))
              .addProperty(val.wavelength, Rb_tot);
        }

        reset();
    }

    CSeries & CSpectralSampleData::properties(Property prop, Side side, MeasurementType type)
    {
        calculateProperties();
        if (prop == Property::Abs && type != MeasurementType::Total)
            type = MeasurementType::Total;     // redirect for absorptance only
        auto aSide = getSide(side, Flipped());
        return m_Property.at(key(prop, aSide, type));
    }

    std::vector<double> CSpectralSampleData::getWavelengths() const
    {
        const auto & s = m_Property.at(key(Property::T, Side::Front, MeasurementType::Total));
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
            for(auto m : kTRMeasurements)
            {
                m_Property[key(Property::T, side, m)] =
                  m_Property.at(key(Property::T, side, m)).interpolate(t_Wavelengths);
                m_Property[key(Property::R, side, m)] =
                  m_Property.at(key(Property::R, side, m)).interpolate(t_Wavelengths);
            }
        }
        // Abs only for Total
        for(const auto & side : allSides())
        {
            m_Property[key(Property::Abs, side, MeasurementType::Total)] =
              m_Property.at(key(Property::Abs, side, MeasurementType::Total))
                .interpolate(t_Wavelengths);
        }
        reset();
    }

    void CSpectralSampleData::reset()
    {
        m_absCalculated = false;
    }

    void CSpectralSampleData::calculateProperties()
    {
        if(m_absCalculated)
            return;

        // Clear Abs (Total only)
        for(const auto & side : allSides())
        {
            m_Property.at(key(Property::Abs, side, MeasurementType::Total)).clear();
        }

        // Use Total wavelength grid
        const auto wv =
          m_Property.at(key(Property::T, Side::Front, MeasurementType::Total)).getXArray();

        // Flip-aware sides for R selection
        const auto RFrontSide = Flipped() ? Side::Back : Side::Front;
        const auto RBackSide = Flipped() ? Side::Front : Side::Back;

        const auto & T_F = m_Property.at(key(Property::T, Side::Front, MeasurementType::Total));
        const auto & T_B = m_Property.at(key(Property::T, Side::Back, MeasurementType::Total));
        const auto & R_F = m_Property.at(key(Property::R, RFrontSide, MeasurementType::Total));
        const auto & R_B = m_Property.at(key(Property::R, RBackSide, MeasurementType::Total));

        const size_t n = T_F.size();
        if(!n)
        {
            m_absCalculated = true;
            return;
        }   // nothing to do

        // (Optional) sanity: ensure alignment
        if(T_B.size() != n || R_F.size() != n || R_B.size() != n)
        {
            throw std::runtime_error(
              "CSpectralSampleData: Total series misaligned for Abs computation.");
        }

        auto & A_F = m_Property.at(key(Property::Abs, Side::Front, MeasurementType::Total));
        auto & A_B = m_Property.at(key(Property::Abs, Side::Back, MeasurementType::Total));

        for(size_t i = 0; i < n; ++i)
        {
            const double wl = wv[i];
            const double t_f = clamp01(T_F[i].value());
            const double t_b = clamp01(T_B[i].value());
            const double r_f = clamp01(R_F[i].value());
            const double r_b = clamp01(R_B[i].value());
            A_F.addProperty(wl, clamp01(1.0 - t_f - r_f));
            A_B.addProperty(wl, clamp01(1.0 - t_b - r_b));
        }

        m_absCalculated = true;
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
            for(auto m : kTRMeasurements)
            {
                m_Property.at(key(Property::T, side, m)).cutExtraData(minLambda, maxLambda);
                m_Property.at(key(Property::R, side, m)).cutExtraData(minLambda, maxLambda);
            }
        }
        for(const auto & side : allSides())
        {
            m_Property.at(key(Property::Abs, side, MeasurementType::Total))
              .cutExtraData(minLambda, maxLambda);
        }
        reset();
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
    {

    }

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
