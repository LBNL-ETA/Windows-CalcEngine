#ifndef PERFORATEDCELLDESCRIPTION_H
#define PERFORATEDCELLDESCRIPTION_H

namespace FenestrationCommon
{
    enum class Side;
}

namespace SingleLayerOptics
{
    class CBeamDirection;

    class CCircularCellDescription
    {
    public:
        CCircularCellDescription(double t_x, double t_y, double t_Thickness, double t_Radius);

        double Beam_dir_dir(FenestrationCommon::Side t_Side, const CBeamDirection & t_Direction);

        [[nodiscard]] double xDimension() const;
        [[nodiscard]] double yDimension() const;
        [[nodiscard]] double thickness() const;
        [[nodiscard]] double radius() const;

    private:
        [[nodiscard]] double visibleAhole(const CBeamDirection & t_Direction) const;
        [[nodiscard]] double visibleAcell(const CBeamDirection & t_Direction) const;
        double m_x;
        double m_y;
        double m_Thickness;
        double m_Radius;
    };

    class CRectangularCellDescription
    {
    public:
        CRectangularCellDescription(
          double t_x, double t_y, double t_Thickness, double t_XHole, double t_YHole);

        double Beam_dir_dir(FenestrationCommon::Side t_Side, const CBeamDirection & t_Direction);

        [[nodiscard]] double xDimension() const;
        [[nodiscard]] double yDimension() const;
        [[nodiscard]] double thickness() const;
        [[nodiscard]] double xHole() const;
        [[nodiscard]] double yHole() const;

    private:
        double TransmittanceV(const CBeamDirection & t_Direction) const;
        double TransmittanceH(const CBeamDirection & t_Direction) const;

        double m_x;
        double m_y;
        double m_Thickness;
        double m_XHole;
        double m_YHole;
    };

}   // namespace SingleLayerOptics

#endif
