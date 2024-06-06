#include <memory>
#include <mutex>
#include <cassert>

#include <WCECommon.hpp>

#include "VenetianSegments.hpp"

#include "VenetianCellDescription.hpp"
#include "BeamDirection.hpp"
#include "MaterialDescription.hpp"

namespace SingleLayerOptics
{
    std::mutex radianceMutex;
    std::mutex irradianceMutex;

    using namespace FenestrationCommon;

    ////////////////////////////////////////////////////////////////////////////////////////////
    ///  CVenetianCellEnergy
    ////////////////////////////////////////////////////////////////////////////////////////////
    CVenetianCellEnergy::CVenetianCellEnergy() : m_Cell(nullptr), m_LayerProperties({})
    {}

    CVenetianCellEnergy::CVenetianCellEnergy(
      const std::shared_ptr<CVenetianCellDescription> & t_Cell,
      const LayerProperties & properties) :
        m_Cell(t_Cell),
        m_LayerProperties(properties),
        m_SlatSegmentsMesh(*m_Cell),
        slatsDiffuseRadiancesMatrix(formIrradianceMatrix(m_Cell->viewFactors(), m_LayerProperties))
    {}

    double CVenetianCellEnergy::T_dir_dir(const CBeamDirection & t_Direction)
    {
        return m_Cell->T_dir_dir(Side::Front, t_Direction);
    }

    double CVenetianCellEnergy::T_dir_dif(const CBeamDirection & t_Direction)
    {
        if(!m_DirectToDiffuseSlatIrradiances.count(t_Direction))
        {
            const auto diffuseViewFactors{
              beamToDiffuseViewFactors(Side::Front, t_Direction, *m_Cell, m_SlatSegmentsMesh)};

            const auto irradiance = directToDiffuseSlatIrradiances(
              diffuseViewFactors, slatsDiffuseRadiancesMatrix, m_SlatSegmentsMesh, t_Direction);
            std::lock_guard<std::mutex> lock(irradianceMutex);
            m_DirectToDiffuseSlatIrradiances[t_Direction] = irradiance;
        }

        // Total energy accounts for direct to direct component. That needs to be subtracted since
        // only direct to diffuse is of interest
        return m_DirectToDiffuseSlatIrradiances.at(t_Direction)[m_SlatSegmentsMesh.numberOfSegments].E_f
               - T_dir_dir(t_Direction);
    }

    double CVenetianCellEnergy::R_dir_dif(const CBeamDirection & t_Direction)
    {
        if(!m_DirectToDiffuseSlatIrradiances.count(t_Direction))
        {
            const auto diffuseViewFactors{
              beamToDiffuseViewFactors(Side::Back, t_Direction, *m_Cell, m_SlatSegmentsMesh)};
            const auto irradiance = directToDiffuseSlatIrradiances(
              diffuseViewFactors, slatsDiffuseRadiancesMatrix, m_SlatSegmentsMesh, t_Direction);
            std::lock_guard<std::mutex> lock(irradianceMutex);
            m_DirectToDiffuseSlatIrradiances[t_Direction] = irradiance;
        }

        return m_DirectToDiffuseSlatIrradiances.at(t_Direction)[0].E_b;
    }

