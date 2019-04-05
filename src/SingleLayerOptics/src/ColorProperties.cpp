#include <cmath>
#include <vector>

#include "ColorProperties.hpp"
#include "IScatteringLayer.hpp"
#include <WCECommon.hpp>

namespace SingleLayerOptics
{
    Trichromatic::Trichromatic(double X, double Y, double Z) : X(X), Y(Y), Z(Z)
    {}

    aRGB::aRGB(int R, int G, int B) : R(R), G(G), B(B)
    {}

    CIE_LAB::CIE_LAB(double L, double A, double B) : L(L), a(A), b(B)
    {}

}   // namespace SingleLayerOptics
