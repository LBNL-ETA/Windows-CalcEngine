#ifndef DIRECTIONALDIFFUSECELL_H
#define DIRECTIONALDIFFUSECELL_H

#include <memory>
#include <vector>

#include "BaseCell.hpp"

namespace FenestrationCommon {

	enum class Side;

}

namespace SingleLayerOptics {

	class ICellDescription;
	class CBeamDirection;
	class CMaterial;

	class CDirectionalDiffuseCell : public virtual CBaseCell {
	public:
		CDirectionalDiffuseCell( const std::shared_ptr< CMaterial >& t_MaterialProperties,
		                         const std::shared_ptr< ICellDescription >& t_Cell );

		virtual double T_dir_dir( const FenestrationCommon::Side t_Side,
		                          const CBeamDirection& t_IncomingDirection, const CBeamDirection& t_OutgoingDirection ) = 0;

		virtual double R_dir_dir( const FenestrationCommon::Side t_Side,
		                          const CBeamDirection& t_IncomingDirection,
		                          const CBeamDirection& t_OutgoingDirection ) = 0;

		virtual std::shared_ptr< std::vector< double > > T_dir_dir_band( const FenestrationCommon::Side t_Side,
		                                                                 const CBeamDirection& t_IncomingDirection,
		                                                                 const CBeamDirection& t_OutgoingDirection ) = 0;

		virtual std::shared_ptr< std::vector< double > > R_dir_dir_band( const FenestrationCommon::Side t_Side,
		                                                                 const CBeamDirection& t_IncomingDirection,
		                                                                 const CBeamDirection& t_OutgoingDirection ) = 0;

	};

}

#endif
