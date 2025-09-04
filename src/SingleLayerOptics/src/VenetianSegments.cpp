#include <memory>
#include <mutex>
#include <cassert>

#include <WCECommon.hpp>

#include "VenetianSegments.hpp"

#include "VenetianCellDescription.hpp"
#include "BeamDirection.hpp"
#include "BSDFDirections.hpp"
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
        return m_Cell->Beam_dir_dir(Side::Front, t_Direction);
    }

    double CVenetianCellEnergy::T_dir_dif(const CBeamDirection & t_Direction)
    {
        if(!m_DirectToDiffuseSlatIrradiances.count(t_Direction))
        {
            const auto diffuseViewFactors{
              beamToDiffuseViewFactors(Side::Front, t_Direction, *m_Cell, m_SlatSegmentsMesh)};

            const auto irradiance =
              slatIrradiances(diffuseViewFactors, slatsDiffuseRadiancesMatrix, m_SlatSegmentsMesh);
            std::lock_guard lock(irradianceMutex);
            m_DirectToDiffuseSlatIrradiances[t_Direction] = irradiance;
        }

        // Total energy accounts for direct to direct component. That needs to be subtracted since
        // only direct to diffuse is of interest
        return m_DirectToDiffuseSlatIrradiances.at(t_Direction)[m_SlatSegmentsMesh.numberOfSegments]
                 .E_f
               - T_dir_dir(t_Direction);
    }

    double CVenetianCellEnergy::R_dir_dif(const CBeamDirection & t_Direction)
    {
        if(!m_DirectToDiffuseSlatIrradiances.count(t_Direction))
        {
            const auto diffuseViewFactors{
              beamToDiffuseViewFactors(Side::Back, t_Direction, *m_Cell, m_SlatSegmentsMesh)};
            const auto irradiance =
              slatIrradiances(diffuseViewFactors, slatsDiffuseRadiancesMatrix, m_SlatSegmentsMesh);
            std::lock_guard lock(irradianceMutex);
            m_DirectToDiffuseSlatIrradiances[t_Direction] = irradiance;
        }

        return m_DirectToDiffuseSlatIrradiances.at(t_Direction)[0].E_b;
    }

    double CVenetianCellEnergy::T_dir_dir(const CBeamDirection & t_IncomingDirection,
                                          const CBeamDirection & t_OutgoingDirection)
    {
        return calculateOutgoingRadiance(
          Side::Back, t_OutgoingDirection, directToDirectSlatRadiances(t_IncomingDirection));
    }

    double CVenetianCellEnergy::R_dir_dir(const CBeamDirection & t_IncomingDirection,
                                          const CBeamDirection & t_OutgoingDirection)
    {
        return calculateOutgoingRadiance(
          Side::Front, t_OutgoingDirection, directToDirectSlatRadiances(t_IncomingDirection));
    }

    namespace Helper
    {
        // In case of outgoing front and incoming back, unit vector need to be inverse.
        Viewer::CSegment2D unitVector(const CBeamDirection & t_Direction,
                                      const BSDFDirection t_BSDFDirection,
                                      const Side t_Side)
        {
            auto result = t_Direction.unitVector();
            if((t_BSDFDirection == BSDFDirection::Outgoing
                 && t_Side == FenestrationCommon::Side::Front)
               || (t_BSDFDirection == BSDFDirection::Incoming
                    && t_Side == FenestrationCommon::Side::Back))
            {
                result = Viewer::CSegment2D(result.startPoint(),
                                            {-result.endPoint().x(), result.endPoint().y()});
            }
            return result;
        }
    }   // namespace Helper

    double
      CVenetianCellEnergy::calculateOutgoingRadiance(const FenestrationCommon::Side side,
                                                     const CBeamDirection & t_OutgoingDirection,
                                                     const std::vector<double> & slatRadiances)
    {
        const auto visibleFraction = m_Cell->visibleBeamSegmentFractionSlatsOnly(
          side, BSDFDirection::Outgoing, t_OutgoingDirection);

        const auto slats = m_Cell->getSlats();

        assert(slatRadiances.size() == visibleFraction.size()
               && slatRadiances.size() == slats.size());
        auto outgoingUnitVector{
          Helper::unitVector(t_OutgoingDirection, BSDFDirection::Outgoing, side)};

        double aResult = 0;
        for(size_t i = 0; i < slatRadiances.size(); ++i)
        {
            aResult +=
              visibleFraction[i] * slatRadiances[i] * slats[i].length()
              * std::abs(slats[i].surfaceUnitNormal().dotProduct(outgoingUnitVector.endPoint()));
        }

        return aResult
               / (WCE_PI * m_Cell->getCellSpacing()
                  * cos(radians(t_OutgoingDirection.profileAngle())));
    }

    double CVenetianCellEnergy::T_dif_dif()
    {
        auto B{diffuseRadiosities(m_SlatSegmentsMesh.numberOfSegments,
                                  m_SlatSegmentsMesh.surfaceIndexes,
                                  m_Cell->viewFactors())};

        return solveSystem(slatsDiffuseRadiancesMatrix, B)[m_SlatSegmentsMesh.numberOfSegments - 1];
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
      slatIrradiances(const std::vector<double> & beamViewFactors,
                      const FenestrationCommon::SquareMatrix & radianceMatrix,
                      const SlatSegmentsMesh & mesh)
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

    std::vector<double>
      directUniformSlatRadiances(const std::vector<SegmentIrradiance> & vector,
                                 const FenestrationCommon::SquareMatrix & radiancesMatrix,
                                 const LayerProperties & properties)
    {
        // Forming left side of the equations for direct to direct radiances solution.
        // Radiances matrix is already formed and used in several different places.
        std::vector<double> rightSide;
        rightSide.reserve(radiancesMatrix.size() + 2 * vector.size() + 2);

        // Iterating through the vector backward
        std::for_each(std::begin(vector), std::end(vector), [&](const SegmentIrradiance & segment) {
            rightSide.push_back(-properties.Tb * segment.E_b - properties.Rf * segment.E_f);
        });

        // Indoor is ignored and set to zero
        rightSide.push_back(0);
        rightSide.push_back(0);

        // Iterating through the vector forward
        std::for_each(std::begin(vector), std::end(vector), [&](const SegmentIrradiance & segment) {
            rightSide.push_back(-properties.Tf * segment.E_f - properties.Rb * segment.E_b);
        });

        // Solve the system and get the solution vector
        std::vector<double> solution = solveSystem(radiancesMatrix, rightSide);

        size_t n = vector.size();

        // Remove the two middle items from the solution vector
        if(solution.size() > n + 1)
        {
            solution.erase(solution.begin() + n, solution.begin() + n + 2);
        }

        // Switch the first and second halves of the solution
        auto mid = solution.begin() + solution.size() / 2;
        std::rotate(solution.begin(), mid, solution.end());

        std::reverse(mid, solution.end());

        return solution;
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
        const auto BeamViewFactors{Helper::beamVector(
          t_Side,
          mesh.numberOfSegments,
          cell.cellBeamViewFactors(t_Side, BSDFDirection::Incoming, t_IncomingDirection),
          cell.Beam_dir_dir(t_Side, t_IncomingDirection))};

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

    namespace Helper
    {
        //! Filtering BeamViewFactors by enclosure index and thus filtering it for upper and lower
        //! slats
        std::vector<Viewer::BeamViewFactor>
          filterByEnclosureIndex(const std::vector<Viewer::BeamViewFactor> & vf,
                                 SingleLayerOptics::SlatPosition slatPosition)
        {
            std::vector<Viewer::BeamViewFactor> filtered;
            std::copy_if(vf.begin(),
                         vf.end(),
                         std::back_inserter(filtered),
                         [slatPosition](const Viewer::BeamViewFactor & bvf) {
                             return bvf.enclosureIndex == static_cast<size_t>(slatPosition);
                         });
            return filtered;
        }

        //! Calculates slat irradiances from the incoming beam
        std::vector<double> slatIrradiancesFromBeam(const std::vector<Viewer::BeamViewFactor> & vf,
                                                    const Viewer::CGeometry2D & slats,
                                                    const CBeamDirection & t_IncomingDirection)
        {
            std::vector<double> result(slats.segments().size(),
                                       0.0);   // Initialize result with zeros

            const auto incomingUnitVector = t_IncomingDirection.unitVector();

            for(const auto & aVF : vf)
            {
                if(aVF.segmentIndex < slats.segments().size())
                {
                    // Calculate irradiance and store it in the corresponding position
                    result[aVF.segmentIndex] =
                      aVF.percentHit
                      * std::abs(slats.segments()[aVF.segmentIndex].surfaceUnitNormal().dotProduct(
                        incomingUnitVector.endPoint()))
                      / std::cos(FenestrationCommon::radians(t_IncomingDirection.profileAngle()));
                }
            }

            return result;
        }

    }   // namespace Helper

    std::vector<SegmentIrradiance>
      CVenetianCellEnergy::directToDirectSlatIrradiances(const CBeamDirection & t_IncomingDirection)
    {
        using SingleLayerOptics::SlatPosition;
        //! Irradiances are always calculated from the front side perspective
        const auto vf{
          m_Cell->cellBeamViewFactors(Side::Front, BSDFDirection::Incoming, t_IncomingDirection)};

        const auto upperSlatIrradiances{Helper::slatIrradiancesFromBeam(
          Helper::filterByEnclosureIndex(vf, SingleLayerOptics::SlatPosition::Top),
          m_Cell->getSlats(SlatPosition::Top),
          t_IncomingDirection)};

        const auto lowerSlatIrradiances{Helper::slatIrradiancesFromBeam(
          Helper::filterByEnclosureIndex(vf, SingleLayerOptics::SlatPosition::Bottom),
          m_Cell->getSlats(SlatPosition::Bottom),
          t_IncomingDirection)};

        // They must be of the same size
        assert(upperSlatIrradiances.size() == lowerSlatIrradiances.size());

        // Ensure both vectors are of the same size
        size_t size = upperSlatIrradiances.size();
        std::vector<SegmentIrradiance> result(size);

        // Fill the result vector
        for(size_t i = 0; i < size; ++i)
        {
            result[i] = {lowerSlatIrradiances[size - i - 1], upperSlatIrradiances[i]};
        }

        return result;
    }

    std::vector<double>
      CVenetianCellEnergy::directToDirectSlatRadiances(const CBeamDirection & t_IncomingDirection)
    {
        if(!m_DirectToDirectSlatIrradiances.count(t_IncomingDirection))
        {
            m_DirectToDirectSlatIrradiances[t_IncomingDirection] =
              directToDirectSlatIrradiances(t_IncomingDirection);
        }

        if(!m_DirectToDirectSlatRadiances.count(t_IncomingDirection))
        {
            m_DirectToDirectSlatRadiances[t_IncomingDirection] =
              directUniformSlatRadiances(m_DirectToDirectSlatIrradiances.at(t_IncomingDirection),
                                         slatsDiffuseRadiancesMatrix,
                                         m_LayerProperties);
        }

        // Radiance results always CW starting from the left segment on the upper slat
        return m_DirectToDirectSlatRadiances.at(t_IncomingDirection);
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