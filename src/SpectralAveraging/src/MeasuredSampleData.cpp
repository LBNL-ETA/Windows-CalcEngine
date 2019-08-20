#include <stdexcept>
#include <cassert>

#include "MeasuredSampleData.hpp"
#include "WCECommon.hpp"


using namespace FenestrationCommon;

namespace SpectralAveraging
{
    ////////////////////////////////////////////////////////////////////////////
    ////     CSpectralSampleData
    ////////////////////////////////////////////////////////////////////////////

    CSpectralSampleData::CSpectralSampleData() : m_Flipped(false), m_absCalculated(false)
    {
        for(const auto & prop : EnumProperty())
        {
            for(const auto & side : EnumSide())
            {
                m_Property[std::make_pair(prop, side)] = std::make_shared<CSeries>();
            }
        }
    }

    void CSpectralSampleData::addRecord(double const t_Wavelength,
                                        double const t_Transmittance,
                                        double const t_ReflectanceFront,
                                        double const t_ReflectanceBack)
    {
        m_Property.at(std::make_pair(Property::T, Side::Front))
          ->addProperty(t_Wavelength, t_Transmittance);
        m_Property.at(std::make_pair(Property::T, Side::Back))
          ->addProperty(t_Wavelength, t_Transmittance);
        m_Property.at(std::make_pair(Property::R, Side::Front))
          ->addProperty(t_Wavelength, t_ReflectanceFront);
        m_Property.at(std::make_pair(Property::R, Side::Back))
          ->addProperty(t_Wavelength, t_ReflectanceBack);
        reset();
    }


    CSpectralSampleData::CSpectralSampleData(const std::vector<MeasuredRow> & tValues) :
        CSpectralSampleData()
    {
        m_Property.at(std::make_pair(Property::T, Side::Front))->clear();
        m_Property.at(std::make_pair(Property::T, Side::Back))->clear();
        m_Property.at(std::make_pair(Property::R, Side::Front))->clear();
        m_Property.at(std::make_pair(Property::R, Side::Back))->clear();
        for(const auto & val : tValues)
        {
            m_Property.at(std::make_pair(Property::T, Side::Front))
              ->addProperty(val.wavelength, val.T);
            m_Property.at(std::make_pair(Property::T, Side::Back))
              ->addProperty(val.wavelength, val.T);
            m_Property.at(std::make_pair(Property::R, Side::Front))
              ->addProperty(val.wavelength, val.Rf);
            m_Property.at(std::make_pair(Property::R, Side::Back))
              ->addProperty(val.wavelength, val.Rb);
        }
    }

    std::shared_ptr<CSeries> CSpectralSampleData::properties(FenestrationCommon::Property prop,
                                                             FenestrationCommon::Side side)
    {
        calculateProperties();
        auto aSide = FenestrationCommon::getSide(side, m_Flipped);
        return m_Property.at(std::make_pair(prop, aSide));
    }

    std::vector<double> CSpectralSampleData::getWavelengths() const
    {
        return m_Property.at(std::make_pair(Property::T, Side::Front))->getXArray();
    }

    // Interpolate current sample data to new wavelengths set
    void CSpectralSampleData::interpolate(std::vector<double> const & t_Wavelengths)
    {
        for(const auto & prop : EnumProperty())
        {
            for(const auto & side : EnumSide())
            {
                m_Property[std::make_pair(prop, side)] =
                  m_Property.at(std::make_pair(prop, side))->interpolate(t_Wavelengths);
            }
        }
    }

    bool CSpectralSampleData::Flipped() const
    {
        return m_Flipped;
    }

    void CSpectralSampleData::Filpped(bool const t_Flipped)
    {
        m_Flipped = t_Flipped;
    }

    void CSpectralSampleData::reset()
    {
        m_absCalculated = false;
    }

    void CSpectralSampleData::calculateProperties()
    {
        if(!m_absCalculated)
        {
            std::shared_ptr<CSeries> reflectancesFront = nullptr;
            std::shared_ptr<CSeries> reflectancesBack = nullptr;
            if(m_Flipped)
            {
                reflectancesFront = m_Property.at(std::make_pair(Property::R, Side::Back));
                reflectancesBack = m_Property.at(std::make_pair(Property::R, Side::Front));
            }
            else
            {
                reflectancesFront = m_Property.at(std::make_pair(Property::R, Side::Front));
                reflectancesBack = m_Property.at(std::make_pair(Property::R, Side::Back));
            }

            m_Property.at(std::make_pair(Property::Abs, Side::Front))->clear();
            m_Property.at(std::make_pair(Property::Abs, Side::Back))->clear();

            const auto wv = m_Property.at(std::make_pair(Property::T, Side::Front))->getXArray();

            for(size_t i = 0; i < wv.size(); ++i)
            {
                auto value = 1
                             - (*m_Property.at(std::make_pair(Property::T, Side::Front)))[i].value()
                             - (*reflectancesFront)[i].value();
                m_Property.at(std::make_pair(Property::Abs, Side::Front))
                  ->addProperty(wv[i], value);

                value = 1 - (*m_Property.at(std::make_pair(Property::T, Side::Back)))[i].value()
                        - (*reflectancesBack)[i].value();
                m_Property.at(std::make_pair(Property::Abs, Side::Back))->addProperty(wv[i], value);
            }
            m_absCalculated = true;
        }
    }


    std::shared_ptr<CSpectralSampleData>
      CSpectralSampleData::create(const std::vector<MeasuredRow> & tValues)
    {
        return std::shared_ptr<CSpectralSampleData>(new CSpectralSampleData(tValues));
    }

    std::shared_ptr<CSpectralSampleData> CSpectralSampleData::create()
    {
        return CSpectralSampleData::create({});
    }

    void CSpectralSampleData::cutExtraData(const double minLambda, const double maxLambda)
    {
        for(const auto & side : EnumSide())
        {
            m_Property.at(std::make_pair(Property::T, side))->cutExtraData(minLambda, maxLambda);
            m_Property.at(std::make_pair(Property::R, side))->cutExtraData(minLambda, maxLambda);
        }
    }

    MeasuredRow::MeasuredRow(double wl, double t, double rf, double rb) :
        wavelength(wl),
        T(t),
        Rf(rf),
        Rb(rb)
    {}
}   // namespace SpectralAveraging
