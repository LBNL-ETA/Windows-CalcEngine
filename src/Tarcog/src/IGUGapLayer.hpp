#ifndef TARIGUGAPLAYER_H
#define TARIGUGAPLAYER_H

#include <memory>
#include "BaseIGULayer.hpp"
#include "WCEGases.hpp"

namespace Gases
{
    class CGas;
}

namespace Tarcog
{
    double const ReferenceTemperature = 273.15;

    class CIGUGapLayer : public CBaseIGULayer, public CGasLayer
    {
    public:
        CIGUGapLayer(double const t_Thickness, double const t_Pressure);
        CIGUGapLayer(double const t_Thickness,
                     double const t_Pressure,
                     const Gases::CGas & t_Gas);

        void connectToBackSide(std::shared_ptr<CBaseLayer> const & t_Layer) override;

        double layerTemperature() override;

        double averageTemperature() const;

        double getPressure() override;

        std::shared_ptr<CBaseLayer> clone() const override;


    protected:
        void initializeStateVariables() override;
        void calculateConvectionOrConductionFlow() override;

    private:
        double calculateRayleighNumber();
        double aspectRatio() const;
        double convectiveH();

        double getGasTemperature() override;

        void checkNextLayer() const;
    };

}   // namespace Tarcog

#endif
