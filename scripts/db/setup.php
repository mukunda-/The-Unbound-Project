<?php //=====================  The Unbound Project  =========================//
     //                                                                     //
    //===== Copyright © 2015, Mukunda Johnson, All rights reserved. =======//

namespace Unbound;
chdir('..');
require_once 'env.php';
require_once 'private/sql_main.php';

$SQLW_DEFAULT_INFO = $SQL_UNBOUND_MAIN;

//-----------------------------------------------------------------------------

function DropDatabases() {
	echo "dropping databases...\n";
	$sql = \SQLW::Get();
	$sql->RunQuery( "
		DROP TABLE IF EXISTS Accounts
	");
	
}

//-----------------------------------------------------------------------------

function SetupDatabases() {

	echo "setting up databases...\n";
	$sql = \SQLW::Get();
	$sql->RunQuery( "
		CREATE TABLE IF NOT EXISTS Accounts (
			accountid INT UNSIGNED  AUTO_INCREMENT PRIMARY KEY,
			
			username  VARCHAR(255)  COMMENT 'Username.',
			password  VARCHAR(255)  COMMENT 'Hashed password.',
			userhash  INT UNSIGNED  COMMENT 'Username hash for for index.',
			token     INT UNSIGNED  COMMENT 'Authentication Token.',
			ip        VARBINARY(16) COMMENT 'IP address associated with token.',
			INDEX USING BTREE (userhash)
		)
		ENGINE InnoDB
	");
	
}

DropDatabases();
SetupDatabases();
echo "done.\n";	

?>