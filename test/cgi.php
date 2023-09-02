<?php

// Read from stdin
$stdin = fopen("php://stdin", "r");
if ($stdin) {
    while (($line = fgets($stdin)) !== false) {
        // Print to stdout
        echo $line;
    }
    fclose($stdin);
} else {
    // Handle error opening stdin
    header("Status: 500 Internal Server Error");
    die("Failed to open stdin\n");
}
echo "Hello World\n";
?>
