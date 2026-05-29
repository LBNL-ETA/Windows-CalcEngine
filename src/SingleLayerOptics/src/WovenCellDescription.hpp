#pragma once

namespace FenestrationCommon
{
    enum class Side;
}

namespace SingleLayerOptics
{
    class CBeamDirection;

    class CWovenCellDescription
    {
    public:
        CWovenCellDescription(double t_Diameter, double t_Spacing);

        double gamma() const;

        double Beam_dir_dir(FenestrationCommon::Side t_Side, const CBeamDirection & t_Direction);

        [[nodiscard]] double diameter() const;
        [[nodiscard]] double spacing() const;

    private:
        double Tx(const CBeamDirection & t_Direction);
        double Ty(const CBeamDirection & t_Direction);

        double cutOffAngle() const;

        double m_Diameter;
        double m_Spacing;
    };

}   // namespace SingleLayerOptics
