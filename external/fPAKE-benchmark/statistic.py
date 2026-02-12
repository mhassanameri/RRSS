import sys
import statistics
import numpy as np
import json
import os
import glob
import datetime

default_folder = "fPAKE/RRSSresults244" # "results244" Source folder of the results


def print_stats():
    print(f"{os.path.basename(old)} \t calc: {statistics.mean(avg_calc)} ± {statistics.stdev(avg_calc)} \t net: {statistics.mean(avg_net)} ± {statistics.stdev(avg_net)} \t\t overall: {statistics.mean(overall)} ± {statistics.stdev(overall)}")
    # print(f"{os.path.basename(old)} \t Sender Communication Overhead: {statistics.mean(avg_send_commoverhead)} ± {statistics.stdev(avg_send_commoverhead)} ")
    print(f"{os.path.basename(old)} \t Receiver Communication Overhead: {statistics.mean(avg_rece_commoverhead)} ± {statistics.stdev(avg_rece_commoverhead)} ")

    """print("\tKey Size:\t", lastkey)
    print("\tNumber of Samples :\t", len(overall))
    print("\tOverall Timing stats in Seconds:")
    print("\t\tMean :\t\t", statistics.mean(overall))
    print("\t\tstdev :\t\t", statistics.stdev(overall))
    print("\t\tMedian :\t", statistics.median(overall))
    print("\t\tVariance :\t", statistics.variance(overall))
    print("\tCalculation stats in Seconds:")
    print("\t\tMean :\t\t", statistics.mean(avg_calc))
    print("\t\tstdev :\t\t", statistics.stdev(avg_calc))
    print("\t\tMedian :\t", statistics.median(avg_calc))
    print("\t\tVariance :\t", statistics.variance(avg_calc))
    print("\tNetwork stats in Seconds:")
    print("\t\tMean :\t\t", statistics.mean(avg_net))
    print("\t\tstdev :\t\t", statistics.stdev(avg_net))
    print("\t\tMedian :\t", statistics.median(avg_net))
    print("\t\tVariance :\t", statistics.variance(avg_net))"""


# files = glob.glob(os.path.join(default_folder,"**","*Sender.json"),recursive=True)
files = glob.glob(os.path.join(default_folder,"**","*Receiver.json"),recursive=True)
files.sort()
old = os.path.dirname(files[0])
old = os.path.split(files[0])[0]
counter = 0
avg_calc = []
avg_net = []
overall = []
# avg_send_commoverhead = []
avg_rece_commoverhead = []

lastkey = 0
lastfile = ""
for file in files:
    lastfile = file
    if not os.path.split(file)[0] == old:
        print_stats()
        avg_calc = []
        avg_net = []
        overall = []
        # avg_send_commoverhead = []
        avg_rece_commoverhead = []
        # overall_commoverhead = []
        old = os.path.split(file)[0]

    with open(file,"r") as json_file:
        js = json.loads(json_file.read())

    for result in js["results"]:
        avg_calc.append(js["results"][result]["avg_calculation_time"]/1000000000)
        avg_net.append(js["results"][result]["avg_network_time"]/1000000000)
        overall.append((js["results"][result]["avg_network_time"]+js["results"][result]["avg_calculation_time"])/1000000000)
        lastkey = js["results"][result]["fp"].__len__()
        # if js["results"][result]["avg_Sender_CommOverhead"] is not None:
        # avg_send_commoverhead.append(js["results"][result]["avg_Sender_CommOverhead"]/1024) #Computing in KB. 
        # if js["results"][result]["avg_Receiver_CommOverhead"] is not None:
        avg_rece_commoverhead.append(js["results"][result]["avg_Receiver_CommOverhead"]/1024) #Computing in KB.

print_stats()