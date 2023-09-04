#!/usr/bin/python

import sys
import os

print("HTTP/1.1 200 OK")
print("Content-type: text/html\r\n")

# Print the received data to stdout
data = sys.stdin.read()

html_response = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Form Data</title>
</head>
<body>
    <h1>CGI working good</h1>
    <p>data: {data}</p>
</body>
</html>
"""
print("Received data:")
print(html_response)