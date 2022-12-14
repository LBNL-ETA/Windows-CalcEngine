#pragma once

namespace Gases
{
    class CGasSettings
    {
    public:
        static CGasSettings & instance();

        [[nodiscard]] double getVacuumPressure() const;
        void setVacuumPressure(double t_Value);

    private:
        CGasSettings();

        // Value that will trigger specific gas calculations. Bellow this value it will be
        // considered that gases need to apply vacuum calculations.
        double m_VacuumPressure;
    };

}   // namespace Gases
