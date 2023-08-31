#!/usr/bin/php-cgi
<?php

// Read data from standard input
$inputData = file_get_contents('php://input');
header("Content-type: text/html\r\n\r\n");
echo "<html>";
echo "<head>";
echo "<title>CGI Input Data</title>";
echo "</head>";
echo "<body>";
echo "<h1>Input Data:</h1>";
echo "<p>{$inputData}</p>";
echo "</body>";
echo "</html>";
?>
