#pragma once

namespace Tarcog
{
    namespace ISO15099
    {
        //! Geometry type will be used as part of vision area and it will be important to calculate
        //! frame and edge areas
        enum class FrameGeometryType
        {
            Square,
            Trapezoid,
            HalfTrapezoid
        };

        struct FrameData
        {
            FrameData(double uValue,
                      double edgeUValue,
                      double projectedFrameDimension,
                      double wettedLength,
                      double absorptance);
            double UValue;
            double EdgeUValue;
            double ProjectedFrameDimension;
            double WettedLength;
            double Absorptance;
        };

        class Frame
        {
        public:
            Frame() = delete;
            Frame(double length, FrameGeometryType frameGeometryType, FrameData frameData);

            [[nodiscard]] double area() const;

        private:
            double m_Length;
            FrameGeometryType m_GeometryType;
            FrameData m_FrameData;
        };
    }   // namespace ISO15099
}   // namespace Tarcog
