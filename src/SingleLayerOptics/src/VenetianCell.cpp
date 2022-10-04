#include <cassert>
#include <stdexcept>

#include "VenetianCell.hpp"
#include "VenetianCellDescription.hpp"
#include "BeamDirection.hpp"
#include "MaterialDescription.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    ////////////////////////////////////////////////////////////////////////////////////////////
    //  CVenetianBase
    ////////////////////////////////////////////////////////////////////////////////////////////

    CVenetianBase::CVenetianBase(const std::shared_ptr<CMaterial> & t_MaterialProperties,
                                 const std::shared_ptr<ICellDescription> & t_Cell,
                                 double rotation) :
        CUniformDiffuseCell(t_MaterialProperties, t_Cell, rotation),
        CDirectionalDiffuseCell(t_MaterialProperties, t_Cell, rotation)
    {}

    std::shared_ptr<CVenetianCellDescription> CVenetianBase::getCellAsVenetian() const
    {
        if(std::dynamic_pointer_cast<CVenetianCellDescription>(m_CellDescription) == nullptr)
        {
            assert("Incorrectly assigned cell description.");
        }

        std::shared_ptr<CVenetianCellDescription> aCell =
          std::dynamic_pointer_cast<CVenetianCellDescription>(m_CellDescription);

        return aCell;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    //  CVenetianCellEnergy
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
          slatIrradiances(t_Direction, m_SlatSegments, m_SlatSegments.slatsEnergy);

        // Total energy accounts for direct to direct component. That needs to be subtracted since
        // only direct to diffuse is of interest
        return irradiance[m_SlatSegments.numberOfSegments].E_f - T_dir_dir(t_Direction);
    }

    double CVenetianCellEnergy::R_dir_dif(const CBeamDirection & t_Direction)
    {
        const auto irradiance{
          slatIrradiances(t_Direction, m_SlatSegments, m_SlatSegments.slatsEnergy)};

        return irradiance[0].E_b;
    }

    double CVenetianCellEnergy::T_dir_dir(const CBeamDirection & t_IncomingDirection,
                                          const CBeamDirection & t_OutgoingDirection)
    {
        const auto radiance{
          slatRadiances(t_IncomingDirection, m_SlatSegments, m_SlatSegments.slatsEnergy)};

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
          slatRadiances(t_IncomingDirection, m_SlatSegments, m_SlatSegments.slatsEnergy);

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

        CLinearSolver aSolver;
        std::vector<double> aSolution = aSolver.solveSystem(m_SlatSegments.slatsEnergy, B);

        return aSolution[numSeg - 1];
    }

    double CVenetianCellEnergy::R_dif_dif()
    {
        auto B{diffuseVector(m_SlatSegments, m_Cell->viewFactors())};

        CLinearSolver aSolver;
        std::vector<double> aSolution = aSolver.solveSystem(m_SlatSegments.slatsEnergy, B);

        return aSolution[m_SlatSegments.numberOfSegments];
    }

    std::vector<SegmentIrradiance>
      CVenetianCellEnergy::slatIrradiances(const CBeamDirection & t_IncomingDirection,
                                           const SlatSegments & slats,
                                           const FenestrationCommon::SquareMatrix & energy)
    {
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
                index = slats.f[i];
            }
            else
            {
                index = slats.b[i - numSeg];
            }
            B.push_back(-BVF[index].viewFactor);
        }

        CLinearSolver aSolver;
        const auto aSolution{aSolver.solveSystem(energy, B)};

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

        return aIrradiances;
    }

    std::vector<double>
      CVenetianCellEnergy::slatRadiances(const CBeamDirection & t_IncomingDirection,
                                         const SlatSegments & slats,
                                         const FenestrationCommon::SquareMatrix & energy)
    {
        const auto irradiance{slatIrradiances(t_IncomingDirection, slats, energy)};
        size_t numSlats = irradiance.size();
        std::vector<double> aRadiances(2 * numSlats - 2);
        for(size_t i = 0; i < numSlats; ++i)
        {
            if(i == 0)
            {
                aRadiances[slats.b[i]] = 1;
            }
            else if(i == numSlats - 1)
            {
                aRadiances[slats.f[i - 1]] = irradiance[i].E_f;
            }
            else
            {
                aRadiances[slats.b[i]] = m_Tf * irradiance[i].E_f + m_Rb * irradiance[i].E_b;
                aRadiances[slats.f[i - 1]] = m_Tb * irradiance[i].E_b + m_Rf * irradiance[i].E_f;
            }
        }

        return aRadiances;
    }

    FenestrationCommon::SquareMatrix SlatSegments::formEnergyMatrix(
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
                    double value = viewFactors(b[i + 1], f[j]) * Tf + viewFactors(f[i], f[j]) * Rf;
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
                      viewFactors(b[i + 1], b[j]) * Tf + viewFactors(f[i], b[j]) * Rf;
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
                      viewFactors(f[i - 1], f[j]) * Tb + viewFactors(b[i], f[j]) * Rb;
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
                    double value = viewFactors(f[i - 1], b[j]) * Tb + viewFactors(b[i], b[j]) * Rb;
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
      CVenetianCellEnergy::diffuseVector(const SlatSegments & slats,
                                         FenestrationCommon::SquareMatrix && viewFactors)
    {
        size_t numSeg{slats.numberOfSegments};

        std::vector<double> B(2 * numSeg);
        for(size_t i = 0; i < numSeg; ++i)
        {
            B[i] = -viewFactors(slats.b[0], slats.f[i]);
            B[i + numSeg] = -viewFactors(slats.b[0], slats.b[i]);
        }

        return B;
    }

    std::vector<CVenetianCellEnergy::BeamSegmentView>
      CVenetianCellEnergy::beamVector(const CBeamDirection & t_Direction, const Side t_Side)
    {
        size_t numSeg{m_SlatSegments.numberOfSegments};

        const auto profileAngle{t_Side == Side::Front ? t_Direction.profileAngle()
                                                      : -t_Direction.profileAngle()};

        std::vector<Viewer::BeamViewFactor> beamVF = m_Cell->beamViewFactors(profileAngle, t_Side);

        std::vector<BeamSegmentView> B(2 * numSeg);
        size_t index = 0;
        for(Viewer::BeamViewFactor & aVF : beamVF)
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

        std::map<Side, size_t> sideIndex{{Side::Front, numSeg}, {Side::Back, 0}};

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

    ////////////////////////////////////////////////////////////////////////////////////////////
    //  CVenetianCell
    ////////////////////////////////////////////////////////////////////////////////////////////
    CVenetianCell::CVenetianCell(const std::shared_ptr<CMaterial> & t_MaterialProperties,
                                 const std::shared_ptr<ICellDescription> & t_Cell,
                                 const double rotation) :
        CBaseCell(t_MaterialProperties, t_Cell, rotation),
        CVenetianBase(t_MaterialProperties, t_Cell, rotation),
        m_BackwardFlowCellDescription(getCellAsVenetian()->getBackwardFlowCell())
    {
        assert(t_Cell != nullptr);
        assert(t_MaterialProperties != nullptr);

        generateVenetianEnergy();
    }

    void CVenetianCell::generateVenetianEnergy()
    {
        const auto & venetianForwardGeometry{getCellAsVenetian()};
        m_Energy =
          CVenetianEnergy(*m_Material, venetianForwardGeometry, m_BackwardFlowCellDescription);
        // Create energy states for entire material band
        m_EnergiesBand.clear();
        std::vector<RMaterialProperties> aMat = m_Material->getBandProperties();

        if(!aMat.empty())
        {
            size_t size = m_Material->getBandSize();
            for(size_t i = 0; i < size; ++i)
            {
                double Tf = aMat[i].getProperty(Property::T, Side::Front);
                double Tb = aMat[i].getProperty(Property::T, Side::Back);
                double Rf = aMat[i].getProperty(Property::R, Side::Front);
                double Rb = aMat[i].getProperty(Property::R, Side::Back);

                CVenetianEnergy aEnergy = CVenetianEnergy(
                  Tf, Tb, Rf, Rb, venetianForwardGeometry, m_BackwardFlowCellDescription);
                m_EnergiesBand.push_back(aEnergy);
            }
        }
    }

    void CVenetianCell::setSourceData(CSeries & t_SourceData)
    {
        CBaseCell::setSourceData(t_SourceData);
        generateVenetianEnergy();
    }

    void CVenetianCell::setBandWavelengths(const std::vector<double> & wavelengths)
    {
        CBaseCell::setBandWavelengths(wavelengths);
        generateVenetianEnergy();
    }

    double CVenetianCell::T_dir_dir(const Side t_Side, const CBeamDirection & t_Direction)
    {
        if(m_CellRotation != 0)
        {
            return m_Energy.getCell(t_Side).T_dir_dir(t_Direction.rotate(m_CellRotation));
        }
        return m_Energy.getCell(t_Side).T_dir_dir(t_Direction);
    }

    std::vector<double> CVenetianCell::T_dir_dir_band(const Side t_Side,
                                                      const CBeamDirection & t_Direction)
    {
        size_t size = m_EnergiesBand.size();
        std::vector<double> aProperties;
        aProperties.reserve(size);
        for(size_t i = 0; i < size; ++i)
        {
            aProperties.push_back(T_dir_dir_at_wavelength(t_Side, t_Direction, i));
        }
        return aProperties;
    }

    double CVenetianCell::T_dir_dir_at_wavelength(FenestrationCommon::Side t_Side,
                                                  const CBeamDirection & t_Direction,
                                                  size_t wavelengthIndex)
    {
        if(m_CellRotation != 0)
        {
            return m_EnergiesBand[wavelengthIndex].getCell(t_Side).T_dir_dir(
              t_Direction.rotate(m_CellRotation));
        }

        return m_EnergiesBand[wavelengthIndex].getCell(t_Side).T_dir_dir(t_Direction);
    }

    double CVenetianCell::T_dir_dif(const Side t_Side, const CBeamDirection & t_Direction)
    {
        if(m_CellRotation != 0)
        {
            return m_Energy.getCell(t_Side).T_dir_dif(t_Direction.rotate(m_CellRotation));
        }
        return m_Energy.getCell(t_Side).T_dir_dif(t_Direction);
    }

    std::vector<double> CVenetianCell::T_dir_dif_band(const Side t_Side,
                                                      const CBeamDirection & t_Direction)
    {
        size_t size = m_EnergiesBand.size();
        std::vector<double> aProperties;
        aProperties.reserve(size);
        for(size_t i = 0; i < size; ++i)
        {
            aProperties.push_back(T_dir_dif_at_wavelength(t_Side, t_Direction, i));
        }
        return aProperties;
    }

    double CVenetianCell::T_dir_dif_at_wavelength(FenestrationCommon::Side t_Side,
                                                  const CBeamDirection & t_Direction,
                                                  size_t wavelengthIndex)
    {
        if(m_CellRotation != 0)
        {
            return m_EnergiesBand[wavelengthIndex].getCell(t_Side).T_dir_dif(
              t_Direction.rotate(m_CellRotation));
        }

        return m_EnergiesBand[wavelengthIndex].getCell(t_Side).T_dir_dif(t_Direction);
    }

    double CVenetianCell::R_dir_dif(const Side t_Side, const CBeamDirection & t_Direction)
    {
        if(m_CellRotation != 0)
        {
            return m_Energy.getCell(t_Side).R_dir_dif(t_Direction.rotate(m_CellRotation));
        }
        return m_Energy.getCell(t_Side).R_dir_dif(t_Direction);
    }

    std::vector<double> CVenetianCell::R_dir_dif_band(const Side t_Side,
                                                      const CBeamDirection & t_Direction)
    {
        size_t size = m_EnergiesBand.size();
        std::vector<double> aProperties;
        aProperties.reserve(size);
        for(size_t i = 0; i < size; ++i)
        {
            aProperties.push_back(R_dir_dif_at_wavelength(t_Side, t_Direction, i));
        }
        return aProperties;
    }

    double CVenetianCell::R_dir_dif_at_wavelength(FenestrationCommon::Side t_Side,
                                                  const CBeamDirection & t_Direction,
                                                  size_t wavelengthIndex)
    {
        if(m_CellRotation != 0)
        {
            return m_EnergiesBand[wavelengthIndex].getCell(t_Side).R_dir_dif(
              t_Direction.rotate(m_CellRotation));
        }

        return m_EnergiesBand[wavelengthIndex].getCell(t_Side).R_dir_dif(t_Direction);
    }

    double CVenetianCell::T_dir_dif(const Side t_Side,
                                    const CBeamDirection & t_IncomingDirection,
                                    const CBeamDirection & t_OutgoingDirection)
    {
        if(m_CellRotation != 0)
        {
            return m_Energy.getCell(t_Side).T_dir_dir(t_IncomingDirection.rotate(m_CellRotation),
                                                      t_OutgoingDirection.rotate(m_CellRotation));
        }
        return m_Energy.getCell(t_Side).T_dir_dir(t_IncomingDirection, t_OutgoingDirection);
    }

    std::vector<double> CVenetianCell::T_dir_dif_band(const Side t_Side,
                                                      const CBeamDirection & t_IncomingDirection,
                                                      const CBeamDirection & t_OutgoingDirection)
    {
        size_t size = m_EnergiesBand.size();
        std::vector<double> aProperties;
        aProperties.reserve(size);
        for(size_t i = 0; i < size; ++i)
        {
            aProperties.push_back(
              T_dir_dif_by_wavelength(t_Side, t_IncomingDirection, t_OutgoingDirection, i));
        }
        return aProperties;
    }

    double CVenetianCell::T_dir_dif_by_wavelength(const FenestrationCommon::Side t_Side,
                                                  const CBeamDirection & t_IncomingDirection,
                                                  const CBeamDirection & t_OutgoingDirection,
                                                  size_t wavelengthIndex)
    {
        if(m_CellRotation != 0)
        {
            return m_EnergiesBand[wavelengthIndex].getCell(t_Side).T_dir_dir(
              t_IncomingDirection.rotate(m_CellRotation),
              t_OutgoingDirection.rotate(m_CellRotation));
        }
        return m_EnergiesBand[wavelengthIndex].getCell(t_Side).T_dir_dir(t_IncomingDirection,
                                                                         t_OutgoingDirection);
    }

    double CVenetianCell::R_dir_dif(const Side t_Side,
                                    const CBeamDirection & t_IncomingDirection,
                                    const CBeamDirection & t_OutgoingDirection)
    {
        if(m_CellRotation != 0)
        {
            return m_Energy.getCell(t_Side).R_dir_dir(t_IncomingDirection.rotate(m_CellRotation),
                                                      t_OutgoingDirection.rotate(m_CellRotation));
        }
        return m_Energy.getCell(t_Side).R_dir_dir(t_IncomingDirection, t_OutgoingDirection);
    }

    std::vector<double> CVenetianCell::R_dir_dif_band(const Side t_Side,
                                                      const CBeamDirection & t_IncomingDirection,
                                                      const CBeamDirection & t_OutgoingDirection)
    {
        size_t size = m_EnergiesBand.size();
        std::vector<double> aProperties;
        aProperties.reserve(size);
        for(size_t i = 0; i < size; ++i)
        {
            aProperties.push_back(
              R_dir_dif_by_wavelength(t_Side, t_IncomingDirection, t_OutgoingDirection, i));
        }
        return aProperties;
    }

    double CVenetianCell::R_dir_dif_by_wavelength(const FenestrationCommon::Side t_Side,
                                                  const CBeamDirection & t_IncomingDirection,
                                                  const CBeamDirection & t_OutgoingDirection,
                                                  size_t wavelengthIndex)
    {
        if(m_CellRotation != 0)
        {
            return m_EnergiesBand[wavelengthIndex].getCell(t_Side).R_dir_dir(
              t_IncomingDirection.rotate(m_CellRotation),
              t_OutgoingDirection.rotate(m_CellRotation));
        }

        return m_EnergiesBand[wavelengthIndex].getCell(t_Side).R_dir_dir(t_IncomingDirection,
                                                                         t_OutgoingDirection);
    }

    double CVenetianCell::T_dif_dif(const Side t_Side)
    {
        return m_Energy.getCell(t_Side).T_dif_dif();
    }

    double CVenetianCell::R_dif_dif(const Side t_Side)
    {
        return m_Energy.getCell(t_Side).R_dif_dif();
    }

    SlatSegments::SlatSegments(
      CVenetianCellDescription & cell, double Tf, double Tb, double Rf, double Rb) :
        numberOfSegments(static_cast<size_t>(cell.numberOfSegments() / 2)),
        f(formFrontSegments(numberOfSegments)),
        b(formBackSegments(numberOfSegments)),
        slatsEnergy(formEnergyMatrix(cell.viewFactors(), Tf, Tb, Rf, Rb))
    {
    }

    std::vector<size_t> SlatSegments::formFrontSegments(const size_t numOfSegments)
    {
        std::vector<size_t> frontSegments;
        frontSegments.reserve(numOfSegments);
        for(size_t i = 0; i < numOfSegments; ++i)
        {
            frontSegments.push_back(2 * numOfSegments - 1 - i);
        }
        return frontSegments;
    }

    std::vector<size_t> SlatSegments::formBackSegments(const size_t numOfSegments)
    {
        std::vector<size_t> backSegments;
        backSegments.reserve(numOfSegments);
        for(size_t i = 0; i < numOfSegments; ++i)
        {
            backSegments.push_back(i);
        }
        return backSegments;
    }
}   // namespace SingleLayerOptics
