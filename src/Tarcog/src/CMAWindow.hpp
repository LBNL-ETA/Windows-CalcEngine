#pragma once

#include "CMAInterface.hpp"
#include "Frame.hpp"
#include "WholeWindow.hpp"
#include "WholeWindowConfigurations.hpp"
#include "CMASettings.hpp"

namespace CMA
{
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

        [[nodiscard]] double vt(double tVis) override;
        [[nodiscard]] double uValue(double Ucog, double keffSpacer) override;
        [[nodiscard]] double shgc(double SHGCcog, double keffSpacer) override;

        [[nodiscard]] Tarcog::IGUDimensions getIGUDimensions() override;

    protected:
        [[nodiscard]] double Ub(double spacerKeff);
        [[nodiscard]] double Uw(double spacerKeff);
        [[nodiscard]] double SHGCb(double spacerKeff, const double tSol);
        [[nodiscard]] double SHGCw(double spacerKeff, const double tSol);

        [[nodiscard]] virtual Tarcog::IWindow & windowAt(Option spacer, Option glazing) = 0;

        std::map<Option, CMABestWorstUFactors> m_BestWorstIGUUvalues;

        BestWorst<double> m_Spacer;
    };

    using SingleVisionFrameMap = std::map<Tarcog::ISO15099::SingleVisionFramePosition, CMAFrame>;

    //////////////////////////////////////////
    //  CMAWindowSingleVision
    //////////////////////////////////////////
    class CMAWindowSingleVision : public CMAWindow
    {
    public:
        CMAWindowSingleVision(
          double width,
          double height,
          double spacerBestKeff = 0.01,
          double spacerWorstKeff = 10.0,
          CMABestWorstUFactors bestUFactor = CreateBestWorstUFactorOption(Option::Best),
          CMABestWorstUFactors worstUFactor = CreateBestWorstUFactorOption(Option::Worst));

        void setFrameData(Tarcog::ISO15099::SingleVisionFramePosition position,
                          CMAFrame & cmaFrameData);
        void setFrameData(const SingleVisionFrameMap & frames);

        void setDividers(CMAFrame frameData, size_t nHorizontal, size_t nVertical);

    private:
        //! Single vision windows needs to create this structure, otherwise it will not work
        [[nodiscard]] static std::map<Option,
                                      std::map<Option, Tarcog::ISO15099::WindowSingleVision>>
          createBestWorstWindows(double width,
                                 double height,
                                 double tvis,
                                 double tsol,
                                 CMABestWorstUFactors bestUFactor,
                                 CMABestWorstUFactors worstUFactor);

        Tarcog::IWindow & windowAt(Option spacer, Option glazing) override;

        //! First option is spacer and second option is glass (best, worst)
        std::map<Option, std::map<Option, Tarcog::ISO15099::WindowSingleVision>> m_Window;
    };

    using DualHorizontalFrameMap =
      std::map<Tarcog::ISO15099::DualHorizontalFramePosition, CMAFrame>;

    //////////////////////////////////////////
    //  CMAWindowDualVisionHorizontal
    //////////////////////////////////////////

    class CMAWindowDualVisionHorizontal : public CMAWindow
    {
    public:
        CMAWindowDualVisionHorizontal(
          double width,
          double height,
          double spacerBestKeff = 0.01,
          double spacerWorstKeff = 10.0,
          CMABestWorstUFactors bestUFactor = CreateBestWorstUFactorOption(Option::Best),
          CMABestWorstUFactors worstUFactor = CreateBestWorstUFactorOption(Option::Worst));

        void setFrameData(Tarcog::ISO15099::DualHorizontalFramePosition position,
                          CMAFrame & cmaFrameData);
        void setFrameData(const DualHorizontalFrameMap & frames);

        void setDividers(CMAFrame frameData, size_t nHorizontal, size_t nVertical);
        void setDividersAuto(CMAFrame divider);

    private:
        //! Single vision windows needs to create this structure, otherwise it will not work
        [[nodiscard]] std::map<Option, std::map<Option, Tarcog::ISO15099::DualVisionHorizontal>>
          createBestWorstWindows(double width,
                                 double height,
                                 double tvis,
                                 double tsol,
                                 CMABestWorstUFactors bestUFactor,
                                 CMABestWorstUFactors worstUFactor) const;

        Tarcog::IWindow & windowAt(Option spacer, Option glazing) override;

        //! First option is spacer and second option is glass (best, worst)
        std::map<Option, std::map<Option, Tarcog::ISO15099::DualVisionHorizontal>> m_Window;
    };

    using DualVerticalFrameMap = std::map<Tarcog::ISO15099::DualVerticalFramePosition, CMAFrame>;

    //////////////////////////////////////////
    //  CMAWindowDualVisionVertical
    //////////////////////////////////////////

    class CMAWindowDualVisionVertical : public CMAWindow
    {
    public:
        CMAWindowDualVisionVertical(
          double width,
          double height,
          double spacerBestKeff = 0.01,
          double spacerWorstKeff = 10.0,
          CMABestWorstUFactors bestUFactor = CreateBestWorstUFactorOption(Option::Best),
          CMABestWorstUFactors worstUFactor = CreateBestWorstUFactorOption(Option::Worst));

        void setFrameData(Tarcog::ISO15099::DualVerticalFramePosition position,
                          CMAFrame & cmaFrameData);
        void setFrameData(const DualVerticalFrameMap & frames);

        void setDividers(CMAFrame cmaFrameData, size_t nHorizontal, size_t nVertical);

    private:
        //! Single vision windows needs to create this structure, otherwise it will not work
        [[nodiscard]] std::map<Option, std::map<Option, Tarcog::ISO15099::DualVisionVertical>>
          createBestWorstWindows(double width,
                                 double height,
                                 double tvis,
                                 double tsol,
                                 CMABestWorstUFactors bestUFactor,
                                 CMABestWorstUFactors worstUFactor) const;

        Tarcog::IWindow & windowAt(Option spacer, Option glazing) override;

        //! First option is spacer and second option is glass (best, worst)
        std::map<Option, std::map<Option, Tarcog::ISO15099::DualVisionVertical>> m_Window;
    };

}   // namespace CMA
