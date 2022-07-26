import uvicorn
from rpcpy import RPC
import random
import os
import json
from datetime import date


app = RPC(mode="ASGI")


@app.register
async def none() -> None:
    return


@app.register
async def online_calculator(expression: str) -> str:
    return eval(expression)


@app.register
async def register_delivery(cost: int, address: str, order_list: str) -> int:
    delivery_id = random.randint(10000000, 99999999)
    data = {'cost': cost, 'address': address, 'order_list': order_list}
    with open('data/' + str(delivery_id) + '.json', 'w', encoding='utf-8') as f:
        f.write(json.dumps(data, ensure_ascii=False))
    return delivery_id


@app.register
async def get_delivery(id: int) -> str:
    data_file = 'data/' + str(id) + '.json'
    if os.path.isfile(data_file):
        f = open(data_file)
        data = json.load(f)
        return str(data)
    return str({'Error': 'Not found'})


@app.register
async def delivery_exist(id: str) -> list:
    data_dir = 'data/'
    out_list = []
    for r, d, f in os.walk(data_dir):
        for file in f:
            if file[:len(id)] == id:
                out_list.append(os.path.join(r, file))
    return out_list


if __name__ == "__main__":
    random.seed(str(date.today()))
    uvicorn.run(app, interface="asgi3", port=65432, host='0.0.0.0')
