#pragma once

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

    struct Frame
    {
        double length{0};
        FrameType frameType{FrameType::Exterior};
        FrameData frameData{};

        //! Keeping frame information on both sides of the frame. This is needed for geometry
        //! calculations. Optional must be used or infinite loop will be created withing Frame
        //! constructor (Frame calling itself over and over again)
        std::map<FrameSide, std::optional<Frame>> frame;

        double dividerArea{0};
        size_t numberOfDividers{0u};
    };
}   // namespace Tarcog::ISO15099