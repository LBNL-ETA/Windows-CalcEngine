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
    CVenetianCellEnergy::CVenetianCellEnergy() : m_Cell(nullptr), m_Tf(0), m_Tb(0), m_Rf(0), m_Rb(0)
    {}

    CVenetianCellEnergy::CVenetianCellEnergy(
      const std::shared_ptr<CVenetianCellDescription> & t_Cell,
      const double Tf,
      const double Tb,
      const double Rf,
      const double Rb) :
        m_Cell(t_Cell),
        m_Tf(Tf),
        m_Tb(Tb),
        m_Rf(Rf),
        m_Rb(Rb),
        m_SlatSegmentsMesh(*m_Cell, m_Tf, m_Tb, m_Rf, m_Rb)
    {}

    double CVenetianCellEnergy::T_dir_dir(const CBeamDirection & t_Direction)
    {
        return m_Cell->T_dir_dir(Side::Front, t_Direction);
    }

    double CVenetianCellEnergy::T_dir_dif(const CBeamDirection & t_Direction)
    {
        if(!m_SlatIrradiances.count(t_Direction))
        {
            const auto irradiance = slatIrradiances(m_Cell, m_SlatSegmentsMesh, t_Direction);
            std::lock_guard<std::mutex> lock(irradianceMutex);
            m_SlatIrradiances[t_Direction] = irradiance;
        }

        // Total energy accounts for direct to direct component. That needs to be subtracted since
        // only direct to diffuse is of interest
        return m_SlatIrradiances.at(t_Direction)[m_SlatSegmentsMesh.numberOfSegments].E_f
               - T_dir_dir(t_Direction);
    }

    double CVenetianCellEnergy::R_dir_dif(const CBeamDirection & t_Direction)
    {
        if(!m_SlatIrradiances.count(t_Direction))
        {
            const auto irradiance = slatIrradiances(m_Cell, m_SlatSegmentsMesh, t_Direction);
            std::lock_guard<std::mutex> lock(irradianceMutex);
            m_SlatIrradiances[t_Direction] = irradiance;
        }

        return m_SlatIrradiances.at(t_Direction)[0].E_b;
    }

    double CVenetianCellEnergy::T_dir_dir(const CBeamDirection & t_IncomingDirection,
                                          const CBeamDirection & t_OutgoingDirection)
    {
        if(!m_SlatIrradiances.count(t_IncomingDirection))
        {
            const auto irradiance{slatIrradiances(m_Cell, m_SlatSegmentsMesh, t_IncomingDirection)};

            std::lock_guard<std::mutex> lock(irradianceMutex);
            m_SlatIrradiances[t_IncomingDirection] = irradiance;
        }

        const auto radiance{slatRadiances(
          t_IncomingDirection, m_SlatIrradiances.at(t_IncomingDirection), m_SlatSegmentsMesh)};

        std::vector<BeamSegmentView> BVF =
          beamVector(Side::Back,
                     m_SlatSegmentsMesh.numberOfSegments,
                     m_Cell->beamViewFactors(Side::Back, t_OutgoingDirection),
                     m_Cell->T_dir_dir(Side::Back, t_OutgoingDirection));

        double aResult = 0;

        // Counting starts from one because this should exclude beam to beam energy.
        for(size_t i = 1; i < radiance.size(); ++i)
        {
            aResult +=
              radiance[i] * BVF[i].percentViewed * BVF[i].viewFactor / m_Cell->segmentLength(i);
        }

        // Area weighting. Needs to be multiplied with number of segments
        double insideSegLength = m_Cell->segmentLength(m_SlatSegmentsMesh.numberOfSegments);

        assert(insideSegLength != 0);

        return insideSegLength * aResult;
    }

    double CVenetianCellEnergy::R_dir_dir(const CBeamDirection & t_IncomingDirection,
                                          const CBeamDirection & t_OutgoingDirection)
    {
        if(!m_SlatIrradiances.count(t_IncomingDirection))
        {
            const auto irradiance{slatIrradiances(m_Cell, m_SlatSegmentsMesh, t_IncomingDirection)};

            std::lock_guard<std::mutex> lock(irradianceMutex);
            m_SlatIrradiances[t_IncomingDirection] = irradiance;
        }

        const auto radiance = slatRadiances(
          t_IncomingDirection, m_SlatIrradiances[t_IncomingDirection], m_SlatSegmentsMesh);

        std::vector<BeamSegmentView> BVF =
          beamVector(Side::Front,
                     m_SlatSegmentsMesh.numberOfSegments,
                     m_Cell->beamViewFactors(Side::Front, t_OutgoingDirection),
                     m_Cell->T_dir_dir(Side::Front, t_OutgoingDirection));

        double aResult = 0;

        for(size_t i = 1; i < radiance.size(); ++i)
        {
            aResult +=
              radiance[i] * BVF[i].percentViewed * BVF[i].viewFactor / m_Cell->segmentLength(i);
        }

        // Area weighting. Needs to be multiplied with number of segments
        double insideSegLength = m_Cell->segmentLength(m_SlatSegmentsMesh.numberOfSegments);

        assert(insideSegLength != 0);

        return insideSegLength * aResult;
    }

    double CVenetianCellEnergy::T_dif_dif()
    {
        auto B{diffuseRadiosities(m_SlatSegmentsMesh.numberOfSegments,
                                  m_SlatSegmentsMesh.surfaceIndexes,
                                  m_Cell->viewFactors())};

        std::vector<double> aSolution = solveSystem(m_SlatSegmentsMesh.slatsRadiancesMatrix, B);

        return aSolution[m_SlatSegmentsMesh.numberOfSegments - 1];
    }

    double CVenetianCellEnergy::R_dif_dif()
    {
        auto B{diffuseRadiosities(m_SlatSegmentsMesh.numberOfSegments,
                                  m_SlatSegmentsMesh.surfaceIndexes,
                                  m_Cell->viewFactors())};

        std::vector<double> aSolution = solveSystem(m_SlatSegmentsMesh.slatsRadiancesMatrix, B);

        return aSolution[m_SlatSegmentsMesh.numberOfSegments];
    }

    SlatSegmentsMesh::SlatSegmentsMesh(
      CVenetianCellDescription & cell, double Tf, double Tb, double Rf, double Rb) :
        numberOfSegments(static_cast<size_t>(cell.numberOfSegments() / 2)),
        surfaceIndexes{formBackSegmentsNumbering(numberOfSegments),
                       formFrontSegmentsNumbering(numberOfSegments)},
        slatsRadiancesMatrix(formIrradianceMatrix(cell.viewFactors(), Tf, Tb, Rf, Rb))
    {}

    std::vector<SegmentIrradiance>
      slatIrradiances(const std::shared_ptr<CVenetianCellDescription> & cell,
                      const SlatSegmentsMesh & mesh,
                      const CBeamDirection & t_IncomingDirection)
    {
        std::vector<SegmentIrradiance> aIrradiances;

        // Beam view factors with percentage view
        std::vector<BeamSegmentView> BVF =
          beamVector(Side::Front,
                     mesh.numberOfSegments,
                     cell->beamViewFactors(Side::Front, t_IncomingDirection),
                     cell->T_dir_dir(Side::Front, t_IncomingDirection));

        // Need to calculate irradiances based on current energy state. Need to do reordering
        // according to energy slat numbering
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
            B.push_back(-BVF[index].viewFactor);
        }

        const auto aSolution{solveSystem(mesh.slatsRadiancesMatrix, B)};

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
                assert("Incorrect value for enclosure. Cannot have more than three enclosures.");
            }
            B[index].viewFactor = aVF.value;
            B[index].percentViewed = aVF.percentHit;
        }

        const std::map<Side, size_t> sideIndex{{Side::Front, numberOfSegments}, {Side::Back, 0}};

        B[sideIndex.at(t_Side)].viewFactor = T_dir_dir;

        return B;
    }

    std::vector<double>
      CVenetianCellEnergy::slatRadiances(const CBeamDirection & t_IncomingDirection,
                                         const std::vector<SegmentIrradiance> & slatIrradiances,
                                         const SlatSegmentsMesh & slats)
    {
        if(m_SlatRadiances.count(t_IncomingDirection))
        {
            return m_SlatRadiances.at(t_IncomingDirection);
        }


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
                  m_Tf * slatIrradiances[i].E_f + m_Rb * slatIrradiances[i].E_b;
                aRadiances[slats.surfaceIndexes.frontSideMeshIndex[i - 1]] =
                  m_Tb * slatIrradiances[i].E_b + m_Rf * slatIrradiances[i].E_f;
            }
        }

        m_SlatRadiances[t_IncomingDirection] = aRadiances;

        return aRadiances;
    }

    FenestrationCommon::SquareMatrix
      SlatSegmentsMesh::formIrradianceMatrix(const FenestrationCommon::SquareMatrix & viewFactors,
                                             double Tf,
                                             double Tb,
                                             double Rf,
                                             double Rb)
    {
        size_t numSeg = numberOfSegments;
        SquareMatrix energy{2 * numSeg};

        auto fillUpperLeft = [&](size_t i, size_t j) -> double {
            if(i != numSeg - 1)
            {
                double value = viewFactors(surfaceIndexes.backSideMeshIndex[i + 1],
                                           surfaceIndexes.frontSideMeshIndex[j])
                                 * Tf
                               + viewFactors(surfaceIndexes.frontSideMeshIndex[i],
                                             surfaceIndexes.frontSideMeshIndex[j])
                                   * Rf;
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
                return viewFactors(surfaceIndexes.backSideMeshIndex[i + 1],
                                   surfaceIndexes.backSideMeshIndex[j])
                         * Tf
                       + viewFactors(surfaceIndexes.frontSideMeshIndex[i],
                                     surfaceIndexes.backSideMeshIndex[j])
                           * Rf;
            }
            return 0.0;
        };

        auto fillUpperRight = [&](size_t i, size_t j) -> double {
            if(i != 0)
            {
                return viewFactors(surfaceIndexes.frontSideMeshIndex[i - 1],
                                   surfaceIndexes.frontSideMeshIndex[j])
                         * Tb
                       + viewFactors(surfaceIndexes.backSideMeshIndex[i],
                                     surfaceIndexes.frontSideMeshIndex[j])
                           * Rb;
            }
            return 0.0;
        };

        auto fillLowerRight = [&](size_t i, size_t j) -> double {
            if(i != 0)
            {
                double value = viewFactors(surfaceIndexes.frontSideMeshIndex[i - 1],
                                           surfaceIndexes.backSideMeshIndex[j])
                                 * Tb
                               + viewFactors(surfaceIndexes.backSideMeshIndex[i],
                                             surfaceIndexes.backSideMeshIndex[j])
                                   * Rb;
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
        double Tf = t_Material.getProperty(Property::T, Side::Front);
        double Tb = t_Material.getProperty(Property::T, Side::Back);
        double Rf = t_Material.getProperty(Property::R, Side::Front);
        double Rb = t_Material.getProperty(Property::R, Side::Back);
        createForwardAndBackward(Tf, Tb, Rf, Rb, t_ForwardFlowGeometry, t_BackwardFlowGeometry);
    }

    CVenetianEnergy::CVenetianEnergy(
      const double Tf,
      const double Tb,
      const double Rf,
      const double Rb,
      const std::shared_ptr<CVenetianCellDescription> & t_ForwardFlowGeometry,
      const std::shared_ptr<CVenetianCellDescription> & t_BackwardFlowGeometry)
    {
        createForwardAndBackward(Tf, Tb, Rf, Rb, t_ForwardFlowGeometry, t_BackwardFlowGeometry);
    }

    CVenetianCellEnergy & CVenetianEnergy::getCell(const Side t_Side)
    {
        return m_CellEnergy.at(t_Side);
    }

    void CVenetianEnergy::createForwardAndBackward(
      double Tf,
      double Tb,
      double Rf,
      double Rb,
      const std::shared_ptr<CVenetianCellDescription> & t_ForwardFlowGeometry,
      const std::shared_ptr<CVenetianCellDescription> & t_BackwardFlowGeometry)
    {
        assert(t_ForwardFlowGeometry != nullptr);
        assert(t_BackwardFlowGeometry != nullptr);
        m_CellEnergy[Side::Front] = CVenetianCellEnergy(t_ForwardFlowGeometry, Tf, Tb, Rf, Rb);
        m_CellEnergy[Side::Back] = CVenetianCellEnergy(t_BackwardFlowGeometry, Tf, Tb, Rf, Rb);
    }
}   // namespace SingleLayerOptics