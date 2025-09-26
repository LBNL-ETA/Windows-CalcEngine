#ifndef AbsorptancesMultiPane_H
#define AbsorptancesMultiPane_H

#include <vector>
#include <WCECommon.hpp>

namespace MultiLayerOptics
{
    //! \brief Calculate absorptances of multiplane layers for simple case (single incident angle)
    class CAbsorptancesMultiPane
    {
    public:
        CAbsorptancesMultiPane(const FenestrationCommon::CSeries & t_T,
                               const FenestrationCommon::CSeries & t_Rf,
                               const FenestrationCommon::CSeries & t_Rb);

        void addLayer(const FenestrationCommon::CSeries & t_T,
                      const FenestrationCommon::CSeries & t_Rf,
                      const FenestrationCommon::CSeries & t_Rb);

        FenestrationCommon::CSeries Abs(size_t Index, FenestrationCommon::Side side);
        size_t numOfLayers();

        FenestrationCommon::CSeries iplus(size_t Index, FenestrationCommon::Side side);
        FenestrationCommon::CSeries iminus(size_t Index, FenestrationCommon::Side side);

        void calculateRTCoefficients();
        void calculateNormalizedRadiances();
        void calculateAbsorptances();

    private:
        void calculateState();

        FenestrationCommon::CSeries rCoeffs(const FenestrationCommon::CSeries & t_T,
                                            const FenestrationCommon::CSeries & t_Rf,
                                            const FenestrationCommon::CSeries & t_Rb,
                                            const FenestrationCommon::CSeries & t_RCoeffs);

        FenestrationCommon::CSeries tCoeffs(const FenestrationCommon::CSeries & t_T,
                                            const FenestrationCommon::CSeries & t_Rb,
                                            const FenestrationCommon::CSeries & t_RCoeffs);

        // This are measured values
        std::vector<FenestrationCommon::CSeries> m_T;

        util::EnumArray<FenestrationCommon::Side, std::vector<FenestrationCommon::CSeries>> m_R;

        // Calculated values are stored here
        util::EnumArray<FenestrationCommon::Side, std::vector<FenestrationCommon::CSeries>> m_Abs;

        util::EnumArray<FenestrationCommon::Side, std::vector<FenestrationCommon::CSeries>>
          m_rCoeffs;
        util::EnumArray<FenestrationCommon::Side, std::vector<FenestrationCommon::CSeries>>
          m_tCoeffs;

        util::EnumArray<FenestrationCommon::Side, std::vector<FenestrationCommon::CSeries>> Iplus;
        util::EnumArray<FenestrationCommon::Side, std::vector<FenestrationCommon::CSeries>> Iminus;

        bool m_StateCalculated;
    };
}   // namespace MultiLayerOptics

#endif
