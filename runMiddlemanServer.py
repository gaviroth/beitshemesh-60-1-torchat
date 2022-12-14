import os
stream = os.popen("cd")
output = stream.read()

l = [1235,1236,1237,1238,1239]

for port in l:
    os.system('start "" cmd /k %cd%/MiddlemanServer/Debug/MiddlemanServer.exe ' + str(port))
    print ("MiddlemanServer/Debug/MiddlemanServer.exe " + str(port))
    