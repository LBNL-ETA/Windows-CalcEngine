#pragma once

#include "WholeWindowConfigurations.hpp"
#include "WindowVision.hpp"

namespace Tarcog
{
    namespace ISO15099
    {
        class WindowSingleVision : public IWindow
        {
        public:
            explicit WindowSingleVision(WindowVision vision);

            [[nodiscard]] double area() const override;
            [[nodiscard]] double uValue() const override;
            [[nodiscard]] double shgc() const override;
            [[nodiscard]] double vt() const override;

        private:
            WindowVision vision;
        };
    }
}