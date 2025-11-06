#pragma once

#include <memory>
#include "LayerInterfaces.hpp"
#include "TarcogConstants.hpp"
#include "SurfacesManager.hpp"

namespace FenestrationCommon
{
    enum class Side;
}

namespace Tarcog::ISO15099
{
    class CBaseLayer : public HeatFlowLayer,
                       public SurfacesManager,
                       public std::enable_shared_from_this<CBaseLayer>
    {
    public:
        CBaseLayer();
        explicit CBaseLayer(double thickness);
        virtual ~CBaseLayer();

        std::shared_ptr<CBaseLayer> getPreviousLayer() const;
        std::shared_ptr<CBaseLayer> getNextLayer() const;

        virtual void connectToBackSide(const std::shared_ptr<CBaseLayer> & t_Layer);
        void tearDownConnections();

        virtual double getThickness() const;
        virtual double getHeatFlow() final;
        virtual double getRadiationFlow();
        virtual double getConvectionConductionFlow() final;

        virtual bool isPermeable() const;
        virtual std::shared_ptr<CBaseLayer> clone() const = 0;
        virtual void precalculateState() {};

        double getConductivity();
        double getEffectiveThermalConductivity();

        void setWidth(double width);
        void setHeight(double height);
        void setTilt(double tilt);

    protected:
        void calculateRadiationFlow() override;
        void calculateConvectionOrConductionFlow() override = 0;
        [[nodiscard]] double getSurfaceArea() const;

        double m_Width{TarcogConstants::DEFAULT_WINDOW_WIDTH};
        double m_Height{TarcogConstants::DEFAULT_WINDOW_HEIGHT};
        double m_Tilt{TarcogConstants::DEFAULT_TILT};
        double m_Thickness{TarcogConstants::DEFAULT_LAYER_THICKNESS};

    private:
        std::weak_ptr<CBaseLayer> m_PreviousLayer;
        std::weak_ptr<CBaseLayer> m_NextLayer;
    };
}
