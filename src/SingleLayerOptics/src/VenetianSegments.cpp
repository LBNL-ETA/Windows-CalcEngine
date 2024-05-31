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
        m_SlatSegments(*m_Cell, m_Tf, m_Tb, m_Rf, m_Rb)
    {}

    double CVenetianCellEnergy::T_dir_dir(const CBeamDirection & t_Direction)
    {
        return m_Cell->T_dir_dir(Side::Front, t_Direction);
    }

    double CVenetianCellEnergy::T_dir_dif(const CBeamDirection & t_Direction)
    {
        const auto irradiance =
          slatIrradiances(t_Direction, m_SlatSegments, m_SlatSegments.slatsViewFactorsMatrix);

        // Total energy accounts for direct to direct component. That needs to be subtracted since
        // only direct to diffuse is of interest
        return irradiance[m_SlatSegments.numberOfSegments].E_f - T_dir_dir(t_Direction);
    }

    double CVenetianCellEnergy::R_dir_dif(const CBeamDirection & t_Direction)
    {
        const auto irradiance{
          slatIrradiances(t_Direction, m_SlatSegments, m_SlatSegments.slatsViewFactorsMatrix)};

        return irradiance[0].E_b;
    }

    double CVenetianCellEnergy::T_dir_dir(const CBeamDirection & t_IncomingDirection,
                                          const CBeamDirection & t_OutgoingDirection)
    {
        const auto radiance{
          slatRadiances(t_IncomingDirection, m_SlatSegments, m_SlatSegments.slatsViewFactorsMatrix)};

        std::vector<BeamSegmentView> BVF = beamVector(t_OutgoingDirection, Side::Back);

        double aResult = 0;

        // Counting starts from one because this should exclude beam to beam energy.
        // double totalSegmentsLength = 0;
        for(size_t i = 1; i < radiance.size(); ++i)
        {
            aResult +=
              radiance[i] * BVF[i].percentViewed * BVF[i].viewFactor / m_Cell->segmentLength(i);
        }

        // Area weighting. Needs to be multiplied with number of segments
        double insideSegLength = m_Cell->segmentLength(m_SlatSegments.numberOfSegments);

        assert(insideSegLength != 0);

        return insideSegLength * aResult;
    }

    double CVenetianCellEnergy::R_dir_dir(const CBeamDirection & t_IncomingDirection,
                                          const CBeamDirection & t_OutgoingDirection)
    {
        const auto radiance =
          slatRadiances(t_IncomingDirection, m_SlatSegments, m_SlatSegments.slatsViewFactorsMatrix);

        std::vector<BeamSegmentView> BVF = beamVector(t_OutgoingDirection, Side::Front);

        double aResult = 0;

        for(size_t i = 1; i < radiance.size(); ++i)
        {
            aResult +=
              radiance[i] * BVF[i].percentViewed * BVF[i].viewFactor / m_Cell->segmentLength(i);
        }

        // Area weighting. Needs to be multiplied with number of segments
        double insideSegLength = m_Cell->segmentLength(m_SlatSegments.numberOfSegments);

        assert(insideSegLength != 0);

        return insideSegLength * aResult;
    }

    double CVenetianCellEnergy::T_dif_dif()
    {
        const auto numSeg{m_SlatSegments.numberOfSegments};

        auto B{diffuseVector(m_SlatSegments, m_Cell->viewFactors())};

        std::vector<double> aSolution = solveSystem(m_SlatSegments.slatsViewFactorsMatrix, B);

        return aSolution[numSeg - 1];
    }

    double CVenetianCellEnergy::R_dif_dif()
    {
        auto B{diffuseVector(m_SlatSegments, m_Cell->viewFactors())};

        std::vector<double> aSolution = solveSystem(m_SlatSegments.slatsViewFactorsMatrix, B);

        return aSolution[m_SlatSegments.numberOfSegments];
    }

    std::vector<SegmentIrradiance>
      CVenetianCellEnergy::slatIrradiances(const CBeamDirection & t_IncomingDirection,
                                           const SlatSegmentsMesh & slats,
                                           const FenestrationCommon::SquareMatrix & energy)
    {
        std::lock_guard<std::mutex> lock(irradianceMutex);
        if(m_SlatIrradiances.count(t_IncomingDirection))
        {
            return m_SlatIrradiances.at(t_IncomingDirection);
        }

        std::vector<SegmentIrradiance> aIrradiances;

        size_t numSeg{slats.numberOfSegments};

        // Beam view factors with percentage view
        std::vector<BeamSegmentView> BVF = beamVector(t_IncomingDirection, Side::Front);

        // Need to calculate irradiances based on current energy state. Need to do reordering
        // according to energy slat numbering
        std::vector<double> B;
        B.reserve(2 * numSeg);
        for(size_t i = 0; i < 2 * numSeg; ++i)
        {
            size_t index = 0;
            if(i < numSeg)
            {
                index = slats.frontSideMeshIndex[i];
            }
            else
            {
                index = slats.backSideMeshIndex[i - numSeg];
            }
            B.push_back(-BVF[index].viewFactor);
        }

        const auto aSolution{solveSystem(energy, B)};

        for(size_t i = 0; i <= numSeg; ++i)
        {
            SegmentIrradiance aIrr;
            if(i == 0)
            {
                aIrr.E_f = 1;
                aIrr.E_b = aSolution[numSeg + i];
            }
            else if(i == numSeg)
            {
                aIrr.E_f = aSolution[i - 1];
                aIrr.E_b = 0;
            }
            else
            {
                aIrr.E_f = aSolution[i - 1];
                aIrr.E_b = aSolution[numSeg + i];
            }
            aIrradiances.push_back(aIrr);
        }

        m_SlatIrradiances[t_IncomingDirection] = aIrradiances;

        return aIrradiances;
    }

    std::vector<double>
      CVenetianCellEnergy::slatRadiances(const CBeamDirection & t_IncomingDirection,
                                         const SlatSegmentsMesh & slats,
                                         const FenestrationCommon::SquareMatrix & energy)
    {
        if(m_SlatRadiances.count(t_IncomingDirection))
        {
            return m_SlatRadiances.at(t_IncomingDirection);
        }

        const auto irradiance{slatIrradiances(t_IncomingDirection, slats, energy)};
        m_SlatIrradiances[t_IncomingDirection] = irradiance;
        size_t numSlats = irradiance.size();
        std::vector<double> aRadiances(2 * numSlats - 2);
        for(size_t i = 0; i < numSlats; ++i)
        {
            if(i == 0)
            {
                aRadiances[slats.backSideMeshIndex[i]] = 1;
            }
            else if(i == numSlats - 1)
            {
                aRadiances[slats.frontSideMeshIndex[i - 1]] = irradiance[i].E_f;
            }
            else
            {
                aRadiances[slats.backSideMeshIndex[i]] = m_Tf * irradiance[i].E_f + m_Rb * irradiance[i].E_b;
                aRadiances[slats.frontSideMeshIndex[i - 1]] = m_Tb * irradiance[i].E_b + m_Rf * irradiance[i].E_f;
            }
        }

        m_SlatRadiances[t_IncomingDirection] = aRadiances;

        return aRadiances;
    }

    FenestrationCommon::SquareMatrix SlatSegmentsMesh::formViewFactorsMatrix(
      FenestrationCommon::SquareMatrix && viewFactors, double Tf, double Tb, double Rf, double Rb)
    {
        size_t numSeg = numberOfSegments;

        // Create energy matrix
        SquareMatrix energy{2 * numSeg};

        // Building upper left side of matrix
        for(size_t i = 0; i < numSeg; ++i)
        {
            for(size_t j = 0; j < numSeg; ++j)
            {
                if(i != numSeg - 1)
                {
                    double value = viewFactors(backSideMeshIndex[i + 1], frontSideMeshIndex[j]) * Tf + viewFactors(frontSideMeshIndex[i], frontSideMeshIndex[j]) * Rf;
                    if(i == j)
                    {
                        value -= 1;
                    }
                    energy(j, i) = value;
                }
                else
                {
                    if(i != j)
                    {
                        energy(j, i) = 0;
                    }
                    else
                    {
                        energy(j, i) = -1;
                    }
                }
            }
        }

        // Building lower left side of matrix
        for(size_t i = 0; i < numSeg; ++i)
        {
            for(size_t j = 0; j < numSeg; ++j)
            {
                if(i != numSeg - 1)
                {
                    const double value =
                      viewFactors(backSideMeshIndex[i + 1], backSideMeshIndex[j]) * Tf + viewFactors(frontSideMeshIndex[i], backSideMeshIndex[j]) * Rf;
                    energy(j + numSeg, i) = value;
                }
                else
                {
                    energy(j + numSeg, i) = 0;
                }
            }
        }

        // Building upper right side of matrix
        for(size_t i = 0; i < numSeg; ++i)
        {
            for(size_t j = 0; j < numSeg; ++j)
            {
                if(i != 0)
                {
                    const double value =
                      viewFactors(frontSideMeshIndex[i - 1], frontSideMeshIndex[j]) * Tb + viewFactors(backSideMeshIndex[i], frontSideMeshIndex[j]) * Rb;
                    energy(j, i + numSeg) = value;
                }
                else
                {
                    energy(j, i + numSeg) = 0;
                }
            }
        }

        // Building lower right side of matrix
        for(size_t i = 0; i < numSeg; ++i)
        {
            for(size_t j = 0; j < numSeg; ++j)
            {
                if(i != 0)
                {
                    double value = viewFactors(frontSideMeshIndex[i - 1], backSideMeshIndex[j]) * Tb + viewFactors(backSideMeshIndex[i], backSideMeshIndex[j]) * Rb;
                    if(i == j)
                    {
                        value -= 1;
                    }
                    energy(j + numSeg, i + numSeg) = value;
                }
                else
                {
                    if(i != j)
                    {
                        energy(j + numSeg, i + numSeg) = 0;
                    }
                    else
                    {
                        energy(j + numSeg, i + numSeg) = -1;
                    }
                }
            }
        }
        return energy;
    }

    std::vector<double>
      CVenetianCellEnergy::diffuseVector(const SlatSegmentsMesh & slats,
                                         FenestrationCommon::SquareMatrix && viewFactors)
    {
        std::vector<double> B(2 * slats.numberOfSegments);
        for(size_t i = 0; i < slats.numberOfSegments; ++i)
        {
            B[i] = -viewFactors(slats.backSideMeshIndex[0], slats.frontSideMeshIndex[i]);
            B[i + slats.numberOfSegments] =
              -viewFactors(slats.backSideMeshIndex[0], slats.backSideMeshIndex[i]);
        }

        return B;
    }

    std::vector<CVenetianCellEnergy::BeamSegmentView>
      CVenetianCellEnergy::beamVector(const CBeamDirection & t_Direction, const Side t_Side)
    {
        size_t numSeg{m_SlatSegments.numberOfSegments};

        const auto profileAngle{t_Side == Side::Front ? t_Direction.profileAngle()
                                                      : -t_Direction.profileAngle()};

        std::vector<BeamSegmentView> B(2 * numSeg);
        size_t index = 0;
        for(const Viewer::BeamViewFactor & aVF : m_Cell->beamViewFactors(profileAngle, t_Side))
        {
            if(aVF.enclosureIndex == 0)
            {   // Top
                index = aVF.segmentIndex + 1;
            }
            else if(aVF.enclosureIndex == 1)
            {   // Bottom
                index = numSeg + 1 + aVF.segmentIndex;
            }
            else
            {
                assert("Incorrect value for enclosure. Cannot have more than three enclosures.");
            }
            B[index].viewFactor = aVF.value;
            B[index].percentViewed = aVF.percentHit;
        }

        const std::map<Side, size_t> sideIndex{{Side::Front, numSeg}, {Side::Back, 0}};

        B[sideIndex.at(t_Side)].viewFactor = m_Cell->T_dir_dir(t_Side, t_Direction);

        return B;
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
}