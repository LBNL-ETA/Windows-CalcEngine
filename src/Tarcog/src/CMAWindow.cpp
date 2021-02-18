#include "CMAWindow.hpp"

namespace CMA
{
    //////////////////////////////////////////
    //  SpacerGlass
    //////////////////////////////////////////
    SpacerGlass::SpacerGlass(double bestUValue,
                             double worstUValue,
                             double bestSpacerKeff,
                             double worstSpacerKeff) :
        m_Options({{CMAProperty::Glass, {bestUValue, worstUValue}},
                   {CMAProperty::Spacer, {bestSpacerKeff, worstSpacerKeff}}})
    {}

    double SpacerGlass::uValue(Option option) const
    {
        return m_Options.at(CMAProperty::Glass).value(option);
    }

    double SpacerGlass::spacerKeff(Option option) const
    {
        return m_Options.at(CMAProperty::Spacer).value(option);
    }

    //////////////////////////////////////////
    //  CMAFrame
    //////////////////////////////////////////
    CMAFrame::CMAFrame(Tarcog::ISO15099::FrameData bestSpacerBestIGU,
                       Tarcog::ISO15099::FrameData bestSpacerWorstIGU,
                       Tarcog::ISO15099::FrameData worstSpacerBestIGU,
                       Tarcog::ISO15099::FrameData worstSpacerWorstIGU) :
        m_Frame({{Option::Best, {bestSpacerBestIGU, bestSpacerWorstIGU}},
                 {Option::Worst, {worstSpacerBestIGU, worstSpacerWorstIGU}}})
    {}

    Tarcog::ISO15099::FrameData CMAFrame::getFrame(Option spacerOption, Option iguOption)
    {
        return m_Frame.at(spacerOption).value(iguOption);
    }

    //////////////////////////////////////////
    //  CMAWindow
    //////////////////////////////////////////
    CMAWindow::CMAWindow(std::shared_ptr<Tarcog::IWindow> window,
                         double spacerBestKeff,
                         double spacerWorstKeff) :
        // clang-format off
        m_Window(
        {
            {
                Option::Best,
                {
                    {Option::Best, window},
                    {Option::Worst, window}
                }
            },
            {
                Option::Worst,
                {
                    {Option::Best, window},
                    {Option::Worst, window}
                }
            }
        }
       ),
        // clang-format on

        m_Spacer(spacerBestKeff, spacerWorstKeff)
    {}

    double CMAWindow::vt() const
    {
        // All options have same visible transmittance. Simply pick one of them.
        return m_Window.at(Option::Best).at(Option::Best)->vt();
    }

    //////////////////////////////////////////
    //  CMAWindowSingleVision
    //////////////////////////////////////////
    CMAWindowSingleVision::CMAWindowSingleVision(
      double width,
      double height,
      double tvis,
      double tsol,
      std::shared_ptr<Tarcog::ISO15099::IIGUSystem> iguSystem,
      double spacerBestKeff,
      double spacerWorstKeff) :
        CMAWindow(std::make_shared<Tarcog::ISO15099::WindowSingleVision>(
                    width, height, tvis, tsol, iguSystem),
                  spacerBestKeff,
                  spacerWorstKeff)
    {}    
}   // namespace CMA
