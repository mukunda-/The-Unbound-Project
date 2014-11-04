<?php
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
			userhash INT UNSIGNED,
			INDEX USING BTREE (userhash)
		)
		ENGINE InnoDB
	");
	
	echo "done.\n";	
}

SetupDatabases();

?>