    double CVenetianCellEnergy::T_dir_dir(const CBeamDirection & t_IncomingDirection,
                                          const CBeamDirection & t_OutgoingDirection)
    {
        if(!m_DirectToDirectSlatIrradiances.count(t_IncomingDirection))
        {
            // std::lock_guard<std::mutex> lock(irradianceMutex);
            const auto irradianceMatrix{formIrradianceMatrix(
              m_Cell->viewFactors(Side::Front, t_IncomingDirection), m_LayerProperties)};
            const auto diffuseViewFactors{
              beamToDiffuseViewFactors(Side::Front, t_IncomingDirection, *m_Cell, m_SlatSegmentsMesh)};
            m_DirectToDirectSlatIrradiances[t_IncomingDirection] = directToDiffuseSlatIrradiances(
              diffuseViewFactors, irradianceMatrix, m_SlatSegmentsMesh, t_IncomingDirection);
        }

        if(!m_DirectToDirectSlatRadiances.count(t_IncomingDirection))
        {
            m_DirectToDirectSlatRadiances[t_IncomingDirection] =
              directToDiffuseSlatRadiances(m_DirectToDirectSlatIrradiances.at(t_IncomingDirection),
                                           m_SlatSegmentsMesh,
                                           m_LayerProperties);
        }

        const auto & radiance = m_DirectToDirectSlatRadiances.at(t_IncomingDirection);

        const auto visibleFraction =
          m_Cell->visibleBeamSegmentFraction(Side::Back, t_OutgoingDirection);

        double aResult = 0;

        using FenestrationCommon::radians;

        for(size_t i = 1; i < radiance.size(); ++i)
        {
            aResult += visibleFraction[i] * radiance[i]
                       * (std::cos(radians(m_Cell->segmentAngle(i)))
                            * std::abs(std::tan(radians(t_OutgoingDirection.profileAngle())))
                          - std::sin(radians(m_Cell->segmentAngle(i))))
                       * m_Cell->segmentLength(i) / FenestrationCommon::WCE_PI;
        }

        return aResult
               / (m_Cell->segmentLength(0)
                  * std::cos(FenestrationCommon::radians(t_OutgoingDirection.theta())));
    }

    double CVenetianCellEnergy::R_dir_dir(const CBeamDirection & t_IncomingDirection,
                                          const CBeamDirection & t_OutgoingDirection)
    {
        if(!m_DirectToDirectSlatIrradiances.count(t_IncomingDirection))
        {
            // std::lock_guard<std::mutex> lock(irradianceMutex);
            const auto irradianceMatrix{formIrradianceMatrix(
              m_Cell->viewFactors(Side::Front, t_IncomingDirection), m_LayerProperties)};
            const auto diffuseViewFactors{
              beamToDiffuseViewFactors(Side::Back, t_IncomingDirection, *m_Cell, m_SlatSegmentsMesh)};
            m_DirectToDirectSlatIrradiances[t_IncomingDirection] = directToDiffuseSlatIrradiances(
              diffuseViewFactors, irradianceMatrix, m_SlatSegmentsMesh, t_IncomingDirection);
        }

        if(!m_DirectToDirectSlatRadiances.count(t_IncomingDirection))
        {
            m_DirectToDirectSlatRadiances[t_IncomingDirection] =
              directToDiffuseSlatRadiances(m_DirectToDirectSlatIrradiances.at(t_IncomingDirection),
                                           m_SlatSegmentsMesh,
                                           m_LayerProperties);
        }

        const auto & radiance = m_DirectToDirectSlatRadiances.at(t_IncomingDirection);

        const auto visibleFraction =
          m_Cell->visibleBeamSegmentFraction(Side::Front, t_OutgoingDirection);

        double aResult = 0;

        using FenestrationCommon::radians;

        for(size_t i = 1; i < radiance.size(); ++i)
        {
            aResult += visibleFraction[i] * radiance[i]
                       * (std::cos(radians(m_Cell->segmentAngle(i)))
                            * std::abs(std::tan(radians(t_OutgoingDirection.profileAngle())))
                          - std::sin(radians(m_Cell->segmentAngle(i))))
                       * m_Cell->segmentLength(i) / FenestrationCommon::WCE_PI;
        }

        return aResult / m_Cell->segmentLength(0);
    }

    double CVenetianCellEnergy::T_dif_dif()
    {
        auto B{diffuseRadiosities(m_SlatSegmentsMesh.numberOfSegments,
                                  m_SlatSegmentsMesh.surfaceIndexes,
                                  m_Cell->viewFactors())};

        std::vector<double> aSolution = solveSystem(slatsDiffuseRadiancesMatrix, B);

        return aSolution[m_SlatSegmentsMesh.numberOfSegments - 1];
    }

    double CVenetianCellEnergy::R_dif_dif()
    {
        auto B{diffuseRadiosities(m_SlatSegmentsMesh.numberOfSegments,
                                  m_SlatSegmentsMesh.surfaceIndexes,
                                  m_Cell->viewFactors())};

        std::vector<double> aSolution = solveSystem(slatsDiffuseRadiancesMatrix, B);

        return aSolution[m_SlatSegmentsMesh.numberOfSegments];
    }

