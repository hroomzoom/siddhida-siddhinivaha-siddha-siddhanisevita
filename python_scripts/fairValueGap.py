from massive import RESTClient
from datetime import datetime, timedelta
from zoneinfo import ZoneInfo
from dataclasses import dataclass

@dataclass
class Candle:
    open: float
    high: float
    low: float
    close: float
    timestamp: int

def initClient() -> RESTClient:
    with open("../apiKey.txt", "r") as file:
        API_KEY = file.read().strip()
    client = RESTClient(api_key=API_KEY)
    return client

def getInitialCandle(instrument:str, multiplier:int, timespan:str):

    with open("../apiKey.txt", "r") as file:
        API_KEY = file.read().strip()
    client = RESTClient(api_key=API_KEY)

    easternTimeZone = ZoneInfo("America/New_York")
    nowEST = datetime.now(easternTimeZone)
   
    if timespan == "minute": 
        nowEST = nowEST.replace(second=0, microsecond=0)
        nowEST -= timedelta(minutes=nowEST.minute % multiplier)

    if timespan == "hour":
        nowEST = nowEST.replace(minute=0, second=0, microsecond=0)
        nowEST -= timedelta(hours=nowEST.hour % multiplier)

    timestamp = int(nowEST.timestamp() * 1000)

    candles = client.get_aggs(
        ticker=instrument,
        multiplier=multiplier,
        timespan=timespan,
        from_=str(timestamp),
        to=str(timestamp))
    
    initialCandle = Candle(candles[0].open, 
                           candles[0].high, 
                           candles[0].low, 
                           candles[0].close, 
                           candles[0].timestamp)
    
    return initialCandle
    

def FindBullishFairValueGap(triCandle:list[Candle], threshold:float) -> bool:

    #conditions
    increasing = False
    wickGap = False
    crossedThreshold = False
        
    # ensure the candles are candles are bullish
    for i in range(3):
        if isBullCandle(triCandle[i]) == False:
            return False
        
    for i in range(3):
        if triCandle[i].high > threshold:
            crossedThreshold = True
    
    # increasing candles
    if (triCandle[0].open < triCandle[1].open and 
        triCandle[1].open < triCandle[2].open and
        triCandle[0].close < triCandle[1].close and 
        triCandle[1].close < triCandle[2].close):
        increasing = True

    # gap between wicks
    if (triCandle[2].low - triCandle[0].high) > 0:
        wickGap = True

    return (crossedThreshold and increasing and wickGap)
    
def isBullCandle(candle: Candle):
    return candle.open < candle.close

def isBearishCandle(candle: Candle):
    return candle.open > candle.close
    
        

        
       

