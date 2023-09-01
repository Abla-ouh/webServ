import time

print("Content-type: text/html\n")
print("<html>")
print("<head>")
print("<title>Delayed CGI Example</title>")
print("</head>")
print("<body>")
print("<h1>Delayed Python CGI Example</h1>")
print("<p>This is a delayed response.</p>")

# Introduce a delay of 5 seconds
time.sleep(50)

print("<p>Delayed response is complete!</p>")
print("</body>")
print("</html>")