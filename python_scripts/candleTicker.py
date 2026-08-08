

import json
import websocket

URL = "wss://socket.massive.com/crypto"

with open("../apiKey.txt", "r") as file:
    API_KEY = file.read().strip()


def on_open(ws):
    print("Connected")

    ws.send(json.dumps({
        "action": "auth",
        "params": API_KEY
    }))

def on_message(ws, message):
    events = json.loads(message)

    for event in events:
        if event.get("ev") == "status":
            print(event.get("message"))

            if event.get("status") == "auth_success":
                ws.send(json.dumps({
                    "action": "subscribe",
                    "params": "XA.BTCUSD"
                }))

        elif event.get("ev") == "XA":
            print("\nBTCUSD candle")
            print("Open:  ", event.get("o"))
            print("High:  ", event.get("h"))
            print("Low:   ", event.get("l"))
            print("Close: ", event.get("c"))
            print("Volume:", event.get("v"))
            print("Start: ", event.get("s"))
            print("End:   ", event.get("e"))


def on_error(ws, error):
    print("Error:", error)


def on_close(ws, close_status_code, close_message):
    print("Connection closed")


if not API_KEY:
    raise RuntimeError("api_key.txt is empty")

ws = websocket.WebSocketApp(
    URL,
    on_open=on_open,
    on_message=on_message,
    on_error=on_error,
    on_close=on_close
)

ws.run_forever()