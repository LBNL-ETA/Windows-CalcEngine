#pragma once

#include "WholeWindowConfigurations.hpp"
#include "WindowVision.hpp"

namespace Tarcog
{
    namespace ISO15099
    {
        ////////////////////////////////////////////////
        /// WindowSingleVision
        ////////////////////////////////////////////////

        class WindowSingleVision : public IWindow
        {
        public:
            WindowSingleVision() = default;

            static WindowSingleVision Create(double width,
                                             double height,
                                             double tvis,
                                             double tsol,
                                             std::shared_ptr<IIGUSystem> iguSystem);

            [[nodiscard]] double area() const override;
            [[nodiscard]] double uValue() const override;
            [[nodiscard]] double shgc() const override;
            [[nodiscard]] double vt() const override;

            void setFrameTop(FrameData frameData);
            void setFrameBottom(FrameData frameData);
            void setFrameLeft(FrameData frameData);
            void setFrameRight(FrameData frameData);
            void setDividers(FrameData frameData, size_t nHorizontal, size_t nVertical);

        private:
            WindowSingleVision(double width,
                               double height,
                               double tvis,
                               double tsol,
                               std::shared_ptr<IIGUSystem> iguSystem);

            WindowVision vision;
        };

        ////////////////////////////////////////////////
        /// WindowDualVision
        ////////////////////////////////////////////////

        class WindowDualVision : public IWindow
        {
        public:
            static WindowDualVision CreateHorizontal(double width,
                                                     double height,
                                                     double tvis_left,
                                                     double tsol_left,
                                                     std::shared_ptr<IIGUSystem> iguSystem_left,
                                                     double tvis_right,
                                                     double tsol_right,
                                                     std::shared_ptr<IIGUSystem> iguSystem_right);

            [[nodiscard]] double area() const override;
            [[nodiscard]] double uValue() const override;
            [[nodiscard]] double shgc() const override;
            [[nodiscard]] double vt() const override;

        private:
            WindowDualVision(double width,
                             double height,
                             double tvis1,
                             double tsol1,
                             std::shared_ptr<IIGUSystem> iguSystem1,
                             double tvis2,
                             double tsol2,
                             std::shared_ptr<IIGUSystem> iguSystem2);

            //! Makes single convective film coefficient averaged over the both visions
            void averageHc();

            WindowVision m_Vision1;
            WindowVision m_Vision2;
        };
    }   // namespace ISO15099
}   // namespace Tarcog
