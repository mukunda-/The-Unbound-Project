<?php //=====================  The Unbound Project  =========================//
     //                                                                     //
    //===== Copyright © 2015, Mukunda Johnson, All rights reserved. =======//

namespace Unbound;
chdir("..");
require_once "env.php";

echo '<html><body>';
///////////////////////////////////////////////////////////////////////////////

function MakeButton( $action ) {
	echo "
		<form action='tests.php' method='POST'>
			<input type='submit' name='action' value='$action'>
		</form>
		";
}
?>
	<html>
	<body>

	<form action="tests.php" method="POST"><input type="submit" name="action" value="poop"></form>
	
<?php
///////////////////////////////////////////////////////////////////////////////

if( $_POST["action"] == "setup" ) {
	$db = \SQLW::Get();
}

echo '</body></html>';
?>

</body>
</html>