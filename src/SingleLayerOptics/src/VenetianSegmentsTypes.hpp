#pragma once

namespace SingleLayerOptics
{
    //! Keeps information about beam view factor and percentage view. Incoming beam value is
    //! normalized to one.
    //! @viewFactor Fraction of the incoming beam that slat is being hit with
    //! @percentViewed Incoming beam will not always fully hit the slat. This is the percentage
    //! of the slat that actually is being hit by the beam.
    struct BeamSegmentView
    {
        BeamSegmentView() : viewFactor(0), percentViewed(0)
        {}
        double viewFactor;
        double percentViewed;
    };
}   // namespace SingleLayerOptics
