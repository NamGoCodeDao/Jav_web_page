import socket
import threading

max_length=256
# Hàm xử lý kết nối của mỗi client
def handle_client(client_socket, address):

    print(f"Accepted connection from: {address}")
    while True:
        data = client_socket.recv(max_length).decode()

        if not data:
            print("Data receive error")
            break

        if data == "Close":
            print(f"Closing connection with {address}")
            client_socket.close()
            break

        print(f"Received from {address}: {data}")
        response = data.upper()
        client_socket.send(response.encode())

# Tạo socket server
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Gắn server với một địa chỉ và cổng
server.bind(("0.0.0.0", 8888))

# Lắng nghe kết nối tới và cho phép tối đa 5 client cùng lúc
server.listen(5)
print("Server is listening...")

while True:
    client_socket, client_address = server.accept()
    client_thread = threading.Thread(target=handle_client, args=(client_socket, client_address))
    client_thread.start()

