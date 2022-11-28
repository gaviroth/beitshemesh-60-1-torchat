import socket

HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
PORT =2345  # Port to listen on (non-privileged ports are > 1023)

data = ""
while(True):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        conn, addr = s.accept()
        with conn:
            print(f"Connected by {addr}")
            data = conn.recv(1024)
            
    port1 = data[:4]#take port from data
    data = data[4:]#sending data without the next port
    print(data)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, int(port1)))
        s.sendall(data)
    s.close()
