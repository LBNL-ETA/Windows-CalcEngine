#ifndef NUSSELTNUMBER_H
#define NUSSELTNUMBER_H

namespace Tarcog
{
    namespace ISO15099
    {
        class CNusseltNumberStrategy
        {
        public:
            virtual ~CNusseltNumberStrategy() = default;
            virtual double calculate(double t_Tilt, double t_Ra, double t_Asp);

        protected:
            double pos(double t_Value) const;
        };

        class CNusseltNumber0To60 : public CNusseltNumberStrategy
        {
        public:
            ~CNusseltNumber0To60() override = default;
            double calculate(double t_Tilt, double t_Ra, double t_Asp) override;
        };

        class CNusseltNumber60 : public CNusseltNumberStrategy
        {
        public:
            ~CNusseltNumber60() override = default;
            double calculate(double t_Tilt, double t_Ra, double t_Asp) override;
        };

        class CNusseltNumber60To90 : public CNusseltNumberStrategy
        {
        public:
            ~CNusseltNumber60To90() override = default;
            double calculate(double t_Tilt, double t_Ra, double t_Asp) override;
        };

        class CNusseltNumber90to180 : public CNusseltNumberStrategy
        {
        public:
            ~CNusseltNumber90to180() override = default;
            double calculate(double t_Tilt, double t_Ra, double t_Asp) override;
        };

        class CNusseltNumber90 : public CNusseltNumberStrategy
        {
        public:
            ~CNusseltNumber90() override = default;
            double calculate(double t_Tilt, double t_Ra, double t_Asp) override;
        };

        class CNusseltNumber
        {
        public:
            [[nodiscard]] double calculate(double t_Tilt, double t_Ra, double t_Asp) const;
        };
    }   // namespace ISO15099
}   // namespace Tarcog


#endif
