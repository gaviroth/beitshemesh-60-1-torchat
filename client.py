import os
import socket
import errno

# FINAL OBJECTS

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 1234  # The port used by the server

# Variables

CLIENT_LOGIN = 'p'
CLIENT_SIGN_UP = 'o'

username = "username"
password = "Aa12345!"
mail = "username@mail.com"
status_code = 0 # 112 - Login , 111 - Sign Up

def login_format_str(username , password):
    json_str = '{"username":"' + username + '","password":"' + password + '"}myport:6789'#myport:6789
    string = str(CLIENT_LOGIN) + str(len(json_str)) + json_str
    return string
def signup_format_str(username , password , mail):
    json_str = '{"Username":"' + username + '","Password":"' + password + '","Email":"' + mail + '"}myport:6789'#myport:6789
    string = str(CLIENT_SIGN_UP) + str(len(json_str)) + json_str
    return string
def login_unformat_str(json):
    pass
def signup_unformat_str(json):
    pass

try:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        status_code = input("Enter status code : ")
        #username = input( "Enter username : ")
        #password = input( "Enter password : ")
        serialized_string = ""
        if (status_code == CLIENT_SIGN_UP): # check for mail needed
            #mail = input( "Enter mail : ")
            serialized_string = signup_format_str(username , password , mail)
        elif (status_code == CLIENT_LOGIN):
            serialized_string = login_format_str(username , password)
        print(serialized_string)
        s.sendall(serialized_string.encode())
        data = s.recv(1024)
    print(f"Received {data!r}")
    input()
except socket.error as error:
    if error.errno == errno.ECONNREFUSED:
        print("Connection refused")
    elif error.errno == errno.EHOSTDOWN:
        print("Host is down")
    elif error.errno == errno.EHOSTUNREACH:
        print("No route to host")
    elif error.errno == errno.ETIMEDOUT:
        print("Connection timed out")
