#pragma once

#include <map>

namespace Tarcog
{
    class Humidity
    {
    public:
        static Humidity fromFixed(double humidity_value);

        // Named constructors for know dew points
        static Humidity H30() {return Humidity{30.0};}
        static Humidity H50() {return Humidity{50.0};}
        static Humidity H70() {return Humidity{70.0};}

        [[nodiscard]] double asDouble() const;

        auto operator<=>(const Humidity&) const = default;

    private:
        double m_value;

        explicit Humidity(double humidity_value);

        static bool isAllowed(double AllowedHumidities);
    };

    struct CRResult
    {
        std::map<Humidity, double /*CR*/> values;
        double average{0.0};
    };

    struct CRResults
    {
        CRResult CRf;
        CRResult CRe;
        CRResult CRg;
        CRResult CR;
        CRResult CRb;
    };
}   // namespace Tarcog
