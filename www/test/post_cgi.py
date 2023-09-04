#!/usr/bin/python

import sys
import os

print("HTTP/1.1 200 OK")
print("Content-type: text/html\r\n")

# Read data from stdin
data = sys.stdin.read()

# Print the received data to stdout

html_response = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Form Data</title>
</head>
<body>
    <h1>Form Data Received</h1>
    <p>data: {data}</p>
</body>
</html>
"""
print("Received data:")
print(html_response)