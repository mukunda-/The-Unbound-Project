<?php

/** ---------------------------------------------------------------------------
 * Exception thrown from MySQLW::RunQuery
 */
class SQLException extends \Exception {
	public $code; // mysqli errno
	
	public function __construct( $errno, $error ) {	
		$code = $errno;
		parent::__construct( $error );
	}
}

?>