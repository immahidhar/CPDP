# API tester for concurrency and distributed performance

import json
import random

import time
import random
import requests
import statistics
import numpy as np
from time import sleep
from configparser import ConfigParser
from concurrent.futures import ThreadPoolExecutor

api_config_filename = "api_config"


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


def test(api_obj: API):
    """
    test by sending a http request and check response
    :param api_obj:
    :return:
    """
    res = None
    api_obj.headers = {
        'Content-Type': 'application/json'
    }
    data = None
    if not api_obj.payload == "":
        data = json.loads(api_obj.payload)
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
    return api_obj


def compute_performance(futures):
    """
    computer performance
    :param futures:
    :return:
    """
    # wait for all threads to be done
    for thread in futures:
        while not thread.done():
            sleep(1)

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

    print("Number of requests failed = " + num_reqs_failed.__str__())
    print("Number of requests successfully = " + num_reqs_successful.__str__())
    if elapsed_list.__len__() > 0:
        print("Average response time = " + statistics.mean(elapsed_list).__str__())
        print("p99 = " + np.percentile(elapsed_list, 99).__str__())
        print("p90 = " + np.percentile(elapsed_list, 90).__str__())
        print("p75 = " + np.percentile(elapsed_list, 75).__str__())
        print("p50 = " + np.percentile(elapsed_list, 50).__str__())
        print("p25 = " + np.percentile(elapsed_list, 25).__str__())


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
    # thread results
    futures = list()

    start_time = round(time.time() * 1000)
    start = start_time
    print("Start time = " + time.time().__str__())

    # send num_requests requests randomly among machines and apis
    num_reqs = int(config["test"]["num_requests"])
    rps = int(config["test"]["rps"])
    print("sending " + num_reqs.__str__() + " requests at " + rps.__str__() + " requests per second ... ")
    for i in range(num_reqs):
        # rate limiting
        if i != 0 and i % rps == 0:
            now = round(time.time() * 1000)
            if now - start_time < 1000:
                # exceeding rps
                print("time = " + time.time().__str__())
                print("Sent " + i.__str__() + " requests, waiting")
                # wait
                sleep((1000 - (now - start_time)) / 1000)
                start_time = round(time.time() * 1000)
            else:
                print("Sent " + i.__str__() + " requests")

        # get random numbers
        process_index = random.choice(range(num_processes))
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
        futures.append(executor.submit(test, api_obj))

    end = round(time.time() * 1000)
    print("All " + num_reqs.__str__() + " requests send in " + (end - start).__str__() + " seconds")

    # compute performance
    compute_performance(futures)

    # shutdown thread pool
    executor.shutdown()


if __name__ == '__main__':
    main()
