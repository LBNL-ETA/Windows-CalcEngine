#ifndef TARIGU_H
#define TARIGU_H

#include <memory>
#include <vector>

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

            std::vector<std::shared_ptr<CIGUSolidLayer>> getSolidLayers() const;
            std::vector<std::shared_ptr<CIGUGapLayer>> getGapLayers() const;
            std::vector<std::shared_ptr<CBaseLayer>> getLayers() const;

            void setTilt(double t_Tilt);
            void setWidth(double t_Width);
            void setHeight(double t_Height);

            void setSolarRadiation(double t_SolarRadiation) const;

            std::shared_ptr<CBaseLayer> getEnvironment(Environment t_Environment) const;

            std::vector<double> getState() const;
            void setState(const std::vector<double> & t_State) const;

            std::vector<double> getTemperatures() const;
            std::vector<double> getRadiosities() const;
            std::vector<double> getMaxDeflections() const;
            std::vector<double> getMeanDeflections() const;

            double getTilt() const;
            double getWidth() const;
            double getHeight() const;
            double getThickness() const;

            size_t getNumOfLayers() const;

            double getVentilationFlow(Environment t_Environment) const;

            void setInitialGuess(const std::vector<double> & t_Guess) const;

            void setDeflectionProperties(double t_Tini, double t_Pini);
            void setDeflectionProperties(const std::vector<double> & t_MeasuredDeflections);

        private:
            // Replces layer in existing construction and keeps correct connections in linked list
            void replaceLayer(const std::shared_ptr<CBaseIGULayer> & t_Original,
                              const std::shared_ptr<CBaseIGULayer> & t_Replacement);

            // Check if layer needs to be decorated with another object
            void checkForLayerUpgrades(const std::shared_ptr<CBaseLayer> & t_Layer);

            std::vector<std::shared_ptr<CBaseLayer>> m_Layers;

            double m_Width;    // meters
            double m_Height;   // meters
            double m_Tilt;     // degrees

            // Routines to calculate deflection coefficients
            double Ldmean() const;
            double Ldmax() const;
        };

    }   // namespace ISO15099

}   // namespace Tarcog


#endif
