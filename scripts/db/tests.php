<?php //=====================  The Unbound Project  =========================//
     //                                                                     //
    //===== Copyright © 2015, Mukunda Johnson, All rights reserved. =======//

namespace Unbound;
chdir("..");
require_once "env.php";
require_once "private/sql_test.php";

echo '<html><body>';
///////////////////////////////////////////////////////////////////////////////

function MakeButton( $action ) {
	echo "
		<form action='tests.php' method='POST'>
			<input type='submit' name='action' value='$action'>
		</form>
		";
}

MakeButton( 'setup' );
///////////////////////////////////////////////////////////////////////////////
if( !isset($_POST['action']) ) die();

if( $_POST['action'] == 'setup' ) {
	$db = \SQLW::Get( $SQL_TEST );
	$db->RunQuery( "
		CREATE TABLE IF NOT EXISTS test1 (
			col1 INTEGER,
			col2 VARCHAR(64),
			col3 FLOAT 
		) ENGINE InnoDB" );
		
	$db->RunQuery( "TRUNCATE TABLE test1" );
	
	$db->RunQuery( "
		INSERT INTO test1 (col1,col2,col3) 
		VALUES (1,'testes',1.125),
			   (2,'banana',9.875),
			   (3,'apple',0.84375),
			   (4,'carrot',0.5)
			   " );
	
	echo "OK.";
}

echo '</body></html>';
?>

</body>
</html>