    SlatSegmentsMesh::SlatSegmentsMesh(CVenetianCellDescription & cell) :
        numberOfSegments(static_cast<size_t>(cell.numberOfSegments() / 2)),
        surfaceIndexes{formBackSegmentsNumbering(numberOfSegments),
                       formFrontSegmentsNumbering(numberOfSegments)}
    {}

    std::vector<size_t> SlatSegmentsMesh::formFrontSegmentsNumbering(size_t nSegments)
    {
        std::vector<size_t> frontSegments;
        frontSegments.reserve(nSegments);
        for(size_t i = 0; i < nSegments; ++i)
        {
            frontSegments.push_back(2 * nSegments - 1 - i);
        }
        return frontSegments;
    }

    std::vector<size_t> SlatSegmentsMesh::formBackSegmentsNumbering(size_t nSegments)
    {
        std::vector<size_t> backSegments;
        backSegments.reserve(nSegments);
        for(size_t i = 0; i < nSegments; ++i)
        {
            backSegments.push_back(i);
        }
        return backSegments;
    }

    std::vector<SegmentIrradiance>
      directToDiffuseSlatIrradiances(const std::vector<double> & beamViewFactors,
                      const FenestrationCommon::SquareMatrix & radianceMatrix,
                      const SlatSegmentsMesh & mesh,
                      const CBeamDirection & t_IncomingDirection)
    {
        auto B = beamViewFactors;
        const auto aSolution{solveSystem(radianceMatrix, B)};

        std::vector<SegmentIrradiance> aIrradiances;
        for(size_t i = 0; i <= mesh.numberOfSegments; ++i)
        {
            SegmentIrradiance aIrr;
            if(i == 0)
            {
                aIrr.E_f = 1;
                aIrr.E_b = aSolution[mesh.numberOfSegments + i];
            }
            else if(i == mesh.numberOfSegments)
            {
                aIrr.E_f = aSolution[i - 1];
                aIrr.E_b = 0;
            }
            else
            {
                aIrr.E_f = aSolution[i - 1];
                aIrr.E_b = aSolution[mesh.numberOfSegments + i];
            }
            aIrradiances.push_back(aIrr);
        }

        return aIrradiances;
    }

    std::vector<double> diffuseRadiosities(const size_t numberOfSegments,
                                           const SegmentIndexes & indexes,
                                           const FenestrationCommon::SquareMatrix & viewFactors)
    {
        std::vector<double> B(2 * numberOfSegments);
        for(size_t i = 0; i < numberOfSegments; ++i)
        {
            B[i] = -viewFactors(indexes.backSideMeshIndex[0], indexes.frontSideMeshIndex[i]);
            B[i + numberOfSegments] =
              -viewFactors(indexes.backSideMeshIndex[0], indexes.backSideMeshIndex[i]);
        }

        return B;
    }

    std::vector<double>
      directToDiffuseSlatRadiances(const std::vector<SegmentIrradiance> & slatIrradiances,
                                      const SlatSegmentsMesh & slats,
                                      const LayerProperties & properties)
    {
        size_t numSlats = slatIrradiances.size();
        std::vector<double> aRadiances(2 * numSlats - 2);
        for(size_t i = 0; i < numSlats; ++i)
        {
            if(i == 0)
            {
                aRadiances[slats.surfaceIndexes.backSideMeshIndex[i]] = 1;
            }
            else if(i == numSlats - 1)
            {
                aRadiances[slats.surfaceIndexes.frontSideMeshIndex[i - 1]] = slatIrradiances[i].E_f;
            }
            else
            {
                aRadiances[slats.surfaceIndexes.backSideMeshIndex[i]] =
                  properties.Tf * slatIrradiances[i].E_f + properties.Rb * slatIrradiances[i].E_b;
                aRadiances[slats.surfaceIndexes.frontSideMeshIndex[i - 1]] =
                  properties.Tb * slatIrradiances[i].E_b + properties.Rf * slatIrradiances[i].E_f;
            }
        }

        return aRadiances;
    }

