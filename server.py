import socket
import threading
import pickle

# Server configuration
SERVER_IP = '192.168.1.17'  # Replace with your local IPv4 address
PORT = 5555
ADDR = (SERVER_IP, PORT)

# Initialize server
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(ADDR)
server.listen()

clients = []
game_state = {
    "players": {},  # Track each player's readiness status by address
    "game_started": False
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
    """Handle communication with a connected client."""
    print(f"New connection: {addr}")
    game_state["players"][addr] = {"ready": False}
    clients.append(conn)
    broadcast_game_state()  # Notify all clients of new connection

    while True:
        try:
            data = conn.recv(4096)
            if not data:
                print(f"Connection closed by {addr}")
                break

            # Update player readiness status from client
            updated_state = pickle.loads(data)
            if isinstance(updated_state, dict) and "ready" in updated_state:
                game_state["players"][addr]["ready"] = updated_state["ready"]

                # Check if all players are ready to start the game
                if all(player["ready"] for player in game_state["players"].values()) and len(game_state["players"]) == 2:
                    game_state["game_started"] = True

                broadcast_game_state()  # Update all clients with the latest game state
        except Exception as e:
            print(f"Error handling data from {addr}: {e}")
            break

    # Clean up on disconnect
    clients.remove(conn)
    del game_state["players"][addr]
    conn.close()
    print(f"Client {addr} disconnected")
    broadcast_game_state()

def start_server():
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
