#include "CMAWindow.hpp"

#include <utility>

#include "SimpleIGU.hpp"

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
    CMAWindow::CMAWindow(double spacerBestKeff,
                         double spacerWorstKeff,
                         CMABestWorstUFactors bestUFactor,
                         CMABestWorstUFactors worstUFactor) :
        m_BestWorstIGUUvalues({{Option::Best, bestUFactor}, {Option::Worst, worstUFactor}}),
        m_Spacer(spacerBestKeff, spacerWorstKeff)
    {}

    double CMAWindow::vt()
    {
        // All options have same visible transmittance. Simply pick one of them.
        return windowAt(Option::Best, Option::Best).vt();
    }

    double CMAWindow::uValue(const double Ucog, const double keffSpacer)
    {
        return Ub(keffSpacer)
               + ((Uw(keffSpacer) - Ub(keffSpacer))
                  * (Ucog - m_BestWorstIGUUvalues.at(Option::Worst).uValue()))
                   / (m_BestWorstIGUUvalues.at(Option::Worst).uValue()
                      - m_BestWorstIGUUvalues.at(Option::Best).uValue());
    }

    double CMAWindow::shgc(const double SHGCcog, const double keffSpacer)
    {
        return SHGCb(keffSpacer) + (SHGCw(keffSpacer) - SHGCb(keffSpacer)) * (SHGCcog - 1);
    }

    double CMAWindow::Ub(const double spacerKeff)
    {
        const auto lnTop{std::log(spacerKeff) - std::log(m_Spacer.value(Option::Best))};
        const auto lnBot{std::log(m_Spacer.value(Option::Worst))
                         - std::log(m_Spacer.value(Option::Best))};
        const auto dU{windowAt(Option::Worst, Option::Best).uValue()
                      - windowAt(Option::Best, Option::Best).uValue()};

        return windowAt(Option::Best, Option::Best).uValue() + dU * lnTop / lnBot;
    }

    double CMAWindow::Uw(const double spacerKeff)
    {
        const auto lnTop{std::log(spacerKeff) - std::log(m_Spacer.value(Option::Best))};
        const auto lnBot{std::log(m_Spacer.value(Option::Worst))
                         - std::log(m_Spacer.value(Option::Best))};
        const auto dU{windowAt(Option::Worst, Option::Worst).uValue()
                      - windowAt(Option::Best, Option::Worst).uValue()};

        return windowAt(Option::Best, Option::Worst).uValue() + dU * lnTop / lnBot;
    }

    double CMAWindow::SHGCb(const double spacerKeff)
    {
        const auto lnTop{std::log(spacerKeff) - std::log(m_Spacer.value(Option::Best))};
        const auto lnBot{std::log(m_Spacer.value(Option::Worst))
                         - std::log(m_Spacer.value(Option::Best))};
        const auto dU{windowAt(Option::Worst, Option::Best).shgc()
                      - windowAt(Option::Best, Option::Best).shgc()};

        return windowAt(Option::Best, Option::Best).shgc() + dU * lnTop / lnBot;
    }

    double CMAWindow::SHGCw(const double spacerKeff)
    {
        const auto lnTop{std::log(spacerKeff) - std::log(m_Spacer.value(Option::Best))};
        const auto lnBot{std::log(m_Spacer.value(Option::Worst))
                         - std::log(m_Spacer.value(Option::Best))};
        const auto dU{windowAt(Option::Worst, Option::Worst).shgc()
                      - windowAt(Option::Best, Option::Worst).shgc()};

        return windowAt(Option::Best, Option::Worst).shgc() + dU * lnTop / lnBot;
    }

    std::map<Option, std::map<Option, Tarcog::ISO15099::WindowSingleVision>>
      CMAWindowSingleVision::createBestWorstWindows(double width,
                                                    double height,
                                                    double tvis,
                                                    double tsol,
                                                    CMABestWorstUFactors bestUFactor,
                                                    CMABestWorstUFactors worstUFactor) const
    {
        const auto bestSHGC{0.0};
        const auto worstSHGC{0.0};
        const auto hc{0.0};
        std::map<Option, std::map<Option, Tarcog::ISO15099::WindowSingleVision>> winMap(
          {{Option::Best,
            {{Option::Best,
              Tarcog::ISO15099::WindowSingleVision(
                width,
                height,
                tvis,
                tsol,
                std::make_shared<Tarcog::ISO15099::SimpleIGU>(bestUFactor.uValue(), bestSHGC, hc))},
             {Option::Worst,
              Tarcog::ISO15099::WindowSingleVision(width,
                                                   height,
                                                   tvis,
                                                   tsol,
                                                   std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                                                     worstUFactor.uValue(), worstSHGC, hc))}}},
           {Option::Worst,
            {{Option::Best,
              Tarcog::ISO15099::WindowSingleVision(
                width,
                height,
                tvis,
                tsol,
                std::make_shared<Tarcog::ISO15099::SimpleIGU>(bestUFactor.uValue(), bestSHGC, hc))},
             {Option::Worst,
              Tarcog::ISO15099::WindowSingleVision(width,
                                                   height,
                                                   tvis,
                                                   tsol,
                                                   std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                                                     worstUFactor.uValue(), worstSHGC, hc))}}}});

        return winMap;
    }

    Tarcog::IWindow & CMAWindowSingleVision::windowAt(Option spacer, Option glazing)
    {
        return m_Window.at(spacer).at(glazing);
    }

    //////////////////////////////////////////
    //  CMAWindowSingleVision
    //////////////////////////////////////////
    CMAWindowSingleVision::CMAWindowSingleVision(double width,
                                                 double height,
                                                 double tvis,
                                                 double tsol,
                                                 double spacerBestKeff,
                                                 double spacerWorstKeff,
                                                 CMABestWorstUFactors bestUFactor,
                                                 CMABestWorstUFactors worstUFactor) :
        CMAWindow(spacerBestKeff, spacerWorstKeff, bestUFactor, worstUFactor),
        m_Window(createBestWorstWindows(width, height, tvis, tsol, bestUFactor, worstUFactor))
    {}
}   // namespace CMA
