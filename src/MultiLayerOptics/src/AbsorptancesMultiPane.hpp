#ifndef AbsorptancesMultiPane_H
#define AbsorptancesMultiPane_H

#include <memory>
#include <vector>
#include <WCECommon.hpp>

namespace MultiLayerOptics {

	//! \brief Calculate absorptances of multiplane layers for simple case (single incident angle)
	class CAbsorptancesMultiPane {
	public:
		CAbsorptancesMultiPane(const FenestrationCommon::CSeries &t_T,
                               const FenestrationCommon::CSeries &t_Rf,
                               const FenestrationCommon::CSeries &t_Rb );

		void addLayer(const FenestrationCommon::CSeries &t_T,
                      const FenestrationCommon::CSeries &t_Rf,
                      const FenestrationCommon::CSeries &t_Rb );

		FenestrationCommon::CSeries Abs(size_t const Index );
		size_t numOfLayers();

	private:
		void calculateState();

		std::shared_ptr< FenestrationCommon::CSeries > rCoeffs(
			const FenestrationCommon::CSeries& t_T,
			const FenestrationCommon::CSeries& t_Rf,
			const FenestrationCommon::CSeries& t_Rb,
			const FenestrationCommon::CSeries& t_RCoeffs );

		std::shared_ptr< FenestrationCommon::CSeries > tCoeffs(
			const FenestrationCommon::CSeries& t_T,
			const FenestrationCommon::CSeries& t_Rb,
			const FenestrationCommon::CSeries& t_RCoeffs );

		std::vector< FenestrationCommon::CSeries > m_T;
		std::vector< FenestrationCommon::CSeries > m_Rf;
		std::vector< FenestrationCommon::CSeries > m_Rb;
		std::vector< FenestrationCommon::CSeries > m_Abs;

		std::vector< std::shared_ptr< FenestrationCommon::CSeries > > m_rCoeffs;
		std::vector< std::shared_ptr< FenestrationCommon::CSeries > > m_tCoeffs;

		bool m_StateCalculated;
	};
}

#endif
