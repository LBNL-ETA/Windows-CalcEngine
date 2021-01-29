#pragma once

#include <map>
#include "IGUConfigurations.hpp"
#include "WholeWindowConfigurations.hpp"
#include "Frame.hpp"

namespace Tarcog
{
    namespace ISO15099
    {
        class WindowVision : public IVision
        {
        public:
            WindowVision() = delete;
            WindowVision(double width, double height, double tvis, double tsol, std::shared_ptr<IIGUSystem> iguSystem);
            [[nodiscard]] double area() const override;
            [[nodiscard]] double uValue() const override;
            [[nodiscard]] double shgc() const override;
            [[nodiscard]] double vt() const override;
            [[nodiscard]] double hc() const override;
            void setHc(double hc) override;
            
            void setFrameData(FramePosition position, FrameData frameData);

        private:
            //! Makes connection between frames for correct area calculations.
            void connectFrames();

            //! Resizes IGU according to frames that are currently set in the vision
            void resizeIGU();

            [[nodiscard]] double frameProjectedArea() const;
            [[nodiscard]] double edgeOfGlassArea() const;

            std::shared_ptr<IIGUSystem> m_IGUSystem;

            double m_Width{0};
            double m_Height{0};
            double m_IGUUvalue{0};
            double m_IGUSHGC{0};
            double m_VT{1};
            double m_HcExterior{0};

            size_t m_NumOfVerticalDividers{0u};
            size_t m_NumOfHorizontalDividers{0u};            

            std::map<FramePosition, Frame> m_Frame;

        };
    }   // namespace ISO15099
}   // namespace Tarcog
