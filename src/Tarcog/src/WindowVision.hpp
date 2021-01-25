#pragma once

#include <map>
#include <memory>
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
            WindowVision(double width, double height, double tvis, double tsol, const IIGUSystem & iguSystem);
            WindowVision(double width, double height, double uvalue, double shgc, double vt, double hcExterior);
            [[nodiscard]] double area() const override;
            [[nodiscard]] double uValue() const override;
            [[nodiscard]] double shgc() const override;
            [[nodiscard]] double vt() const override;
            [[nodiscard]] double hc() const override;

            void setFrameData(FramePosition position, FrameData frameData);

        private:
            double m_Width{0};
            double m_Height{0};
            double m_Uvalue{0};
            double m_SHGC{0};
            double m_VT{1};
            double m_HcExterior{0};

            static const double m_EOGHeight;

            size_t m_NumOfVerticalDividers{0u};
            size_t m_NumOfHorizontalDividers{0u};

            std::map<FramePosition, Frame> m_Frame;

        };
    }   // namespace ISO15099
}   // namespace Tarcog
