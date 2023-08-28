<?php
// Get all environment variables
$envVariables = $_SERVER;

// Set the content type to plain text
header('Content-Type: text/plain');

// Print the environment variables as the HTTP response
foreach ($envVariables as $key => $value) {
    echo "$key: $value\n";
}
?>


// print argument passed to the script
<?php
echo "Hello, World!\n";
echo "You passed the following arguments to the script: ";
foreach ($argv as $arg) {
    echo "$arg ";
}
echo "\n";
?>