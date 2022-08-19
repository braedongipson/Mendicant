<?php
$servername = "cssql.seattleu.edu";
$username = "ll_bgipson";
$password = "ll_bgipson";
$dbname = "ll_bgipson";
// Create connection
$conn = mysqli_connect($servername, $username, $password, $dbname);
// Check connection
if (!$conn) {
die("Connection failed: " . mysqli_connect_error());
}
$_POST["query"];
echo $_POST["query"];

$sql = $_POST["query"];
$result = mysqli_query($conn, $sql);
if (mysqli_num_rows($result) > 0) {
// output data of each row
while($row = mysqli_fetch_assoc($result)) {
echo "PokemonID " . $row["Pokemon_ID"]. " | Pokemon Name: " . $row["Pokemon_Name"]. " | Pokemon_Category: " . 
$row["Pokemon_Category"] . " | Type_ID: " . $row["Type_ID"] . "<br>";
}
} else {
echo "0 results";
}
// Free result set
mysqli_free_result($result);
mysqli_close($conn);
?> 
