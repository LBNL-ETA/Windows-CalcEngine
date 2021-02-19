#include <utility>

#include "CMAWindow.hpp"

#include "SimpleIGU.hpp"
#include "CMAInterface.hpp"

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
        const auto ub{Ub(keffSpacer)};
        const auto uw{Uw(keffSpacer)};
        const auto ucw{m_BestWorstIGUUvalues.at(Option::Worst).uValue()};
        const auto ucb{m_BestWorstIGUUvalues.at(Option::Best).uValue()};
        return ub + (uw - ub) * (Ucog - ucb) / (ucw - ucb);
    }

    double CMAWindow::shgc(const double SHGCcog, const double keffSpacer)
    {
        return SHGCb(keffSpacer) + (SHGCw(keffSpacer) - SHGCb(keffSpacer)) * SHGCcog;
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
        const auto worstSHGC{1.0};
        const auto bestHC{bestUFactor.hcout()};
        const auto worstHC{worstUFactor.hcout()};
        std::map<Option, std::map<Option, Tarcog::ISO15099::WindowSingleVision>> winMap(
          {{Option::Best,
            {{Option::Best,
              Tarcog::ISO15099::WindowSingleVision(width,
                                                   height,
                                                   tvis,
                                                   tsol,
                                                   std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                                                     bestUFactor.uValue(), bestSHGC, bestHC))},
             {Option::Worst,
              Tarcog::ISO15099::WindowSingleVision(width,
                                                   height,
                                                   tvis,
                                                   tsol,
                                                   std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                                                     worstUFactor.uValue(), worstSHGC, worstHC))}}},
           {Option::Worst,
            {{Option::Best,
              Tarcog::ISO15099::WindowSingleVision(width,
                                                   height,
                                                   tvis,
                                                   tsol,
                                                   std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                                                     bestUFactor.uValue(), bestSHGC, bestHC))},
             {Option::Worst,
              Tarcog::ISO15099::WindowSingleVision(
                width,
                height,
                tvis,
                tsol,
                std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                  worstUFactor.uValue(), worstSHGC, worstHC))}}}});

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

    void CMAWindowSingleVision::setFrameTop(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameTop(frameData);
            }
        }
    }

    void CMAWindowSingleVision::setFrameBottom(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameBottom(frameData);
            }
        }
    }

    void CMAWindowSingleVision::setFrameLeft(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameLeft(frameData);
            }
        }
    }

    void CMAWindowSingleVision::setFrameRight(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameRight(frameData);
            }
        }
    }

    void CMAWindowSingleVision::setDividers(CMAFrame cmaFrameData,
                                            size_t nHorizontal,
                                            size_t nVertical)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption)
                  .at(glazingOption)
                  .setDividers(frameData, nHorizontal, nVertical);
            }
        }
    }

    //////////////////////////////////////////
    //  CMAWindowDualVisionHorizontal
    //////////////////////////////////////////

    CMAWindowDualVisionHorizontal::CMAWindowDualVisionHorizontal(
      double width,
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

    void CMAWindowDualVisionHorizontal::setFrameTopLeft(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameTopLeft(frameData);
            }
        }
    }

    void CMAWindowDualVisionHorizontal::setFrameTopRight(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameTopRight(frameData);
            }
        }
    }

    void CMAWindowDualVisionHorizontal::setFrameBottomLeft(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameBottomLeft(frameData);
            }
        }
    }

    void CMAWindowDualVisionHorizontal::setFrameBottomRight(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameBottomRight(frameData);
            }
        }
    }

    void CMAWindowDualVisionHorizontal::setFrameLeft(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameLeft(frameData);
            }
        }
    }

    void CMAWindowDualVisionHorizontal::setFrameRight(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameRight(frameData);
            }
        }
    }

    void CMAWindowDualVisionHorizontal::setFrameMeetingRail(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameMeetingRail(frameData);
            }
        }
    }

    void CMAWindowDualVisionHorizontal::setDividers(CMAFrame cmaFrameData,
                                                    size_t nHorizontal,
                                                    size_t nVertical)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption)
                  .at(glazingOption)
                  .setDividers(frameData, nHorizontal, nVertical);
            }
        }
    }

    std::map<Option, std::map<Option, Tarcog::ISO15099::DualVisionHorizontal>>
      CMAWindowDualVisionHorizontal::createBestWorstWindows(double width,
                                                            double height,
                                                            double tvis,
                                                            double tsol,
                                                            CMABestWorstUFactors bestUFactor,
                                                            CMABestWorstUFactors worstUFactor) const
    {
        const auto bestSHGC{0.0};
        const auto worstSHGC{1.0};
        const auto bestHC{bestUFactor.hcout()};
        const auto worstHC{worstUFactor.hcout()};
        std::map<Option, std::map<Option, Tarcog::ISO15099::DualVisionHorizontal>> winMap(
          {{Option::Best,
            {{Option::Best,
              Tarcog::ISO15099::DualVisionHorizontal(width,
                                                     height,
                                                     tvis,
                                                     tsol,
                                                     std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                                                       bestUFactor.uValue(), bestSHGC, bestHC),
                                                     tvis,
                                                     tsol,
                                                     std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                                                       bestUFactor.uValue(), bestSHGC, bestHC))},
             {Option::Worst,
              Tarcog::ISO15099::DualVisionHorizontal(
                width,
                height,
                tvis,
                tsol,
                std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                  worstUFactor.uValue(), worstSHGC, worstHC),
                tvis,
                tsol,
                std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                  worstUFactor.uValue(), worstSHGC, worstHC))}}},
           {Option::Worst,
            {{Option::Best,
              Tarcog::ISO15099::DualVisionHorizontal(width,
                                                     height,
                                                     tvis,
                                                     tsol,
                                                     std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                                                       bestUFactor.uValue(), bestSHGC, bestHC),
                                                     tvis,
                                                     tsol,
                                                     std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                                                       bestUFactor.uValue(), bestSHGC, bestHC))},
             {Option::Worst,
              Tarcog::ISO15099::DualVisionHorizontal(
                width,
                height,
                tvis,
                tsol,
                std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                  worstUFactor.uValue(), worstSHGC, worstHC),
                tvis,
                tsol,
                std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                  worstUFactor.uValue(), worstSHGC, worstHC))}}}});

        return winMap;
    }

    Tarcog::IWindow & CMAWindowDualVisionHorizontal::windowAt(Option spacer, Option glazing)
    {
        return m_Window.at(spacer).at(glazing);
    }

    //////////////////////////////////////////
    //  CMAWindowDualVisionVertical
    //////////////////////////////////////////

    CMAWindowDualVisionVertical::CMAWindowDualVisionVertical(double width,
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

    void CMAWindowDualVisionVertical::setFrameTop(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameTop(frameData);
            }
        }
    }

    void CMAWindowDualVisionVertical::setFrameBottom(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameBottom(frameData);
            }
        }
    }

    void CMAWindowDualVisionVertical::setFrameTopLeft(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameTopLeft(frameData);
            }
        }
    }

    void CMAWindowDualVisionVertical::setFrameTopRight(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameTopRight(frameData);
            }
        }
    }

    void CMAWindowDualVisionVertical::setFrameBottomLeft(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameBottomLeft(frameData);
            }
        }
    }

    void CMAWindowDualVisionVertical::setFrameBottomRight(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameBottomRight(frameData);
            }
        }
    }

    void CMAWindowDualVisionVertical::setFrameMettingRail(CMAFrame cmaFrameData)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameMettingRail(frameData);
            }
        }
    }

    void CMAWindowDualVisionVertical::setDividers(CMAFrame cmaFrameData,
                                                  size_t nHorizontal,
                                                  size_t nVertical)
    {
        for(auto spacerOption : EnumOption())
        {
            for(auto glazingOption : EnumOption())
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption)
                  .at(glazingOption)
                  .setDividers(frameData, nHorizontal, nVertical);
            }
        }
    }

    std::map<Option, std::map<Option, Tarcog::ISO15099::DualVisionVertical>>
      CMAWindowDualVisionVertical::createBestWorstWindows(double width,
                                                          double height,
                                                          double tvis,
                                                          double tsol,
                                                          CMABestWorstUFactors bestUFactor,
                                                          CMABestWorstUFactors worstUFactor) const
    {
        const auto bestSHGC{0.0};
        const auto worstSHGC{1.0};
        const auto bestHC{bestUFactor.hcout()};
        const auto worstHC{worstUFactor.hcout()};
        std::map<Option, std::map<Option, Tarcog::ISO15099::DualVisionVertical>> winMap(
          {{Option::Best,
            {{Option::Best,
              Tarcog::ISO15099::DualVisionVertical(width,
                                                   height,
                                                   tvis,
                                                   tsol,
                                                   std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                                                     bestUFactor.uValue(), bestSHGC, bestHC),
                                                   tvis,
                                                   tsol,
                                                   std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                                                     bestUFactor.uValue(), bestSHGC, bestHC))},
             {Option::Worst,
              Tarcog::ISO15099::DualVisionVertical(width,
                                                   height,
                                                   tvis,
                                                   tsol,
                                                   std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                                                     worstUFactor.uValue(), worstSHGC, worstHC),
                                                   tvis,
                                                   tsol,
                                                   std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                                                     worstUFactor.uValue(), worstSHGC, worstHC))}}},
           {Option::Worst,
            {{Option::Best,
              Tarcog::ISO15099::DualVisionVertical(width,
                                                   height,
                                                   tvis,
                                                   tsol,
                                                   std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                                                     bestUFactor.uValue(), bestSHGC, bestHC),
                                                   tvis,
                                                   tsol,
                                                   std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                                                     bestUFactor.uValue(), bestSHGC, bestHC))},
             {Option::Worst,
              Tarcog::ISO15099::DualVisionVertical(
                width,
                height,
                tvis,
                tsol,
                std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                  worstUFactor.uValue(), worstSHGC, worstHC),
                tvis,
                tsol,
                std::make_shared<Tarcog::ISO15099::SimpleIGU>(
                  worstUFactor.uValue(), worstSHGC, worstHC))}}}});

        return winMap;
    }

    Tarcog::IWindow & CMAWindowDualVisionVertical::windowAt(Option spacer, Option glazing)
    {
        return m_Window.at(spacer).at(glazing);
    }
}   // namespace CMA
