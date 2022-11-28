import os
import socket
import errno

HOST = "127.0.0.1"  # The server's hostname or IP address
port1 = 4567#נתב
port2 = 0
data =""

#sending message to נתב to get a route
try:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, port1))
        s.sendall(b"Hello")
        data = s.recv(1024).decode()
        print(data)
except socket.error as error:
    if error.errno == errno.ECONNREFUSED:
        print("Connection refused")
    elif error.errno == errno.EHOSTDOWN:
        print("Host is down")
    elif error.errno == errno.EHOSTUNREACH:
        print("No route to host")
    elif error.errno == errno.ETIMEDOUT:
        print("Connection timed out")

print("received from natav - " + data)
port2 = data[:4]#take port from data
data = data[4:]#sending data without the next port

#sending message to שרת ביינים based on the route from the נתב
try:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, int(port2)))
        s.sendall(data.encode() + b"Hello" )
except socket.error as error:
    if error.errno == errno.ECONNREFUSED:
        print("Connection refused")
    elif error.errno == errno.EHOSTDOWN:
        print("Host is down")
    elif error.errno == errno.EHOSTUNREACH:
        print("No route to host")
    elif error.errno == errno.ETIMEDOUT:
        print("Connection timed out")
print("sent")
input()#so window dosnt close 