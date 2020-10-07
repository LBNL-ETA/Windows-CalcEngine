#ifndef DIRECTIONALDIFFUSEBSDFLAYER_H
#define DIRECTIONALDIFFUSEBSDFLAYER_H

#include <memory>

#include "BSDFLayer.hpp"

namespace SingleLayerOptics {

	class CDirectionalDiffuseCell;

	// All outgoing directions are calculated
	class CDirectionalDiffuseBSDFLayer : public CBSDFLayer {
	public:
		CDirectionalDiffuseBSDFLayer( const std::shared_ptr< CDirectionalDiffuseCell > & t_Cell,
									  const CBSDFHemisphere & t_Hemisphere );

	protected:
		std::shared_ptr< CDirectionalDiffuseCell > cellAsDirectionalDiffuse() const;
		void calcDiffuseDistribution( const FenestrationCommon::Side aSide,
		                              const CBeamDirection& incomingDirection,
		                              const size_t incomingDirectionIndex );
		void calcDiffuseDistribution_wv( const FenestrationCommon::Side aSide,
		                                 const CBeamDirection& incomingDirection,
		                                 const size_t incomingDirectionIndex );

	};

}

#endif
