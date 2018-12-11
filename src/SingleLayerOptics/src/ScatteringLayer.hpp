#ifndef SCATTERINGLAYER_H
#define SCATTERINGLAYER_H

#include <memory>
#include <map>

#include "BSDFLayerMaker.hpp"
#include "IScatteringLayer.hpp"
#include "OpticalSurface.hpp"
#include "WCECommon.hpp"

namespace SingleLayerOptics
{
    class CLayerSingleComponent;
    class CMaterial;
    class CBSDFIntegrator;
    class CBSDFLayer;
    class ICellDescription;

    enum class EmissivityPolynomials
    {
        NFRC_301_Coated,
        NFRC_301_Uncoated,
        EN12898
    };

    extern std::map<EmissivityPolynomials, std::vector<double>> emissPolynomial;

    // Handles general case layer when properties can be direct, diffuse or combination between
    // these two.
    class CScatteringLayer : public IScatteringLayer
    {
    public:
        CScatteringLayer() = default;
        CScatteringLayer(const CScatteringSurface & t_Front, const CScatteringSurface & t_Back);
        CScatteringLayer(const CScatteringSurface && t_Front, const CScatteringSurface && t_Back);

        CScatteringLayer(double Tf_dir_dir,
                         double Rf_dir_dir,
                         double Tb_dir_dir,
                         double Rb_dir_dir,
                         double Tf_dir_dif,
                         double Rf_dir_dif,
                         double Tb_dir_dif,
                         double Rb_dir_dif,
                         double Tf_dif_dif,
                         double Rf_dif_dif,
                         double Tb_dif_dif,
                         double Rb_dif_dif);

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

        static CScatteringLayer
          createPerforatedRectangularLayer(const std::shared_ptr<CMaterial> & t_Material,
                                           double x,
                                           double y,
                                           double thickness,
                                           double xHole,
                                           double yHole);


        void setSourceData(std::shared_ptr<FenestrationCommon::CSeries> t_SourceData) const;

        void setBlackBodySource(double temperature);

        CScatteringSurface & getSurface(FenestrationCommon::Side t_Side);

        double getPropertySimple(FenestrationCommon::PropertySimple t_Property,
                                 FenestrationCommon::Side t_Side,
                                 FenestrationCommon::Scattering t_Scattering,
                                 double t_Theta = 0,
                                 double t_Phi = 0) override;

        // void setPropertySimple( const FenestrationCommon::PropertySimple t_Property,
        //  const FenestrationCommon::Side t_Side, const FenestrationCommon::Scattering
        //  t_Scattering, const double value ) const;

        double getAbsorptance(FenestrationCommon::Side t_Side,
                              FenestrationCommon::ScatteringSimple t_Scattering,
                              double t_Theta = 0,
                              double t_Phi = 0);

        double
          getAbsorptance(FenestrationCommon::Side t_Side, double t_Theta = 0, double t_Phi = 0);

        // This function is valid only for specular layers
        double normalToHemisphericalEmissivity(FenestrationCommon::Side t_Side,
                                               EmissivityPolynomials type);
        double normalToHemisphericalEmissivity(FenestrationCommon::Side t_Side,
                                               const std::vector<double> & polynomial);

        CLayerSingleComponent getLayer(FenestrationCommon::Scattering t_Scattering,
                                       double t_Theta = 0,
                                       double t_Phi = 0);

        std::vector<double> getWavelengths() const override;
        void setWavelengths(const std::vector<double> & wavelengths);

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

        double m_Theta;
        double m_Phi;
    };

}   // namespace SingleLayerOptics

#endif
