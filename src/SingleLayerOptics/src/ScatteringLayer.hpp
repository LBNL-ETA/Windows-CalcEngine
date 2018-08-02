#ifndef SCATTERINGLAYER_H
#define SCATTERINGLAYER_H

#include <memory>
#include <map>

#include "BSDFLayerMaker.hpp"
#include "IScatteringLayer.hpp"
#include "WCECommon.hpp"

namespace SingleLayerOptics
{
    class CScatteringSurface;
    class CLayerSingleComponent;
    class CMaterial;
    class CBSDFIntegrator;
    class CBSDFLayer;
    class ICellDescription;

    // Handles general case layer when properties can be direct, diffuse or combination between
    // these two.
    class CScatteringLayer : public IScatteringLayer
    {
    public:
        CScatteringLayer(const CScatteringSurface & t_Front, const CScatteringSurface & t_Back);
        CScatteringLayer(const CScatteringSurface && t_Front, const CScatteringSurface && t_Back);

        CScatteringLayer(const double Tf_dir_dir,
                         const double Rf_dir_dir,
                         const double Tb_dir_dir,
                         const double Rb_dir_dir,
                         const double Tf_dir_dif,
                         const double Rf_dir_dif,
                         const double Tb_dir_dif,
                         const double Rb_dir_dif,
                         const double Tf_dif_dif,
                         const double Rf_dif_dif,
                         const double Tb_dif_dif,
                         const double Rb_dif_dif);

        CScatteringLayer(const std::shared_ptr<CMaterial> & t_Material,
                         std::shared_ptr<ICellDescription> t_Description = nullptr,
                         const DistributionMethod t_Method = DistributionMethod::UniformDiffuse);

        static CScatteringLayer createSpecularLayer(const std::shared_ptr<CMaterial> & t_Material);
        static CScatteringLayer
          createPerfectlyDiffusingLayer(const std::shared_ptr<CMaterial> & t_Material);
        static CScatteringLayer createWovenLayer(const std::shared_ptr<CMaterial> & t_Material,
                                                 double diameter,
                                                 double spacing);
        static CScatteringLayer
          createVenetianLayer(const std::shared_ptr<CMaterial> & t_Material,
                              double slatWidth,
                              double slatSpacing,
                              double slatTiltAngle,
                              double curvatureRadius,
                              size_t numOfSlatSegments,
                              DistributionMethod method = DistributionMethod::DirectionalDiffuse);

        static CScatteringLayer
          createPerforatedCircularLayer(const std::shared_ptr<CMaterial> & t_Material,
                                        double x,
                                        double y,
                                        double thickness,
                                        double radius);


        void setSourceData(std::shared_ptr<FenestrationCommon::CSeries> t_SourceData) const;

        CScatteringSurface & getSurface(const FenestrationCommon::Side t_Side);

        double getPropertySimple(const FenestrationCommon::PropertySimple t_Property,
                                 const FenestrationCommon::Side t_Side,
                                 const FenestrationCommon::Scattering t_Scattering,
                                 const double t_Theta = 0,
                                 const double t_Phi = 0) override;

        // void setPropertySimple( const FenestrationCommon::PropertySimple t_Property,
        //  const FenestrationCommon::Side t_Side, const FenestrationCommon::Scattering
        //  t_Scattering, const double value ) const;

        double getAbsorptance(const FenestrationCommon::Side t_Side,
                              const FenestrationCommon::ScatteringSimple t_Scattering,
                              const double t_Theta = 0,
                              const double t_Phi = 0);

        double getAbsorptance(const FenestrationCommon::Side t_Side,
                              const double t_Theta = 0,
                              const double t_Phi = 0);

        CLayerSingleComponent getLayer(const FenestrationCommon::Scattering t_Scattering,
                                       const double t_Theta = 0,
                                       const double t_Phi = 0);

        std::vector<double> getWavelengths() const override;

        double getMinLambda() const override;
        double getMaxLambda() const override;

    private:
        CScatteringLayer(const std::shared_ptr<CBSDFLayer> & aBSDF);

        void createResultsAtAngle(const double t_Theta, const double t_Phi);

        CScatteringSurface createSurface(const FenestrationCommon::Side t_Side,
                                         const double t_Theta,
                                         const double t_Phi);

        bool checkCurrentAngles(const double t_Theta, const double t_Phi);

        std::map<FenestrationCommon::Side, CScatteringSurface> m_Surface;

        std::shared_ptr<CBSDFLayer> m_BSDFLayer;
        // std::shared_ptr<CBaseCell> m_Cell;

        double m_Theta;
        double m_Phi;
    };

}   // namespace SingleLayerOptics

#endif
