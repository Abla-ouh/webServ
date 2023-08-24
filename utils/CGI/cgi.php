#!/usr/bin/php-cgi
<?php
// Read the input from the HTML form
$name = isset($_POST['name']) ? $_POST['name'] : '';

// Output the CGI response
echo "Content-type: text/html\r\n\r\n";
echo "<html><head><title>PHP CGI Example</title></head><body>";

if ($name) {
    echo "<h1>Hello, $name!</h1>";
} else {
    echo "<h1>Please enter your name in the form below.</h1>";
}

echo "<form method='post' action='index.php'>";
echo "Name: <input type='text' name='name'><br>";
echo "<input type='submit' value='Submit'>";
echo "</form>";

echo "</body></html>";
?>