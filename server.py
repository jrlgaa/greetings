import socket
import threading
import pickle

# Define server details
SERVER_IP = '192.168.1.17'  # Replace with your local IPv4 address
PORT = 5555
ADDR = (SERVER_IP, PORT)

# Create a socket object
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind and listen
try:
    server.bind(ADDR)
    server.listen()
    print(f"Server is running on {SERVER_IP}:{PORT}")
except Exception as e:
    print(f"Error starting server: {e}")
    exit(1)

# Store connected clients and game state
clients = []
game_state = {
    "players": {},  # Store player info by address
    "game_started": False  # Track if the game has started
}


def broadcast_game_state():
    """Send the current game state to all connected clients."""
    data = pickle.dumps(game_state)
    for client in clients:
        try:
            client.send(data)
        except Exception as e:
            print(f"Error sending data to client: {e}")


def handle_client(conn, addr):
    print(f"New connection: {addr}")

    # Initialize player data
    game_state["players"][addr] = {
        "position": (0, 0),  # Example initial position
        "ready": False  # Player readiness
    }
    clients.append(conn)  # Add client connection

    # Send updated game state to all clients
    broadcast_game_state()

    while True:
        try:
            # Receive data from the client
            data = conn.recv(4096)
            if not data:
                print(f"Connection closed by {addr}")
                break

            # Deserialize and update game state
            updated_state = pickle.loads(data)
            if isinstance(updated_state, dict):
                game_state["players"][addr].update(updated_state)

                # Broadcast the updated game state
                broadcast_game_state()
        except Exception as e:
            print(f"Error handling data from {addr}: {e}")
            break

    # Handle client disconnection
    clients.remove(conn)
    del game_state["players"][addr]
    conn.close()
    print(f"Client {addr} disconnected")
    broadcast_game_state()
def start_server():
    """Start the server and accept client connections."""
    print("Server is starting...")
    while True:
        conn, addr = server.accept()
        thread = threading.Thread(target=handle_client, args=(conn, addr))
        thread.start()


if __name__ == "__main__":
    try:
        start_server()
    except KeyboardInterrupt:
        print("Server shutting down...")
        for client in clients:
            client.close()
        server.close()
