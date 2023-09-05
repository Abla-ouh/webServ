#!/usr/bin/python

import sys
import os

print("HTTP/1.1 200 OK")
print("Content-type: text/html\r\n")

# Print the received data to stdout
data = ""
if os.environ.get('REQUEST_METHOD') == "POST":
	data = sys.stdin.read()
Cookie = os.environ.get("HTTP_COOKIE")

html_response = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Form Data</title>
	<style>
		body {{
			text-align: center;
			font-family: Arial, Helvetica, sans-serif;
			background-color: #1d3557;
			color: #f1faee;
		}}
	</style>
</head>
<body>
    <h1>CGI working good from Python</h1>
    <p>Form: {data}</p>
    <p>Cookie: {Cookie}</p>
</body>
</html>
"""
print(html_response)