    namespace Helper
    {
        std::vector<BeamSegmentView>
          beamVector(const Side t_Side,
                     const size_t numberOfSegments,
                     const std::vector<Viewer::BeamViewFactor> & t_BeamViewFactors,
                     const double T_dir_dir)
        {
            std::vector<BeamSegmentView> B(2 * numberOfSegments);
            size_t index = 0;
            for(const Viewer::BeamViewFactor & aVF : t_BeamViewFactors)
            {
                if(aVF.enclosureIndex == 0)
                {   // Top
                    index = aVF.segmentIndex + 1;
                }
                else if(aVF.enclosureIndex == 1)
                {   // Bottom
                    index = numberOfSegments + 1 + aVF.segmentIndex;
                }
                else
                {
                    assert(
                      "Incorrect value for enclosure. Cannot have more than three enclosures.");
                }
                B[index].viewFactor = aVF.value;
                B[index].percentViewed = aVF.percentHit;
            }

            const std::map<Side, size_t> sideIndex{{Side::Front, numberOfSegments},
                                                   {Side::Back, 0}};

            B[sideIndex.at(t_Side)].viewFactor = T_dir_dir;
            B[sideIndex.at(t_Side)].percentViewed = T_dir_dir;

            return B;
        }
    }   // namespace Helper

    std::vector<double> beamToDiffuseViewFactors(const Side t_Side,
                                                 const CBeamDirection & t_IncomingDirection,
                                                 CVenetianCellDescription & cell,
                                                 const SlatSegmentsMesh & mesh)
    {
        const auto BeamViewFactors{
          Helper::beamVector(t_Side,
                             mesh.numberOfSegments,
                             cell.cellBeamViewFactors(t_Side, t_IncomingDirection),
                             cell.T_dir_dir(t_Side, t_IncomingDirection))};

        std::vector<double> B;
        B.reserve(2 * mesh.numberOfSegments);
        for(size_t i = 0; i < 2 * mesh.numberOfSegments; ++i)
        {
            size_t index = 0;
            if(i < mesh.numberOfSegments)
            {
                index = mesh.surfaceIndexes.frontSideMeshIndex[i];
            }
            else
            {
                index = mesh.surfaceIndexes.backSideMeshIndex[i - mesh.numberOfSegments];
            }
            B.push_back(-BeamViewFactors[index].viewFactor);
        }

        return B;
    }

