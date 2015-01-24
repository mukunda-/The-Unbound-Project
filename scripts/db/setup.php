<?php //=====================  The Unbound Project  =========================//
     //                                                                     //
    //===== Copyright © 2015, Mukunda Johnson, All rights reserved. =======//

namespace Unbound;
chdir("..");
require_once "env.php";

//-----------------------------------------------------------------------------

function SetupDatabases() {

	echo "setting up databases...\n";
	$sql = \SQLW::Get();
	$sql->RunQuery( "
		CREATE TABLE IF NOT EXISTS Accounts (
			accountid INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
			username VARCHAR(255) COMMENT 'Username.',
			password VARCHAR(255) COMMENT 'Hashed password.',
			userhash INT UNSIGNED COMMENT 'Username hash for for index.',
			INDEX USING BTREE (userhash)
		)
		ENGINE InnoDB
	");
	
	echo "done.\n";	
}

SetupDatabases();

?>