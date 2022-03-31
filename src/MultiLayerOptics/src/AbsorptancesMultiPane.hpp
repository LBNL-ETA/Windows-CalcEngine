#ifndef AbsorptancesMultiPane_H
#define AbsorptancesMultiPane_H

#include <memory>
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
        std::map<FenestrationCommon::Side, std::vector<FenestrationCommon::CSeries>> m_R;

        // Calculated values are stored here
        std::map<FenestrationCommon::Side, std::vector<FenestrationCommon::CSeries>> m_Abs;

        std::map<FenestrationCommon::Side, std::vector<FenestrationCommon::CSeries>> m_rCoeffs;
        std::map<FenestrationCommon::Side, std::vector<FenestrationCommon::CSeries>> m_tCoeffs;

        std::map<FenestrationCommon::Side, std::vector<FenestrationCommon::CSeries>> Iplus;
        std::map<FenestrationCommon::Side, std::vector<FenestrationCommon::CSeries>> Iminus;

        bool m_StateCalculated;
    };
}   // namespace MultiLayerOptics

#endif
