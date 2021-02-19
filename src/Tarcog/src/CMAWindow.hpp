#pragma once

#include "CMAInterface.hpp"
#include "Frame.hpp"
#include "WholeWindow.hpp"
#include "WholeWindowConfigurations.hpp"
#include "CMASettings.hpp"

namespace CMA
{
    //////////////////////////////////////////
    //  SpacerGlass
    //////////////////////////////////////////
    class SpacerGlass
    {
    public:
        SpacerGlass(double bestUValue,
                    double worstUValue,
                    double bestSpacerKeff = 0.01,
                    double worstSpacerKeff = 10);

        [[nodiscard]] double uValue(Option option) const;
        [[nodiscard]] double spacerKeff(Option option) const;

    private:
        std::map<CMAProperty, BestWorst<double>> m_Options;
    };

    //////////////////////////////////////////
    //  CMAFrame
    //////////////////////////////////////////
    class CMAFrame
    {
    public:
        CMAFrame(Tarcog::ISO15099::FrameData bestSpacerBestIGU,
                 Tarcog::ISO15099::FrameData bestSpacerWorstIGU,
                 Tarcog::ISO15099::FrameData worstSpacerBestIGU,
                 Tarcog::ISO15099::FrameData worstSpacerWorstIGU);

        Tarcog::ISO15099::FrameData getFrame(Option spacerOption, Option iguOption);

    private:
        // First option is by spacer and second option (inside BestWorst) is by IGU
        std::map<Option, BestWorst<Tarcog::ISO15099::FrameData>> m_Frame;
    };

    //////////////////////////////////////////
    //  CMAWindow
    //////////////////////////////////////////
    class CMAWindow : public ICMAWindow
    {
    public:
        CMAWindow(double spacerBestKeff = 0.01,
                  double spacerWorstKeff = 10.0,
                  CMABestWorstUFactors bestUFactor = CreateBestWorstUFactorOption(Option::Best),
                  CMABestWorstUFactors worstUFactor = CreateBestWorstUFactorOption(Option::Worst));

        [[nodiscard]] double vt() override;
        [[nodiscard]] double uValue(double Ucog, double keffSpacer) override;
        [[nodiscard]] double shgc(double SHGCcog, double keffSpacer) override;

    protected:
        [[nodiscard]] double Ub(double spacerKeff);
        [[nodiscard]] double Uw(double spacerKeff);
        [[nodiscard]] double SHGCb(double spacerKeff);
        [[nodiscard]] double SHGCw(double spacerKeff);

        [[nodiscard]] virtual Tarcog::IWindow & windowAt(Option spacer, Option glazing) = 0;

        std::map<Option, CMABestWorstUFactors> m_BestWorstIGUUvalues;

        BestWorst<double> m_Spacer;
    };

    //////////////////////////////////////////
    //  CMAWindowSingleVision
    //////////////////////////////////////////
    class CMAWindowSingleVision : public CMAWindow
    {
    public:
        CMAWindowSingleVision(
          double width,
          double height,
          double tvis,
          double tsol,
          double spacerBestKeff = 0.01,
          double spacerWorstKeff = 10.0,
          CMABestWorstUFactors bestUFactor = CreateBestWorstUFactorOption(Option::Best),
          CMABestWorstUFactors worstUFactor = CreateBestWorstUFactorOption(Option::Worst));

        void setFrameTop(CMAFrame cmaFrameData);
        void setFrameBottom(CMAFrame cmaFrameData);
        void setFrameLeft(CMAFrame cmaFrameData);
        void setFrameRight(CMAFrame cmaFrameData);
        void setDividers(CMAFrame frameData, size_t nHorizontal, size_t nVertical);

    private:
        //! Single vision windows needs to create this structure, otherwise it will not work
        [[nodiscard]] std::map<Option, std::map<Option, Tarcog::ISO15099::WindowSingleVision>>
          createBestWorstWindows(double width,
                                 double height,
                                 double tvis,
                                 double tsol,
                                 CMABestWorstUFactors bestUFactor,
                                 CMABestWorstUFactors worstUFactor) const;

        Tarcog::IWindow & windowAt(Option spacer, Option glazing) override;

        //! First option is spacer and second option is glass (best, worst)
        std::map<Option, std::map<Option, Tarcog::ISO15099::WindowSingleVision>> m_Window;
    };


}   // namespace CMA
