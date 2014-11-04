<?php

/**
 * Contains SQL connection credentials.
 */
final class SQL_Login {

	// address of SQL server, eg 'yoursite.com';
	public static $address;
	
	// username for SQL database eg 'you'
	public static $username;
	
	// password for SQL login eg 'roawm(#*)H93wmth9vu0e49a3c'
	public static $password;
	
	// name of SQL database eg 'yoursite_store';
	public static $database;

}

// private file for filling in the fields.
include 'private/sql_login_init.php';

?>