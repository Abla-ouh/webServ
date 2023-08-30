#!/usr/bin/php-cgi
<?php
// Set the content type to plain text
header('test:test');
// header('HTTP/1.1 500 Internal Server Error');
header('Status: 403 Forbidden');
header('Status: 404 Not Found');
// header('HTTP/1.1 201 Created');
header('Status: 301 Moved Permanently');

// Print the HTTP response
echo "Hello, World!\n";
echo "Status: 500 ok"
?>
