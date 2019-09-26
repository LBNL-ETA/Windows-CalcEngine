#ifndef TARCOGSINGLESYSTEM_H
#define TARCOGSINGLESYSTEM_H

#include <memory>
#include <map>
#include <vector>

#include "IGU.hpp"

namespace Tarcog
{
	namespace ISO15099 {
		enum class Environment;

		class CBaseIGULayer;

		class CIGUSolidLayer;

		class CIGUGapLayer;

		class CEnvironment;

		class CNonLinearSolver;

		class CSingleSystem {
		public:
			CSingleSystem( CIGU & t_IGU,
						   const std::shared_ptr< CEnvironment > & t_Indoor,
						   const std::shared_ptr< CEnvironment > & t_Outdoor );

			CSingleSystem( const CSingleSystem & t_SingleSystem );
			CSingleSystem & operator=( const CSingleSystem & t_SingleSystem );

			std::vector< std::shared_ptr< CIGUSolidLayer>> getSolidLayers() const;
			std::vector< std::shared_ptr< CIGUGapLayer>> getGapLayers() const;

			std::vector<double> getSolidEffectiveLayerConductivities() const;
			std::vector<double> getGapEffectiveLayerConductivities() const;

			std::vector< double> getTemperatures() const;
			std::vector< double> getRadiosities() const;

			std::vector< double> getMaxDeflections() const;
			std::vector< double> getMeanDeflections() const;

			std::shared_ptr< CSingleSystem > clone() const;

			double getHeatFlow( Environment t_Environment ) const;
			double getConvectiveHeatFlow( Environment t_Environment ) const;
			double getRadiationHeatFlow( Environment t_Environment ) const;
			double getHc( Environment t_Environment ) const;
			double getAirTemperature( Environment t_Environment ) const;

			// If interior layer have openings, this will return heat flow from airflow
			double getVentilationFlow( Environment t_Environment ) const;
			double getUValue() const;
			size_t getNumberOfIterations() const;
			double solutionTolarance() const;
			bool isToleranceAchieved() const;

			double EffectiveConductivity() const;

			// Set solution tolerance
			void setTolerance( double t_Tolerance ) const;
			// Set intial guess for solution.
			void setInitialGuess( const std::vector< double > & t_Temperatures ) const;

			void setSolarRadiation( double t_SolarRadiation );
			double getSolarRadiation() const;

			void solve() const;

			double thickness() const;

            void setAbsorptances(const std::vector<double> & absorptances);

		private:
			CIGU m_IGU;
			std::map< Environment, std::shared_ptr< CEnvironment>> m_Environment;
			std::shared_ptr< CNonLinearSolver > m_NonLinearSolver;
			void initializeStartValues();
		};

	}

}   // namespace Tarcog

#endif
