#ifndef TARCOGSYSTEM_H
#define TARCOGSYSTEM_H

#include <memory>
#include <vector>
#include <map>

namespace Tarcog {

	class CIGU;
	class CEnvironment;
	class CSingleSystem;
	class CIGUSolidLayer;
	enum class Environment;

	enum class System { Uvalue, SHGC };

	class CSystem {
	public:
		CSystem( CIGU & t_IGU,
		         std::shared_ptr< CEnvironment > const& t_Indoor,
		         std::shared_ptr< CEnvironment > const& t_Outdoor );

		std::shared_ptr< std::vector< double > > getTemperatures( System t_System ) const;
		std::shared_ptr< std::vector< double > > getRadiosities( System t_System ) const;

		std::shared_ptr< std::vector< double > > getMaxDeflections( System t_System ) const;
		std::shared_ptr< std::vector< double > > getMeanDeflections( System t_System ) const;

		std::vector< std::shared_ptr< CIGUSolidLayer > > getSolidLayers( System t_System ) const;

		double getHeatFlow( System t_System, Environment t_Environment ) const;
		double getUValue() const;
		double getSHGC( double t_TotSol ) const;
		size_t getNumberOfIterations( System t_System ) const;

	private:
		std::map< System, std::shared_ptr< CSingleSystem > > m_System;
	};

}

#endif
