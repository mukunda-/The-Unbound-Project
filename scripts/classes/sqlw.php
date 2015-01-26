<?php

// SQL wrapper with useful query function
// which throws an exception on failure
//
// also provides global instance of database link
//

/** ---------------------------------------------------------------------------
 * mysqli wrapper class
 */
class SQLW extends mysqli {

	// some useful errors
	const ER_DUP_KEY = 1022; // insert failure from duplicate key
	const ER_LOCK_WAIT_TIMEOUT = 1205; // deadlock thing
	const ER_LOCK_DEADLOCK = 1213; // deadlock thing
	
	private $info;

	private static $connections = [];
	
	/** -----------------------------------------------------------------------
	 * Construct with some special sauce.
	 */
	public function __construct( $info, $flags ) {
		$this->info = $info;
		
		parent::init();
		parent::real_connect( 
			$info['address'],  $info['username'], 
			$info['password'], $info['database'],
			null, null, $flags );
	}

	/** -----------------------------------------------------------------------
	 * Execute a query and throw an SQLException if it fails.
	 *
	 * @param string $query SQL query to execute.
	 * @return SQL result
	 */
	public function RunQuery( $query ) {
		$result = $this->query( $query );
		if( !$result ) {
			throw new SQLException( 
				$this->errno, "SQL Error: ". $this->error );
		}
		return $result;
	}
	
	/** -----------------------------------------------------------------------
	 * Try executing a function and retrying it if any "normal" errors occur.
	 * 
	 * @param function($db) $function Function to execute.
	 * @param int      $tries Max number of failures to allow.
	 * @return mixed   The return value of $function on success.
	 */
	public function DoTransaction( $function, $tries = 5 ) {
	
		for( ; $tries; $tries-- ) {
			try {
				
				return $function( $this );

			} catch( SQLException $e ) {
				if( $e->code == self::ER_LOCK_DEADLOCK ||
					$e->code == self::ER_LOCK_WAIT_TIMEOUT ) {
					
					// try again
					continue;
				}
				
				throw $e;
			}
		}
		
		throw new RuntimeException( "SQL deadlock occurred too many times." );
	}
	
	/** -----------------------------------------------------------------------
	 * Connect to a database or return an existing connection.
	 *
	 * @returns SQLW instance.
	 */
	public static function Get( $info = null ) { 
		
		if( $info === null ) {
			if( !isset( $SQLW_DEFAULT_INFO )) {
				throw new RuntimeException( 
					"Connection info not given, and default info is not set." );
			}
			$info = $SQLW_DEFAULT_INFO;
		}
		
		if( !isset(self::$connections[$info['name']]) ) {
			$db = new self( $info, MYSQLI_CLIENT_FOUND_ROWS );
				
			if( $db->connect_errno ) {
				$ex = new SQLException( 
					(int)$db->connect_errno, 
					"SQL Connection Error: ". (int)$db->connect_error );
				$db = null;
				throw $ex;
			}
			self::$connections[$info['name']] = $db;
		}
		
		return self::$connections[$info['name']]; 
	}
	
	/** -----------------------------------------------------------------------
	 * Close a database connection.
	 *
	 * Normally this is handled by the script termination.
	 */
	public function Close() {
		close();
		unset( self::$connections[$info['name']] );
	}
}

?>