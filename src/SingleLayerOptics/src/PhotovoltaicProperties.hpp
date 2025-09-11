#pragma once

#include <vector>

namespace SingleLayerOptics
{
    ///////////////////////////////////////////////////////////////////////////
    /// PVPowerProperties
    ///////////////////////////////////////////////////////////////////////////

    //! Simple structure to store photovoltaic power properties.
    //!
    //! Note that unit for current is not using standard SI units. The reason is that users will
    //! provide the table in this format and this will avoid any conversions.
    struct PVPowerProperties
    {
        PVPowerProperties(double jsc, double voc, double ff);

        double JSC;   // mA/cm^2
        double VOC;   // V
        double FF;    // Unitless
    };

    ///////////////////////////////////////////////////////////////////////////
    /// PhotovoltaicSampleData
    ///////////////////////////////////////////////////////////////////////////

    class PVPowerPropertiesTable
    {
    public:
        PVPowerPropertiesTable() = default;
        PVPowerPropertiesTable(std::vector<PVPowerProperties> pvPowerProperties);

        [[nodiscard]] double voc(double jsc) const;
        [[nodiscard]] double ff(double jsc) const;

    private:
        struct SearchIndexes
        {
            size_t first;
            size_t second;
        };

        [[nodiscard]] SearchIndexes findIndexes(double el) const;

        std::vector<PVPowerProperties> m_PVPowerProperties;
    };
}   // namespace SingleLayerOptics