#ifndef TARIGUGAPLAYERDEFLECTION_H
#define TARIGUGAPLAYERDEFLECTION_H

#include <memory>

#include "IGUGapLayer.hpp"

namespace Tarcog
{
    class CIGUGapLayerDeflection : public CIGUGapLayer
    {
    public:
        CIGUGapLayerDeflection(CIGUGapLayer const & t_GapLayer,
                               double const t_Tini,
                               double const t_Pini);

        double getPressure() override;

		std::shared_ptr<CBaseLayer> clone() const override;

    private:
        // Windows is produced under given temperature and pressure.
        // That is used for deflection calculations.
        double m_Tini;
        double m_Pini;
    };

}   // namespace Tarcog

#endif
