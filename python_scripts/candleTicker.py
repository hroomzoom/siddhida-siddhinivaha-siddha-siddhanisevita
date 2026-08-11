

# import json
# import websocket

import subprocess

with open("../apiKey.txt", "r") as file:
    API_KEY = file.read().strip()


try:
    # Run the C++ executable and capture its output
    result = subprocess.run(
        ["../rest", 
         "-t", "20:44:00", 
         "-i", "X:BTCUSD", 
         "-m", "1", 
         "-s", "hour",
         "-a", API_KEY],  # Path to exe and optional arguments
        capture_output=True,                   # Captures stdout and stderr
        text=True,                             # Returns output as string (instead of bytes)
        check=True                             # Raises an error if the executable crashes
    )
    
    # Print the program's output
    print("C++ Output:")
    print(result.stdout)

except subprocess.CalledProcessError as e:
    print(f"Executable failed with exit code {e.returncode}")
    print(f"Error output:\n{e.stderr}")
    
except FileNotFoundError:
    print("The specified C++ executable could not be found.")

# URL = "wss://socket.massive.com/crypto"

# with open("../apiKey.txt", "r") as file:
#     API_KEY = file.read().strip()


# def on_open(ws):
#     print("Connected")

#     ws.send(json.dumps({
#         "action": "auth",
#         "params": API_KEY
#     }))

# def on_message(ws, message):
#     events = json.loads(message)

#     print(message)

#     for event in events:
#         if event.get("ev") == "status":
#             print(event.get("message"))

#             if event.get("status") == "auth_success":
#                 ws.send(json.dumps({
#                     "action": "subscribe",
#                     "params": "XAS.BTCUSD"
#                 }))

#         elif event.get("ev") == "XAS":
#             print("\nBTCUSD candle")
#             print("Open:  ", event.get("o"))
#             print("High:  ", event.get("h"))
#             print("Low:   ", event.get("l"))
#             print("Close: ", event.get("c"))
#             print("Volume:", event.get("v"))
#             print("Start: ", event.get("s"))
#             print("End:   ", event.get("e"))


# def on_error(ws, error):
#     print("Error:", error)


# def on_close(ws, close_status_code, close_message):
#     print("Connection closed")


# if not API_KEY:
#     raise RuntimeError("api_key.txt is empty")

# ws = websocket.WebSocketApp(
#     URL,
#     on_open=on_open,
#     on_message=on_message,
#     on_error=on_error,
#     on_close=on_close
# )

# ws.run_forever()