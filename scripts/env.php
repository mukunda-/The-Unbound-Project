<?php

namespace Unbound;

header( "Content-Type: text/plain" );

spl_autoload_register( function( $class ) {
	$class = strtolower( $class );
	$class = str_replace( "\\", "/", $class );
	if( file_exists( "classes/$class.php" ) ) {
		include "classes/$class.php";
	}
	
});

spl_autoload_register( function( $class ) {
	$class = str_replace( "\\", "/", $class );
	if( file_exists( "libs/$class.php" ) ) {
		include "libs/$class.php";
	}
});

$docroot = str_replace( 
			"\\", "/", 
			substr( rtrim(dirname(__FILE__), '/\\'), 
			strlen($_SERVER["DOCUMENT_ROOT"]) ) 
			) . '/';
			
function GetDocumentRoot() {
	return $GLOBALS['docroot'];
}

?>