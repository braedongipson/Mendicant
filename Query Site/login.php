<?php
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

$sql = "SELECT * FROM Accounts WHERE Username = '$username' and Password = MD5('$pwd')";
$result = mysqli_query($conn, $sql);
if (mysqli_num_rows($result) > 0) {
echo "welcome " . $username;
<a href="http://css1.seattleu.edu/~bgipson/Hot_Coffee.html" target="_blank">ENTER SITE</a>
} else {
echo "log in fail!";
}
mysqli_free_result($result);
mysqli_close($conn);
?> 