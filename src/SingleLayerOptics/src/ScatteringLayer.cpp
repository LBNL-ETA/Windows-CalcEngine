#include <stdexcept>

#include "ScatteringLayer.hpp"
#include "LayerSingleComponent.hpp"
#include "OpticalSurface.hpp"
#include "BaseCell.hpp"
#include "BSDFLayer.hpp"
#include "MaterialDescription.hpp"
#include "BSDFDirections.hpp"
#include "BSDFIntegrator.hpp"
#include "BeamDirection.hpp"
#include "WCECommon.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    CScatteringLayer::CScatteringLayer(const CScatteringSurface & t_Front,
                                       const CScatteringSurface & t_Back) :
        m_Surface({{Side::Front, t_Front}, {Side::Back, t_Back}}),
        m_BSDFLayer(nullptr),
        m_Theta(0),
        m_Phi(0)
    {}

    CScatteringLayer::CScatteringLayer(const CScatteringSurface && t_Front,
                                       const CScatteringSurface && t_Back) :
        m_Surface({{Side::Front, std::move(t_Front)}, {Side::Back, std::move(t_Back)}}),
        m_BSDFLayer(nullptr),
        m_Theta(0),
        m_Phi(0)
    {}

    CScatteringLayer::CScatteringLayer(const double Tf_dir_dir,
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
                                       const double Rb_dif_dif) :
        m_Surface({{Side::Front,
                    CScatteringSurface(
                      Tf_dir_dir, Rf_dir_dir, Tf_dir_dif, Rf_dir_dif, Tf_dif_dif, Rf_dif_dif)},
                   {Side::Back,
                    CScatteringSurface(
                      Tb_dir_dir, Rb_dir_dir, Tb_dir_dif, Rb_dir_dif, Tb_dif_dif, Rb_dif_dif)}}),
        m_BSDFLayer(nullptr),
        m_Theta(0),
        m_Phi(0)
    {}

    CScatteringLayer::CScatteringLayer(const std::shared_ptr<CMaterial> & t_Material,
                                       std::shared_ptr<ICellDescription> t_Description,
                                       const DistributionMethod t_Method) :
        m_BSDFLayer(nullptr),
        m_Theta(0),
        m_Phi(0)
    {
        // Scattering layer can also be created from material and cell desctiption in which case
        // integration will be performed using BSDF distribution while direct-direct component will
        // be taken directly from cell.
        auto aBSDF = std::make_shared<CBSDFHemisphere>(BSDFBasis::Full);
        auto aMaker = CBSDFLayerMaker(t_Material, aBSDF, t_Description, t_Method);
        m_BSDFLayer = aMaker.getLayer();
    }

    void CScatteringLayer::setSourceData(std::shared_ptr<CSeries> t_SourceData) const
    {
        if(m_BSDFLayer != nullptr)
        {
            m_BSDFLayer->setSourceData(t_SourceData);
        }
    }

    CScatteringSurface & CScatteringLayer::getSurface(const Side t_Side)
    {
        if(m_Surface.size() == 0)
        {
            m_Theta = 0;
            m_Phi = 0;
            createResultsAtAngle(m_Theta, m_Phi);
        }
        return m_Surface.at(t_Side);
    }

    double CScatteringLayer::getPropertySimple(const PropertySimple t_Property,
                                               const Side t_Side,
                                               const Scattering t_Scattering,
                                               const double t_Theta,
                                               const double t_Phi)
    {
        checkCurrentAngles(t_Theta, t_Phi);
        auto aSurface = getSurface(t_Side);
        return aSurface.getPropertySimple(t_Property, t_Scattering);
    }

    double CScatteringLayer::getAbsorptance(const Side t_Side,
                                            const ScatteringSimple t_Scattering,
                                            const double t_Theta,
                                            const double t_Phi)
    {
        checkCurrentAngles(t_Theta, t_Phi);
        auto & aSurface = getSurface(t_Side);
        return aSurface.getAbsorptance(t_Scattering);
    }

    double
      CScatteringLayer::getAbsorptance(const Side t_Side, const double t_Theta, const double t_Phi)
    {
        checkCurrentAngles(t_Theta, t_Phi);
        auto aSurface = getSurface(t_Side);
        return aSurface.getAbsorptance();
    }

    CLayerSingleComponent CScatteringLayer::getLayer(const Scattering t_Scattering,
                                                     const double t_Theta,
                                                     const double t_Phi)
    {
        double Tf = getPropertySimple(PropertySimple::T, Side::Front, t_Scattering, t_Theta, t_Phi);
        double Rf = getPropertySimple(PropertySimple::R, Side::Front, t_Scattering, t_Theta, t_Phi);
        double Tb = getPropertySimple(PropertySimple::T, Side::Back, t_Scattering, t_Theta, t_Phi);
        double Rb = getPropertySimple(PropertySimple::R, Side::Back, t_Scattering, t_Theta, t_Phi);
        return CLayerSingleComponent(Tf, Rf, Tb, Rb);
    }

    std::vector<double> CScatteringLayer::getWavelengths() const
    {
        return m_BSDFLayer->getBandWavelengths();
    }

    void CScatteringLayer::createResultsAtAngle(const double t_Theta, const double t_Phi)
    {
        if(m_BSDFLayer != nullptr)
        {
            m_Surface.emplace(Side::Front, createSurface(Side::Front, t_Theta, t_Phi));
            m_Surface.emplace(Side::Back, createSurface(Side::Back, t_Theta, t_Phi));
        }
    }

    CScatteringSurface
      CScatteringLayer::createSurface(const Side t_Side, const double t_Theta, const double t_Phi)
    {
        CBeamDirection aDirection = CBeamDirection(t_Theta, t_Phi);
        double T_dir_dir = m_BSDFLayer->getCell()->T_dir_dir(t_Side, aDirection);
        double R_dir_dir = m_BSDFLayer->getCell()->R_dir_dir(t_Side, aDirection);
        double T_dir_dif =
          m_BSDFLayer->getResults()->DirHem(t_Side, PropertySimple::T, t_Theta, t_Phi) - T_dir_dir;
        if(T_dir_dif < 0)
        {
            T_dir_dif = 0;
        }
        double R_dir_dif =
          m_BSDFLayer->getResults()->DirHem(t_Side, PropertySimple::R, t_Theta, t_Phi) - R_dir_dir;
        if(R_dir_dif < 0)
        {
            R_dir_dif = 0;
        }
        double T_dif_dif = m_BSDFLayer->getResults()->DiffDiff(t_Side, PropertySimple::T);
        double R_dif_dif = m_BSDFLayer->getResults()->DiffDiff(t_Side, PropertySimple::R);
        return CScatteringSurface(T_dir_dir, R_dir_dir, T_dir_dif, R_dir_dif, T_dif_dif, R_dif_dif);
    }

    bool CScatteringLayer::checkCurrentAngles(const double t_Theta, const double t_Phi)
    {
        bool curAngles = ((t_Theta == m_Theta) && (t_Phi == m_Phi));
        if(!curAngles)
        {
            m_Theta = t_Theta;
            m_Phi = t_Phi;
            createResultsAtAngle(m_Theta, m_Phi);
        }
        return false;
    }

    double CScatteringLayer::getMinLambda() const
    {
        return m_BSDFLayer->getCell()->getMinLambda();
    }

    double CScatteringLayer::getMaxLambda() const
    {
        return m_BSDFLayer->getCell()->getMaxLambda();
    }

    CScatteringLayer
      CScatteringLayer::createSpecularLayer(const std::shared_ptr<CMaterial> & t_Material)
    {
        const auto aBSDF = std::make_shared<CBSDFHemisphere>(BSDFBasis::Full);
        return CScatteringLayer(CBSDFLayerMaker::getSpecularLayer(t_Material, aBSDF));
    }

    CScatteringLayer::CScatteringLayer(const std::shared_ptr<CBSDFLayer> & aBSDF) :
        m_BSDFLayer(aBSDF)
    {}

    CScatteringLayer CScatteringLayer::createWovenLayer(
      const std::shared_ptr<CMaterial> & t_Material, const double diameter, const double spacing)
    {
        const auto aBSDF = std::make_shared<CBSDFHemisphere>(BSDFBasis::Full);
        return CScatteringLayer(
          CBSDFLayerMaker::getWovenLayer(t_Material, aBSDF, diameter, spacing));
    }

    CScatteringLayer
      CScatteringLayer::createPerfectlyDiffusingLayer(const std::shared_ptr<CMaterial> & t_Material)
    {
        const auto aBSDF = std::make_shared<CBSDFHemisphere>(BSDFBasis::Full);
        return CScatteringLayer(CBSDFLayerMaker::getPerfectlyDiffuseLayer(t_Material, aBSDF));
    }

}   // namespace SingleLayerOptics
