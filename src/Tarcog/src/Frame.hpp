#pragma once

#include <optional>
#include <WCECommon.hpp>


namespace Tarcog::ISO15099
{
    struct FrameData
    {
        double UValue{0};
        double EdgeUValue{0};
        double ProjectedFrameDimension{0};
        double WettedLength{0};
        double Absorptance{0.3};
    };

    [[nodiscard]] FrameData splitFrameWidth(const FrameData& frame);
    [[nodiscard]] double shgc(const FrameData& frame, double hc);

    enum class FrameSide
    {
        Left,
        Right
    };

    //! When building window, frame will be inserted differently which is based on what the
    //! frame type is. This is important factor when calculating frame area as well as edge of
    //! glass area since interior frames will take less space
    enum class FrameType
    {
        Interior,
        Exterior
    };

    class Frame
    {
    public:
        Frame(double length,
              FrameType frameType = FrameType::Exterior,
              FrameData frameData = FrameData());

        [[nodiscard]] double projectedArea() const;
        [[nodiscard]] double wettedArea() const;
        void setFrameData(FrameData frameData);
        [[nodiscard]] const FrameData & frameData() const;
        [[nodiscard]] double edgeOfGlassArea() const;
        [[nodiscard]] double projectedFrameDimension() const;

        //! Divider area that will be subtracted from the frame
        void assignDividerArea(double area, size_t nDividers);

        double m_Length;
        FrameType m_FrameType;
        FrameData m_FrameData;

        //! Keeping frame information on both sides of the frame. This is needed for geometry
        //! calculations. Optional must be used or infinite loop will be created withing Frame
        //! constructor (Frame calling itself over and over again)
        std::map<FrameSide, std::optional<Frame>> m_Frame;

        double m_DividerArea{0};
        size_t m_NumberOfDividers{0u};
    };
}   // namespace Tarcog::ISO15099
