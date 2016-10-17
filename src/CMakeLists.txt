add_subdirectory( Common )
set( LINK_TO_Common FenestrationCommon )

add_subdirectory( Gases )
set( LINK_TO_Gases Gases )

add_subdirectory( Tarcog )

add_subdirectory( SpectralAveraging )
set( LINK_TO_SpectralAveraging SpectralAveraging )

add_subdirectory( Viewer )
set( LINK_TO_Viewer Viewer )

add_subdirectory( SingleLayerOptics )
set( LINK_TO_SingleLayerOptics SingleLayerOptics )

add_subdirectory( MultiLayerOptics )