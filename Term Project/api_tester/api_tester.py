# API tester for concurrency and distributed performance

import json
import random

import time
import random
import requests
import threading
import statistics
import numpy as np
from time import sleep
from configparser import ConfigParser
from concurrent.futures import ThreadPoolExecutor

api_config_filename = "api_config"

lock = threading.Lock()

# thread results
futures = list()
req_count = 0

start = time.time()
print("Start time = " + time.time().__str__())


class API:
    """
    API class to store api info including api response
    """
    api: str
    req_type: str
    url: str
    payload: str
    headers: str
    res: requests.Response

    def __init__(self, api, req_type, url, payload, headers):
        self.api = api
        self.req_type = req_type
        self.url = url
        self.payload = payload
        self.headers = headers

    def __str__(self):
        return f"{self.api}: {self.req_type} {self.url}"


def get_api_config(filename):
    """
    read api configuration from api_config file
    :param filename:
    :return: threads, processes, apis
    """
    config_object = ConfigParser()
    config_object.read(filename)
    threads = config_object["threads"]["max_threads"]
    machine_processes = config_object["machines"]
    processes = json.loads(machine_processes["processes"])
    apis = json.loads(config_object["api"]["apis"])
    return config_object, threads, processes, apis


def get_thread_pool(num_threads):
    """
    creates a thread pool with num_threads number of threads
    uses ThreadPoolExecutor from "concurrent.futures"
    :param num_threads:
    :return: thread pool executor object
    """
    print("Creating thread pool with " + num_threads.__str__() + " threads")
    executor = ThreadPoolExecutor(int(num_threads))
    print("Thread pool ready")
    return executor


def test(api_obj: API, start_t, rps):
    """
    test by sending a http request and check response
    :param api_obj:
    :return:
    """
    # acquire the lock
    # lock.acquire()

    # rate limiting
    global req_count
    """n_sec = round(time.time() - start_t)
    # print("req_count: " + req_count.__str__() + " n_sec: " + n_sec.__str__())
    while req_count > rps and req_count > rps * n_sec:
        # print("Thread waiting for rate limiting")
        n_sec = round(time.time() - start_t)
        sleep(0.01)
    """

    res = None
    api_obj.headers = {
        'Content-Type': 'application/json'
    }
    data = None
    req_count += 1
    if not api_obj.payload == "":
        data = json.loads(api_obj.payload)
    # print("Sending request")
    if api_obj.req_type == 'GET':
        try:
            res = requests.get(url=api_obj.url, data=json.dumps(data), headers=api_obj.headers)
        except requests.exceptions.RequestException as e:
            print("request failed!", e)
    elif api_obj.req_type == 'PUT':
        try:
            res = requests.put(url=api_obj.url, data=json.dumps(data), headers=api_obj.headers)
        except requests.exceptions.RequestException as e:
            print("request failed!", e)
    elif api_obj.req_type == 'POST':
        try:
            res = requests.post(url=api_obj.url, data=json.dumps(data), headers=api_obj.headers)
        except requests.exceptions.RequestException as e:
            print("request failed!", e)
    else:
        print("req_type not handled")
    api_obj.res = res

    # print("Received response")

    # release the lock
    # lock.release()

    return api_obj


def compute_performance():
    """
    computer performance
    :return:
    """
    # get results to compute performance
    num_reqs_failed = 0
    num_reqs_successful = 0
    elapsed_list = list()
    for future in futures:
        api_obj: API = future.result()
        # print(api_obj, api_obj.res)
        if api_obj.res is None:
            # request failed
            num_reqs_failed += 1
        else:
            # check response status
            if not api_obj.res.ok:
                print(api_obj, api_obj.res)
                num_reqs_failed += 1
            else:
                num_reqs_successful += 1
                res_elapsed: datetime.timedelta = api_obj.res.elapsed
                elapsed_list.append(res_elapsed.total_seconds())

    print("\tNumber of requests failed = " + num_reqs_failed.__str__())
    print("\tNumber of requests successful = " + num_reqs_successful.__str__())
    print("\tPercentage of requests successful = " + (
                num_reqs_successful / (num_reqs_successful + num_reqs_failed) * 100).__str__())
    if elapsed_list.__len__() > 0:
        print("\tAverage response time = " + statistics.mean(elapsed_list).__str__())
        print("\tp99 = " + np.percentile(elapsed_list, 99).__str__())
        print("\tp90 = " + np.percentile(elapsed_list, 90).__str__())
        print("\tp75 = " + np.percentile(elapsed_list, 75).__str__())
        print("\tp50 = " + np.percentile(elapsed_list, 50).__str__())
        print("\tp25 = " + np.percentile(elapsed_list, 25).__str__())


def main():
    """
    test performance
    :return:
    """
    # get config
    config, max_threads, processes_apis, apis = get_api_config(api_config_filename)
    print(max_threads, processes_apis, apis)
    num_processes = processes_apis.__len__()
    print("Number of processes = " + num_processes.__str__())
    num_api = apis.__len__()
    print("Number of apis = " + num_api.__str__())

    # get thread pool
    executor = get_thread_pool(int(max_threads))

    # send num_requests requests randomly among machines and apis
    num_reqs = int(config["test"]["num_requests"])
    rps = int(config["test"]["rps"])
    print("sending " + num_reqs.__str__() + " requests at " + rps.__str__() + " requests per second ... ")
    for i in range(num_reqs):
        # rate limiting
        """n_sec = round(time.time() - start)
        while i > rps and i > rps * n_sec:
            print(time.time().__str__() + ": sent " + req_count.__str__() + " requests, waiting ...")
            n_sec = round(time.time() - start)
            sleep(1)
        """

        # round-robin for process
        process_index = i % num_processes
        # get random number for api
        api_index = random.choice(range(num_api))
        # print(process_index, api_index)

        # prepare api request randomly
        process_url = processes_apis[process_index]
        api = apis[api_index]
        req_type = config[api]["request_type"]
        api_url = config[api]["api_url"]
        payload = config[api]["payload"]
        headers = config[api]["headers"]
        url = process_url + api_url
        api_obj = API(api=api, req_type=req_type, url=url, payload=payload, headers=headers)
        # print(api_obj.__str__())

        # submit task to thread pool
        futures.append(executor.submit(test, api_obj, start, rps))

    # print num of reqs sent
    while req_count < num_reqs:
        print(time.time().__str__() + ": sent " + req_count.__str__() + " requests")
        sleep(1)

    end = time.time()
    print("All " + num_reqs.__str__() + " requests send in " + (end - start).__str__() + " seconds")
    print("RPS = " + (num_reqs / round(end-start)).__str__())

    # wait for all threads to be done
    for thread in futures:
        while not thread.done():
            sleep(1)

    # compute performance
    compute_performance()

    # shutdown thread pool
    executor.shutdown()


if __name__ == '__main__':
    main()
