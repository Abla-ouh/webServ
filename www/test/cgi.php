<?php
// Set the HTTP response headers
header("HTTP/1.1 200 OK");
header("Content-type: text/html");

// Print the received data from POST request
$data = "";
if ($_SERVER['REQUEST_METHOD'] == "POST") {
    $data = file_get_contents('php://input');
}

// Get and print the value of the "HTTP_COOKIE" environment variable
$cookie = $_SERVER['HTTP_COOKIE'];

// Generate the HTML response
$html_response = "
<!DOCTYPE html>
<html>
<head>
    <title>Form Data</title>
    <style>
        body {
            text-align: center;
            font-family: Arial, Helvetica, sans-serif;
            background-color: #1d3557;
            color: #f1faee;
        }
    </style>
</head>
<body>
    <h1>CGI working good from PHP</h1>
    <p>Form: $data</p>
    <p>Cookie: $cookie</p>
</body>
</html>
";

// Output the HTML response
echo $html_response;
?>
