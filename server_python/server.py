import socket
import ctypes
import struct

lib = ctypes.CDLL('../server/libserver_shared.so')

class VaccelRequest(ctypes.Structure):
    _fields_ = [("function_type", ctypes.c_uint32),
                ("which_function_args", ctypes.c_uint32),
                ("function_args", ctypes.c_uint32 * 2)] 
    
class VaccelResponse(ctypes.Structure):
    _fields_ = [("function_type", ctypes.c_uint32),
                ("which_function_args", ctypes.c_uint32),
                ("function_args", ctypes.c_uint32 * 2)] 

lib.receive_request.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_uint8), ctypes.c_size_t]
lib.receive_request.restype = ctypes.c_ssize_t

def setup_server_socket():
    HOST = '127.0.0.1'
    PORT = 12350
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((HOST, PORT))
    server_socket.listen()
    print(f"Server is listening on {HOST}:{PORT}")
    return server_socket

def accept_client_connection(server_socket):
    client_socket, client_address = server_socket.accept()
    print(f"Connected to {client_address}")
    return client_socket

def receive_request(client_socket):
    buffer_size = 1024
    buffer = (ctypes.c_ubyte * buffer_size)()
    bytes_received = lib.receive_request(client_socket.fileno(), buffer, buffer_size)
    if bytes_received <= 0:
        return None
    return bytes(buffer[:bytes_received])


def send_response(client_socket, response):
    response_buffer = ctypes.create_string_buffer(ctypes.sizeof(response))
    ctypes.memmove(response_buffer, ctypes.addressof(response), ctypes.sizeof(response))
    client_socket.sendall(response_buffer.raw)

def process_request_and_send_response(client_socket, request_buffer):

    response = lib.process_request_and_send_response(client_socket,request_buffer, 6)

    print("Sending response:")
    print(response)
    send_response(client_socket, response)

    

def create_session_response(flags):
    return 1

def update_session_response(flags):
    return 1

def destroy_session_response(flags):
    return 1

def main():
    server_socket = setup_server_socket()
    client_socket = accept_client_connection(server_socket)

    try:
        while True:
            request_buffer = receive_request(client_socket)
            if not request_buffer:
                break
            process_request_and_send_response(client_socket, request_buffer)
    except Exception as e:
        print("An error occurred:", e)
    finally:
        client_socket.close()
        server_socket.close()

if __name__ == "__main__":
    main()

if __name__ == "__main__":
    main()