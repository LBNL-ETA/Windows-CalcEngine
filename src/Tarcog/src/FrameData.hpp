#pragma once

#include <functional>
#include <optional>
#include <variant>
#include <array>
#include <map>

namespace Tarcog::ISO15099
{
    //! Data structure for Insulating Glass Unit (IGU) properties.
    //! Contains thermal and physical characteristics of the glazing unit.
    struct IGUData
    {
        double UValue{0};      //! Thermal transmittance (U-value) of the IGU [W/(m²·K)]
        double Thickness{0};   //! Total thickness of the IGU [m]
    };

    //! Generic frame type will have edge of glass calculations based on coefficients
    using GenericFrame = std::array<double, 5>;

    using DividerEdgePoly = std::array<double, 4>;
    using DividerBodyPoly = std::array<double, 5>;

    //! GenericDivider calculations is used
    struct GenericDivider
    {
        DividerEdgePoly EdgePoly;
        DividerBodyPoly BodyPoly;
    };

    //! Data structure for window frame properties.
    //! Contains thermal and dimensional characteristics of a window frame.
    struct FrameData
    {
        double UValue{0};       //! Thermal transmittance of the frame [W/(m²·K)]
        double EdgeUValue{0};   //! Thermal transmittance at the edge of glass [W/(m²·K)]
        double ProjectedFrameDimension{0};   //! Projected width/dimension of the frame [m]
        double WettedLength{0};              //! Length of frame in contact with other materials [m]
        double Absorptance{0.3};             //! Solar absorptance of the frame (0-1)
        std::optional<IGUData> iguData;   //! Optional data for the IGU associated with this frame
        std::variant<std::monostate, GenericFrame, GenericDivider> Class;
    };

    double dividerUValue(const DividerBodyPoly & poly, double uCenter, double gap);

    //! Calculates frame edge uValue based on FrameData type
    double frameEdgeUValue(const GenericFrame & c, double uCenter, double gap);
    double dividerEdgeUValue(const DividerEdgePoly & poly, double uCenter, double gap);

    //! Each frame can have frame attached to either left or right side of it.
    enum class FrameSide : uint8_t
    {
        Left,
        Right
    };

    //! When building window, frame will be inserted differently which is based on what the
    //! frame type is. This is important factor when calculating frame area as well as edge of
    //! glass area since interior frames will take less space
    enum class FrameType : uint8_t
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
        std::map<FrameSide, std::optional<std::reference_wrapper<const Frame>>> frame;

        double dividerArea{0};
        size_t numberOfDividers{0U};
    };
}   // namespace Tarcog::ISO15099