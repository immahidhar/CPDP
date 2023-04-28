# API Tester

A load/stress testing tool used to test the performance of any REST API with respect to response time.

The tool can send concurrent API requests to multiple processes in a round robin fashion to ensure fairness.
The APIs themselves will be chosen randomly.

Make sure python3 is installed and all the required libraries are imported. 
See *requirements.txt*.

### api_config:

This is the configuration file which the tool reads to test the API.
All the config details including the API should be mentioned in this file.

* _num_requests_: Total number of requests to send as part of test.
* _rps_: Number of requests per second.
* _max_threads_: Maximum number of threads to assign for tests.
* _processes_: The url of the processes (just hostname:port).
* _apis_: A list of config names of the API to be included in test.
* [...]: The API config name which has the API endpoint details 
  * request_type, api_url, payload, headers

To run the tool, enter the following command:
```commandline
$ python3 api_tester.py
```
