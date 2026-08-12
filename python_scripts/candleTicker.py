

import json
import websocket
import subprocess
import fairValueGap as fvg
from fairValueGap import Candle

from massive import WebSocketClient
from massive.websocket.models import WebSocketMessage, Feed, Market
from typing import List

URL = "wss://socket.massive.com/forex"
FX_INSTRUMENT = "EURUSD"
WS_FX_PAIR = "EUR/USD"
CANDLE_WINDOW_LEN = 60
candleWindow = [Candle]

# get high low
initCandle = fvg.getInitialCandle(("C:" + FX_INSTRUMENT), 5, "minute")
high = initCandle.high
low = initCandle.low
startTime = initCandle.timestamp

print("high: ", high, "low: ", low)

with open("../apiKey.txt", "r") as file:
    KEY = file.read().strip()

def on_open(ws):
    print("Connected")

    ws.send(json.dumps({
        "action": "auth",
        "params": KEY
    }))

def on_message(ws, message):
    events = json.loads(message)

    for event in events:
        if event.get("ev") == "status":
            print(event.get("message"))

            if event.get("status") == "auth_success":
                ws.send(json.dumps({
                    "action": "subscribe",
                    "params": "CA." + WS_FX_PAIR
                }))

        elif event.get("ev") == "CA":

            
            
            # get candle
            currentCandle = Candle(float(event.get("o")),
                                   float(event.get("h")),
                                   float(event.get("l")),
                                   float(event.get("c")),
                                   float(event.get("s")))
            c = currentCandle
            print((f"{c.timestamp}, o={c.open}, h={c.high}, l={c.low}, c={c.close}"))

            # append to window
            if len(candleWindow) < CANDLE_WINDOW_LEN:
                candleWindow.append(currentCandle)
            else:
                candleWindow.pop(0)
                candleWindow.append(currentCandle)

            # see if fvg conditions are met
            conditionsMet = False
            if len(candleWindow) > 3:
                conditionsMet = fvg.FindBullishFairValueGap(currentCandle[-3:-1])
            
            if conditionsMet:
                print("value gap found!")
                with open("example.txt", "w", encoding="utf-8") as file:
                    
                    file.write(f"high: {high}\n")
                    file.write(f"low: {low}\n")
                    file.write(f"start time: {startTime}\n")
                    file.write("\n")
                    file.write("\n")
                    file.write("t,open,high,low,close\n")
                    
                    for c in candleWindow:
                        file.write(f"{c.timestamp},{c.open},{c.high},{c.low},{c.close}\n")


def on_error(ws, error):
    print("Error:", error)

def on_close(ws, close_status_code, close_message):
    print("Connection closed")

if not KEY:
    raise RuntimeError("api_key.txt is empty")

ws = websocket.WebSocketApp(
    URL,
    on_open=on_open,
    on_message=on_message,
    on_error=on_error,
    on_close=on_close
)

ws.run_forever()

