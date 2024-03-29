#ifndef BASECELL_H
#define BASECELL_H

#include <memory>
#include <vector>

namespace FenestrationCommon
{
    enum class Side;
    class CSeries;

}   // namespace FenestrationCommon

namespace SingleLayerOptics
{
    class CMaterial;
    class ICellDescription;
    class CBeamDirection;

    // Handles optical layer "cell". Base behavior is to calculate specular (direct-direct)
    // component of a light beam. Inherit from this class when want to create new shading type.
    class CBaseCell
    {
    public:
        virtual ~CBaseCell() = default;
        CBaseCell();
        CBaseCell(const std::shared_ptr<CMaterial> & t_Material,
                  const std::shared_ptr<ICellDescription> & t_CellDescription,
                  double rotation = 0);

        virtual void setSourceData(FenestrationCommon::CSeries & t_SourceData);

        // Direct to direct component of transmitted ray
        // These dir_dir and dir_dir_band functions are returning only direct portion of the
        // incoming beam that goes directly through cell without interfering (bouncing off) with
        // material (Simon)
        virtual double T_dir_dir(FenestrationCommon::Side t_Side,
                                 const CBeamDirection & t_Direction);
        virtual double R_dir_dir(FenestrationCommon::Side t_Side,
                                 const CBeamDirection & t_Direction);

        virtual std::vector<double> T_dir_dir_band(FenestrationCommon::Side t_Side,
                                                   const CBeamDirection & t_Direction);

        virtual double T_dir_dir_at_wavelength(FenestrationCommon::Side t_Side,
                                               const CBeamDirection & t_Direction,
                                               size_t wavelengthIndex);

        virtual std::vector<double> R_dir_dir_band(FenestrationCommon::Side t_Side,
                                                   const CBeamDirection & t_Direction);

        virtual double R_dir_dir_at_wavelength(FenestrationCommon::Side t_Side,
                                               const CBeamDirection & t_Direction,
                                               size_t wavelengthIndex);

        std::vector<double> getBandWavelengths() const;
        virtual void setBandWavelengths(const std::vector<double> & wavelengths);
        int getBandIndex(double t_Wavelength) const;
        size_t getBandSize() const;

        double getMinLambda() const;
        double getMaxLambda() const;

        void Flipped(bool flipped) const;

        std::shared_ptr<CMaterial> getMaterial();

    protected:
        std::shared_ptr<CMaterial> m_Material;
        std::shared_ptr<ICellDescription> m_CellDescription;

        // This indicates cell rotation in phi angle
        double m_CellRotation;
    };
}   // namespace SingleLayerOptics

#endif
