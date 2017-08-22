#include "PerforatedCell.hpp"
#include "CellDescription.hpp"
#include "MaterialDescription.hpp"
#include "WCECommon.hpp"
#include "BeamDirection.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace SingleLayerOptics {

	////////////////////////////////////////////////////////////////////////////////////////////
	//  CPerforatedCell
	////////////////////////////////////////////////////////////////////////////////////////////
	CPerforatedCell::CPerforatedCell( const shared_ptr< CMaterial >& t_MaterialProperties,
	                                  const shared_ptr< ICellDescription >& t_Cell ) :
		CBaseCell( t_MaterialProperties, t_Cell ), CUniformDiffuseCell( t_MaterialProperties, t_Cell ) {

	}

}
