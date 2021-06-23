#include <utility>
#include <algorithm>
#include <functional>
#include <cmath>

#include <WCECommon.hpp>
#include <WCETarcog.hpp>

namespace Deflection
{
    LayerData::LayerData(const double thickness, const double density, double modulusOfElasticity) :
        thickness(thickness * 1000),
        density(density),
        modulusOfElasticity(modulusOfElasticity / 1000)
    {}

    GapData::GapData(double thickness, double initialTemperature, double initialPressure) :
        thickness(thickness * 1000),
        initialTemperature(initialTemperature),
        initialPressure(initialPressure / 1000)
    {}

    DeflectionResults::DeflectionResults(std::optional<double> error,
                                         const std::vector<double> & deflection) :
        error(std::move(error)), deflection(deflection)
    {}

    DeflectionE1300::DeflectionE1300(double width,
                                     double height,
                                     std::vector<LayerData> layer,
                                     std::vector<GapData> gap) :
        m_LongDimension(width > height ? width * 1000 : height * 1000),
        m_ShortDimension(width > height ? height * 1000 : width * 1000),
        m_Layer(std::move(layer)),
        m_Gap(std::move(gap)),
        m_PsLoaded(getPsLoaded(m_Layer, m_Theta)),
        m_Pcs(calcPcs(m_ShortDimension, m_Layer)),
        m_Vcs(calcVcs(m_ShortDimension, m_Layer)),
        m_PnVns(Table::columnInterpolation(DeflectionData::getVNData(),
                                           m_LongDimension / m_ShortDimension))
    {
        for(auto & val : m_PnVns)
        {
            if(val.x.has_value())
            {
                val.x = std::exp(val.x.value());
            }

            if(val.y.has_value())
            {
                val.y = std::exp(val.y.value());
            }
        }
        // Provided tables do not include zero points and they are used for deflection calculations.
        m_PnVns.insert(m_PnVns.begin(), {0, 0});
    }

    [[maybe_unused]] void DeflectionE1300::setExteriorPressure(const double pressure)
    {
        m_ExteriorPressure = pressure / 1000;
    }

    [[maybe_unused]] void DeflectionE1300::setInteriorPressure(const double pressure)
    {
        m_InteriorPressure = pressure / 1000;
    }

    [[maybe_unused]] void DeflectionE1300::setIGUTheta(const double theta)
    {
        m_Theta = theta;
        m_PsLoaded = getPsLoaded(m_Layer, m_Theta);
    }

    [[maybe_unused]] void DeflectionE1300::setAppliedLoad(std::vector<double> appliedLoad)
    {
        m_AppliedLoad = std::move(appliedLoad);
    }

    [[maybe_unused]] void DeflectionE1300::setLoadTemperatures(std::vector<double> loadTemperature)
    {
        m_LoadTemperature = std::move(loadTemperature);
    }

    std::vector<double> DeflectionE1300::getPsWeight(const std::vector<LayerData> & layer,
                                                     double theta)
    {
        std::vector<double> result;
        const auto pi{std::atan(1) * 4};
        for(const auto & lay : layer)
        {
            result.push_back(lay.thickness / 1000 * lay.density * 9.81 / 1000
                             * std::cos((theta * pi) / 180));
        }
        return result;
    }

    std::vector<double> DeflectionE1300::getPsLoaded(const std::vector<LayerData> & layer,
                                                     double theta)
    {
        std::vector<double> result{DeflectionE1300::getPsWeight(layer, theta)};
        if(m_AppliedLoad.size() == result.size())
        {
            std::transform(
              result.begin(), result.end(), m_AppliedLoad.begin(), result.begin(), std::plus<>());
        }
        result[0] += m_ExteriorPressure;
        result[result.size() - 1] += m_InteriorPressure;
        return result;
    }

    std::vector<double> DeflectionE1300::calcPcs(double shortDimension,
                                                 const std::vector<LayerData> & layer)
    {
        std::vector<double> result;
        for(const auto & lay : layer)
        {
            result.emplace_back(std::pow(shortDimension / (2 * lay.thickness), 4)
                                / lay.modulusOfElasticity);
        }
        return result;
    }

    std::vector<double> DeflectionE1300::calcVcs(double shortDimension,
                                                 const std::vector<LayerData> & layer)
    {
        std::vector<double> result;
        for(const auto & lay : layer)
        {
            result.emplace_back(1 / (lay.thickness * std::pow(shortDimension / 2, 2)));
        }
        return result;
    }

