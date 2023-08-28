#!/usr/bin/php-cgi
<?php
// Headers
header("Content-Type: text/html; charset=utf-8");
header("X-Powered-By: PHP-CGI");
header("Cache-Control: no-cache, no-store, must-revalidate");
header("Pragma: no-cache");
header("Expires: 0");
?>

<!DOCTYPE html>
<html>
<head>
    <title>PHP-CGI Example</title>
</head>
<body>
    <h1>Hello, PHP-CGI World!</h1>
    <p>This is a simple example of a PHP-CGI script.</p>
    <p>Current date and time: <?php echo date('Y-m-d H:i:s'); ?></p>
</body>
</html>
