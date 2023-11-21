#pragma once

#include <memory>

#include "LayerInterfaces.hpp"
#include "TarcogConstants.hpp"

namespace FenestrationCommon
{
    enum class Side;
}


namespace Tarcog::ISO15099
{
    // Base description for any tarcog layer. This includes interior and exterior environments
    // as well. It must contain base definition of 2D geometry (Width and Height) and definition
    // of heat flow that is divided in three categories (convection, conduction and radiation).
    // Every layer can contain only Conduction + Radiation or Convection + Radiation.
    class CBaseLayer : public CLayerHeatFlow, public std::enable_shared_from_this<CBaseLayer>
    {
    public:
        CBaseLayer();
        explicit CBaseLayer(double thickness);
        virtual ~CBaseLayer() = default;

        std::shared_ptr<CBaseLayer> getPreviousLayer() const;
        std::shared_ptr<CBaseLayer> getNextLayer() const;

        virtual void connectToBackSide(std::shared_ptr<CBaseLayer> const & t_Layer);

        void tearDownConnections();

        double getThickness() const;
        virtual double getBaseThickness() const final;

        // This is to determine if layer is porous and leaking air from gap to the surrounding
        // environment. Layer are non-porous by default.
        virtual bool isPermeable() const;

        virtual std::shared_ptr<CBaseLayer> clone() const = 0;

        //! Some of the layers will require pre-calculation to be done in order to perform
        //! main loop calculation.
        virtual void precalculateState(){};

        double getConductivity();
        double getEffectiveThermalConductivity();

        void setWidth(double width);
        void setHeight(double height);
        void setTilt(double tilt);
        void setBaseThickness(double thickness);

    protected:
        void calculateRadiationFlow() override;
        void calculateConvectionOrConductionFlow() override = 0;

        std::shared_ptr<CBaseLayer> m_PreviousLayer;
        std::shared_ptr<CBaseLayer> m_NextLayer;

        [[nodiscard]] double getSurfaceArea() const;

        double m_Width{TarcogConstants::DEFAULT_WINDOW_WIDTH};
        double m_Height{TarcogConstants::DEFAULT_WINDOW_HEIGHT};
        double m_Tilt{TarcogConstants::DEFAULT_TILT};
        double m_Thickness{TarcogConstants::DEFAULT_LAYER_THICKNESS};
    };
}   // namespace Tarcog::ISO15099