    DeflectionResults DeflectionE1300::results()
    {
        auto dp1p{DP1pGuess(m_PsLoaded[0] - m_PsLoaded[m_PsLoaded.size() - 1], m_Layer)};
        auto dp1c{dp1p * 1.01};

        auto Dp{nIGU_Li(0u, 0, dp1p)};
        auto Dc{nIGU_Li(0u, 0, dp1c)};

        auto Errx{Dc.error.value()};
        std::vector<double> defX;

        if(!Dp.error.has_value() || !Dc.error.has_value())
        {
            throw std::runtime_error("Beyond Charts");
        }
        else
        {
            auto IterCnt{0u};
            do
            {
                auto dp1x{
                  dp1c - Dc.error.value() * (dp1c - dp1p) / (Dc.error.value() - Dp.error.value())};
                auto Dx{nIGU_Li(0, 0, dp1x)};
                Errx = Dx.error.value();
                dp1p = dp1c;
                Dp.error = Dc.error;
                dp1c = dp1x;
                Dc.error = Dx.error;
                IterCnt++;
                defX = Dx.deflection;
            } while(std::abs(Errx) > 0.001 && IterCnt < 500u);
        }

        return {Errx, defX};
    }

    double DeflectionE1300::DP1pGuess(double Pdiff, const std::vector<LayerData> & layer)
    {
        double result{0.01};
        if(Pdiff != 0)
        {
            auto sum{0.0};
            for(const auto & lay : layer)
            {
                sum += std::pow(lay.thickness, 3);
            }
            result = Pdiff * (std::pow(layer[0].thickness, 3) / sum);
        }
        return result;
    }

    DeflectionResults DeflectionE1300::nIGU_Li(size_t index, double PasiLoaded, double dpCoeff)
    {
        std::vector<double> DPs;

        auto j{index + 1u};

        auto DPni{dpCoeff * m_Pcs[index]};

        auto si{DPni > 0 ? 1.0 : -1.0};

        auto Vi{0.0};
        auto val{Table::tableColumnInterpolation(m_PnVns, DPni * si)};
        if(val.has_value())
        {
            Vi = val.value() / m_Vcs[index] * si;
        }

        auto Pai{m_PsLoaded[index] + PasiLoaded - dpCoeff};

        std::optional<double> Err0;

        if(index != m_Gap.size() - 1)
        {
            auto DPjp{dpCoeff / 2};
            auto DPjc{DPjp * 1.05};

            auto Defp{nIGU_Li(j, Pai, DPjp)};
            auto Defc{nIGU_Li(j, Pai, DPjc)};

            if(!Defp.error.has_value() || !Defc.error.has_value())
            {
                Err0 = Defc.error;
            }
            else
            {
                double Errx{Defc.error.value()};
                size_t IterCnt = 0u;
                auto DPjx{0.0};
                std::vector<double> DPsi;
                do
                {
                    DPjx = DPjc
                           - Defc.error.value() * (DPjc - DPjp)
                               / (Defc.error.value() - Defp.error.value());
                    auto Defx{nIGU_Li(j, Pai, DPjx)};
                    if(Defx.error.has_value())
                    {
                        Errx = Defx.error.value();
                    }
                    DPsi = Defx.deflection;
                    DPjp = DPjc;
                    Defp.error = Defc.error;
                    DPjc = DPjx;
                    Defc.error = Errx;
                    IterCnt++;
                } while(std::abs(Errx) > 0.001 && IterCnt < 500u);
                auto DPnj = DPjx * m_Pcs[j];

                auto sj{DPnj > 0 ? 1 : -1};
                auto Vj{0.0};
                auto val{Table::tableColumnInterpolation(m_PnVns, DPnj * sj)};
                if(val.has_value())
                {
                    Vj = val.value() / m_Vcs[j] * sj;
                }
                Err0 = ((m_Gap[index].initialPressure * m_LoadTemperature[index])
                        / (Pai * m_Gap[index].initialTemperature) - 1) * m_Gap[index].thickness
                       * m_ShortDimension * m_LongDimension
                       + Vi - Vj;
                DPs.emplace_back(dpCoeff);
                for(const auto & val : DPsi)
                {
                    DPs.emplace_back(val);
                }
            }
        }
        else
        {
            auto DPj = Pai - m_PsLoaded[j];
            auto DPnj = DPj * m_Pcs[j];
            auto sj{DPnj > 0 ? 1.0 : -1.0};
            auto Vj{0.0};
            auto val{Table::tableColumnInterpolation(m_PnVns, DPnj * sj)};
            if(val.has_value())
            {
                Vj = val.value() / m_Vcs[j] * sj;
            }
            Err0 = ((m_Gap[index].initialPressure * m_LoadTemperature[index])
                      / (Pai * m_Gap[index].initialTemperature) - 1) * m_Gap[index].thickness
                      * m_ShortDimension * m_LongDimension
                    + Vi - Vj;
            DPs.emplace_back(dpCoeff);
            DPs.emplace_back(DPj);
        }

        return {Err0, DPs};
    }
}   // namespace Deflection