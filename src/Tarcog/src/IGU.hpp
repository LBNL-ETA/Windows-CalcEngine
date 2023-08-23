#pragma once

#include <memory>
#include <vector>

#include "DeflectionFromCurves.hpp"

namespace Tarcog
{
    namespace ISO15099
    {
        enum class Environment;
        class CBaseIGULayer;
        class CIGUSolidLayer;
        class CIGUGapLayer;
        class CBaseLayer;
        class CSurface;


        class CIGU
        {
        public:
            CIGU(double t_Width = 1, double t_Height = 1, double t_Tilt = 90);
            CIGU(CIGU const & t_IGU);
            CIGU & operator=(CIGU const & t_IGU);
            ~CIGU();

            void addLayer(const std::shared_ptr<CBaseLayer> & t_Layer);
            void addLayers(const std::initializer_list<std::shared_ptr<CBaseIGULayer>> & layers);

            void setAbsorptances(const std::vector<double> & absorptances, double solarRadiation);

            [[nodiscard]] std::vector<std::shared_ptr<CIGUSolidLayer>> getSolidLayers() const;
            [[nodiscard]] std::vector<std::shared_ptr<CIGUGapLayer>> getGapLayers() const;
            [[nodiscard]] std::vector<std::shared_ptr<CBaseLayer>> getLayers() const;

            void setTilt(double t_Tilt);
            void setWidth(double t_Width);
            void setHeight(double t_Height);

            void setSolarRadiation(double t_SolarRadiation) const;

            [[nodiscard]] std::shared_ptr<CBaseLayer>
              getEnvironment(Environment t_Environment) const;

            [[nodiscard]] std::vector<double> getState() const;
            void setState(const std::vector<double> & t_State) const;

            [[nodiscard]] std::vector<double> getTemperatures() const;
            [[nodiscard]] std::vector<double> getRadiosities() const;

            [[nodiscard]] std::vector<double> getMaxLayerDeflections() const;
            [[nodiscard]] std::vector<double> getMeanLayerDeflections() const;
            [[nodiscard]] std::vector<double> getMaxGapDeflections() const;
            [[nodiscard]] std::vector<double> getMeanGapDeflections() const;

            [[nodiscard]] std::vector<double> getGapPressures() const;

            //! Function to return pressure difference on each of the layers when using deflection
            //! model
            [[nodiscard]] std::vector<double> getPanesLoad();

            [[nodiscard]] double getTilt() const;
            [[nodiscard]] double getWidth() const;
            [[nodiscard]] double getHeight() const;
            [[nodiscard]] double getThickness() const;

            [[nodiscard]] size_t getNumOfLayers() const;

            [[nodiscard]] double getVentilationFlow(Environment t_Environment) const;

            void setInitialGuess(const std::vector<double> & t_Guess) const;

            void setDeflectionProperties(double t_Tini,
                                         double t_Pini,
                                         double t_InsidePressure = 101325,
                                         double t_OutsidePressure = 101325);

            void setDeflectionProperties(const std::vector<double> & t_MeasuredDeflections);

            void setAppliedLoad(std::vector<double> t_AppliedLoad);

            void clearDeflection();

            //! Function that will update layers deflection states based on new temperature data
            void updateDeflectionState();

            void precalculateLayerStates();

        private:
            // Replaces layer in existing construction and keeps correct connections in linked list
            void replaceLayer(const std::shared_ptr<CBaseIGULayer> & t_Original,
                              const std::shared_ptr<CBaseIGULayer> & t_Replacement);

            // Check if layer needs to be decorated with another object
            void checkForLayerUpgrades(const std::shared_ptr<CBaseLayer> & t_Layer);

            [[nodiscard]] double
              calculateDeflectionNumerator(const std::vector<double> & t_MeasuredDeflections) const;
            [[nodiscard]] double calculateDeflectionDenominator() const;
            [[nodiscard]] std::vector<double>
              calculateLDefMax(const std::vector<double> & t_MeasuredDeflections) const;

            std::vector<std::shared_ptr<CBaseLayer>> m_Layers;

            double m_Width;    // meters
            double m_Height;   // meters
            double m_Tilt;     // degrees

            // Reset deflection state of all surfaces back to zero.
            void resetSurfaceDeflections();

            // Routines to calculate deflection coefficients
            [[nodiscard]] double Ldmean() const;
            [[nodiscard]] double Ldmax() const;

            //! This is by default set to nullptr since deflection is not turn on by default.
            //! Setting deflection properties will enable deflection calculations automatically.
            std::optional<Deflection::DeflectionE1300> m_DeflectionFromE1300Curves{std::nullopt};

            //! It is possible that user can set applied load before setting initial parameters for
            //! the deflection in which case applied load will not be set automatically. This is
            //! intermediate variable that keeps applied load so it can be applied later.
            std::vector<double> m_DeflectionAppliedLoad;
        };

    }   // namespace ISO15099

}   // namespace Tarcog