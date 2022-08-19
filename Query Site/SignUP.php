<?php
echo "PLEASE ENTER USERNAME AND PASSWORD TO CREATE AN ACCOUNT"
$servername = "cssql.seattleu.edu";
$username = "ll_etarlov";
$password = "ll_etarlov";
$dbname = "ll_etarlov";
// Create connection
$conn = mysqli_connect($servername, $username, $password, 
$dbname);
// Check connection
if (!$conn) {
die("Connection failed: " . mysqli_connect_error());
}

$username = $_POST["username"];
$pwd = $_POST["password"];
$sql = "INSERT values INTO Account('$username', MD5('$pwd')"

<a href="http://css1.seattleu.edu/~bgipson/login.php" target="_blank">BACK TO LOGIN</a>
mysqli_close($conn);
?> 