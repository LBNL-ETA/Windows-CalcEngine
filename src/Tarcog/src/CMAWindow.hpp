#pragma once

#include "CMAInterface.hpp"
#include "Frame.hpp"
#include "WholeWindow.hpp"

namespace CMA
{
    //////////////////////////////////////////
    //  SpacerGlass
    //////////////////////////////////////////
    class SpacerGlass
    {
    public:
        SpacerGlass(double bestUValue, double worstUValue, double bestSpacerKeff = 0.01, double worstSpacerKeff = 10);

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
        CMAWindow(std::shared_ptr<Tarcog::IWindow> window, double spacerBestKeff, double spacerWorstKeff);
        [[nodiscard]] double vt() const override;

    protected:
        //! First option is spacer and second option is glass
        std::map<Option, std::map<Option, std::shared_ptr<Tarcog::IWindow>>> m_Window;
        BestWorst<double> m_Spacer;
    };

    //////////////////////////////////////////
    //  CMAWindowSingleVision
    //////////////////////////////////////////
    class CMAWindowSingleVision : public CMAWindow
    {
    public:
        CMAWindowSingleVision(double width,
            double height,
            double tvis,
            double tsol,
            std::shared_ptr<Tarcog::ISO15099::IIGUSystem> iguSystem,
            double spacerBestKeff,
            double spacerWorstKeff);

        [[nodiscard]] double uValue() const override;
        [[nodiscard]] double shgc() const override;
    };

}