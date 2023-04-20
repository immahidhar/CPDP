# API tester for concurrency and distributed performance

import requests

res = requests.get("http://localhost:8080/order/get_all")

print(res.json())