    FenestrationCommon::SquareMatrix CVenetianCellEnergy::formIrradianceMatrix(
      const FenestrationCommon::SquareMatrix & viewFactors, const LayerProperties & properties)
    {
        size_t numSeg = m_SlatSegmentsMesh.numberOfSegments;
        SquareMatrix energy{2 * numSeg};

        auto fillUpperLeft = [&](size_t i, size_t j) -> double {
            if(i != numSeg - 1)
            {
                double value =
                  viewFactors(m_SlatSegmentsMesh.surfaceIndexes.backSideMeshIndex[i + 1],
                              m_SlatSegmentsMesh.surfaceIndexes.frontSideMeshIndex[j])
                    * properties.Tf
                  + viewFactors(m_SlatSegmentsMesh.surfaceIndexes.frontSideMeshIndex[i],
                                m_SlatSegmentsMesh.surfaceIndexes.frontSideMeshIndex[j])
                      * properties.Rf;
                if(i == j)
                {
                    value -= 1;
                }
                return value;
            }
            return (i == j) ? -1.0 : 0.0;
        };

        auto fillLowerLeft = [&](size_t i, size_t j) -> double {
            if(i != numSeg - 1)
            {
                return viewFactors(m_SlatSegmentsMesh.surfaceIndexes.backSideMeshIndex[i + 1],
                                   m_SlatSegmentsMesh.surfaceIndexes.backSideMeshIndex[j])
                         * properties.Tf
                       + viewFactors(m_SlatSegmentsMesh.surfaceIndexes.frontSideMeshIndex[i],
                                     m_SlatSegmentsMesh.surfaceIndexes.backSideMeshIndex[j])
                           * properties.Rf;
            }
            return 0.0;
        };

        auto fillUpperRight = [&](size_t i, size_t j) -> double {
            if(i != 0)
            {
                return viewFactors(m_SlatSegmentsMesh.surfaceIndexes.frontSideMeshIndex[i - 1],
                                   m_SlatSegmentsMesh.surfaceIndexes.frontSideMeshIndex[j])
                         * properties.Tb
                       + viewFactors(m_SlatSegmentsMesh.surfaceIndexes.backSideMeshIndex[i],
                                     m_SlatSegmentsMesh.surfaceIndexes.frontSideMeshIndex[j])
                           * properties.Rb;
            }
            return 0.0;
        };

        auto fillLowerRight = [&](size_t i, size_t j) -> double {
            if(i != 0)
            {
                double value =
                  viewFactors(m_SlatSegmentsMesh.surfaceIndexes.frontSideMeshIndex[i - 1],
                              m_SlatSegmentsMesh.surfaceIndexes.backSideMeshIndex[j])
                    * properties.Tb
                  + viewFactors(m_SlatSegmentsMesh.surfaceIndexes.backSideMeshIndex[i],
                                m_SlatSegmentsMesh.surfaceIndexes.backSideMeshIndex[j])
                      * properties.Rb;
                if(i == j)
                {
                    value -= 1;
                }
                return value;
            }
            return (i == j) ? -1.0 : 0.0;
        };

        for(size_t i = 0; i < numSeg; ++i)
        {
            for(size_t j = 0; j < numSeg; ++j)
            {
                energy(j, i) = fillUpperLeft(i, j);
                energy(j + numSeg, i) = fillLowerLeft(i, j);
                energy(j, i + numSeg) = fillUpperRight(i, j);
                energy(j + numSeg, i + numSeg) = fillLowerRight(i, j);
            }
        }

        return energy;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    //  CVenetianEnergy
    ////////////////////////////////////////////////////////////////////////////////////////////
    CVenetianEnergy::CVenetianEnergy()
    {}

    CVenetianEnergy::CVenetianEnergy(
      const CMaterial & t_Material,
      const std::shared_ptr<CVenetianCellDescription> & t_ForwardFlowGeometry,
      const std::shared_ptr<CVenetianCellDescription> & t_BackwardFlowGeometry)
    {
        // clang-format off
        createForwardAndBackward({t_Material.getProperty(Property::T, Side::Front),
                                  t_Material.getProperty(Property::R, Side::Front),
                                  t_Material.getProperty(Property::T, Side::Back),
                                  t_Material.getProperty(Property::R, Side::Back)},
                                    t_ForwardFlowGeometry,
                                    t_BackwardFlowGeometry);
        // clang-format on
    }

    CVenetianEnergy::CVenetianEnergy(
      const LayerProperties & properties,
      const std::shared_ptr<CVenetianCellDescription> & t_ForwardFlowGeometry,
      const std::shared_ptr<CVenetianCellDescription> & t_BackwardFlowGeometry)
    {
        createForwardAndBackward(properties, t_ForwardFlowGeometry, t_BackwardFlowGeometry);
    }

    CVenetianCellEnergy & CVenetianEnergy::getCell(const Side t_Side)
    {
        return m_CellEnergy.at(t_Side);
    }

    void CVenetianEnergy::createForwardAndBackward(
      const LayerProperties & layerProperties,
      const std::shared_ptr<CVenetianCellDescription> & t_ForwardFlowGeometry,
      const std::shared_ptr<CVenetianCellDescription> & t_BackwardFlowGeometry)
    {
        assert(t_ForwardFlowGeometry != nullptr);
        assert(t_BackwardFlowGeometry != nullptr);
        m_CellEnergy[Side::Front] = CVenetianCellEnergy(t_ForwardFlowGeometry, layerProperties);
        m_CellEnergy[Side::Back] = CVenetianCellEnergy(t_BackwardFlowGeometry, layerProperties);
    }
}   // namespace SingleLayerOptics