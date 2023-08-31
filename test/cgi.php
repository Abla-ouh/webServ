<?php
$variableName = "QUERY_STRING"; // Change this to the name of your environment variable

if (isset($_SERVER[$variableName])) {
    $variableValue = $_SERVER[$variableName];
    echo "Value of $variableName: $variableValue";
} else {
    echo "Environment variable $variableName not set.";
}
?>
