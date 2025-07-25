#include "CMAWindow.hpp"

#include "SimpleIGU.hpp"

namespace CMA
{
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

    double CMAWindow::vt(const double tVis)
    {
        // All options have same visible transmittance. Simply pick one of them.
        return windowAt(Option::Best, Option::Best).vt(tVis);
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
        // For CMA it does not matter what the value of tsol is
        const auto tSol{1.0};
        return SHGCb(keffSpacer, tSol)
               + (SHGCw(keffSpacer, tSol) - SHGCb(keffSpacer, tSol)) * SHGCcog;
    }

    Tarcog::IGUDimensions CMAWindow::getIGUDimensions()
    {
        return windowAt(Option::Best, Option::Best).getIGUDimensions();
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

    double CMAWindow::SHGCb(const double spacerKeff, const double tSol)
    {
        const auto lnTop{std::log(spacerKeff) - std::log(m_Spacer.value(Option::Best))};
        const auto lnBot{std::log(m_Spacer.value(Option::Worst))
                         - std::log(m_Spacer.value(Option::Best))};
        const auto dU{windowAt(Option::Worst, Option::Best).shgc(tSol)
                      - windowAt(Option::Best, Option::Best).shgc(tSol)};

        return windowAt(Option::Best, Option::Best).shgc() + dU * lnTop / lnBot;
    }

    double CMAWindow::SHGCw(const double spacerKeff, const double tSol)
    {
        const auto lnTop{std::log(spacerKeff) - std::log(m_Spacer.value(Option::Best))};
        const auto lnBot{std::log(m_Spacer.value(Option::Worst))
                         - std::log(m_Spacer.value(Option::Best))};
        const auto dU{windowAt(Option::Worst, Option::Worst).shgc(tSol)
                      - windowAt(Option::Best, Option::Worst).shgc(tSol)};

        return windowAt(Option::Best, Option::Worst).shgc() + dU * lnTop / lnBot;
    }

    std::map<Option, std::map<Option, Tarcog::ISO15099::WindowSingleVision>>
      CMAWindowSingleVision::createBestWorstWindows(double width,
                                                    double height,
                                                    double tvis,
                                                    double tsol,
                                                    CMABestWorstUFactors bestUFactor,
                                                    CMABestWorstUFactors worstUFactor)
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
                                                 double spacerBestKeff,
                                                 double spacerWorstKeff,
                                                 CMABestWorstUFactors bestUFactor,
                                                 CMABestWorstUFactors worstUFactor) :
        CMAWindow(spacerBestKeff, spacerWorstKeff, bestUFactor, worstUFactor),
        m_Window(createBestWorstWindows(width, height, 0.0, 0.0, bestUFactor, worstUFactor))
    {}

    void CMAWindowSingleVision::setFrameData(Tarcog::ISO15099::SingleVisionFramePosition position,
                                             CMAFrame & cmaFrameData)
    {
        EnumOption options;
        for(auto spacerOption : options)
        {
            for(auto glazingOption : options)
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameData(position, frameData);
            }
        }
    }

    void CMAWindowSingleVision::setFrameData(const SingleVisionFrameMap & frames)
    {
        std::ranges::for_each(frames, [this](const auto & pair) {
            auto [position, frameData] = pair;
            setFrameData(position, frameData);
        });
    }

    void CMAWindowSingleVision::setDividers(CMAFrame cmaFrameData,
                                            size_t nHorizontal,
                                            size_t nVertical)
    {
        EnumOption options;
        for(auto spacerOption : options)
        {
            for(auto glazingOption : options)
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
      double spacerBestKeff,
      double spacerWorstKeff,
      CMABestWorstUFactors bestUFactor,
      CMABestWorstUFactors worstUFactor) :
        CMAWindow(spacerBestKeff, spacerWorstKeff, bestUFactor, worstUFactor),
        m_Window(createBestWorstWindows(width, height, 0.0, 0.0, bestUFactor, worstUFactor))
    {}

    void CMAWindowDualVisionHorizontal::setFrameData(
      Tarcog::ISO15099::DualHorizontalFramePosition position, CMAFrame & cmaFrameData)
    {
        EnumOption options;
        for(auto spacerOption : options)
        {
            for(auto glazingOption : options)
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameData(position, frameData);
            }
        }
    }

    void CMAWindowDualVisionHorizontal::setFrameData(const DualHorizontalFrameMap & frames)
    {
        std::ranges::for_each(frames, [this](const auto & pair) {
            auto [position, frameData] = pair;
            setFrameData(position, frameData);
        });
    }

    void CMAWindowDualVisionHorizontal::setDividers(CMAFrame cmaFrameData,
                                                    size_t nHorizontal,
                                                    size_t nVertical)
    {
        EnumOption options;
        for(auto spacerOption : options)
        {
            for(auto glazingOption : options)
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption)
                  .at(glazingOption)
                  .setDividers(frameData, nHorizontal, nVertical);
            }
        }
    }

    void CMAWindowDualVisionHorizontal::setDividersAuto(CMAFrame divider)
    {
        EnumOption options;
        for(auto spacerOption : options)
        {
            for(auto glazingOption : options)
            {
                const auto frameData = divider.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption)
                  .at(glazingOption)
                  .setDividersAuto(frameData);
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
                                                             double spacerBestKeff,
                                                             double spacerWorstKeff,
                                                             CMABestWorstUFactors bestUFactor,
                                                             CMABestWorstUFactors worstUFactor) :
        CMAWindow(spacerBestKeff, spacerWorstKeff, bestUFactor, worstUFactor),
        m_Window(createBestWorstWindows(width, height, 0.0, 0.0, bestUFactor, worstUFactor))
    {}

    void CMAWindowDualVisionVertical::setFrameData(
      Tarcog::ISO15099::DualVerticalFramePosition position, CMAFrame & cmaFrameData)
    {
        EnumOption options;
        for(auto spacerOption : options)
        {
            for(auto glazingOption : options)
            {
                const auto frameData = cmaFrameData.getFrame(spacerOption, glazingOption);
                m_Window.at(spacerOption).at(glazingOption).setFrameData(position, frameData);
            }
        }
    }

    void CMAWindowDualVisionVertical::setFrameData(const DualVerticalFrameMap & frames)
    {
        std::ranges::for_each(frames, [this](const auto & pair) {
            auto [position, frameData] = pair;
            setFrameData(position, frameData);
        });
    }

    void CMAWindowDualVisionVertical::setDividers(CMAFrame cmaFrameData,
                                                  size_t nHorizontal,
                                                  size_t nVertical)
    {
        EnumOption options;
        for(auto spacerOption : options)
        {
            for(auto glazingOption : options)
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
