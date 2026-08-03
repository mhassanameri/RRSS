from fPAKERRSS import *
import configparser
import sys
import statistics
import gzip
import json
import os
import glob
import re

def get_finger_prints_gzip(path):
    return glob.glob(os.path.join(path,"**","*.gz"),recursive=True)
def get_fingerprints_json(path):
    return glob.glob(os.path.join(path,"[1-2]",  "**", "result_*.json"), recursive=True)
    # return glob.glob(os.path.join(path,"**","result_[0-9][0-9]_[0-9][0-9]Sender.json"),recursive=True)


numberoOfExecution = range(3)
bench = "benchmark"
role = None
root = os.getcwd()
if len(sys.argv) != 1:
    role = sys.argv[1]

data = {}
data[bench] = []
start = [0]
config = configparser.ConfigParser()
config.read("config.ini")
IP = config["DEFAULT"]["IP"]
seclvl = int(config["DEFAULT"]["SECPARAM"])
if role is None:
    print("rolefromConfigue")
    role = config["DEFAULT"]["ROLE"]   
jsonpath = config["DEFAULT"]["JSONDIR"]

pw = ""

print(jsonpath)

fps = get_fingerprints_json(jsonpath)
fps.sort()
# print(fps)
def get_timing(interLayer):
    c = 0
    n = 0
    for iter in interLayer["LiPAKE"]:
        c += interLayer["LiPAKE"][iter]["crypto_time"]
        n += interLayer["LiPAKE"][iter]["network_time"]
    return c, n

for seclvl in (0,1):
    for jsonfile in fps:
        
        print(jsonfile)
        name = os.path.basename(jsonfile)
        m = re.match(r"result_(\d+)_(\d+)(Sender|Receiver)\.json$", name)
        if not m:
            raise ValueError(f"Unexpected filename format: {name}")
        k1 = m.group(1)
        k2 = m.group(2)
        file_role = m.group(3)

        filepath, _ = os.path.split(jsonfile)
        rel_path = os.path.relpath(filepath, "../results128")
        
        if seclvl == 0:
            result_filepath = os.path.join("RRSSresults128", rel_path)
        else:
            result_filepath = os.path.join("RRSSresults244", rel_path)

        benchrun = {}
        benchrun["file"] = os.path.basename(jsonfile)
        benchrun["results"] = {}
        stamplayer = benchrun["results"]
        with open(jsonfile,"r") as jf:
            #print(jsonfile)
            content = jf.read()
            parsed = json.loads(content)
            stamps = parsed["results"]
            counter = 0
            for s in stamps :
                counter+=1
                prints = stamps[s]
                fp = prints["fp"]
                # stamplayer[s] = {}
                if role.lower() == file_role.lower():
                    stamplayer[s] = {}
                    stamplayer[s]["role"] = role.lower()
                    stamplayer[s]["fp"] = fp

                    stamplayer[s]["execution"] = {}
                    exec_iteration = stamplayer[s]["execution"]
                    network_timings = []
                    computation_timings = []
                    SenderCommunicationOverhead = []
                    RreceiverCommunicationOverhead= []
                    for i in numberoOfExecution:
                        print("Number of execution: ",i)
                        interLayer = {}
                        exec_iteration["{:03d}".format(i)] = interLayer
                        finalpw = ""
                        PORT = 10005
                        if role.lower() == "sender":
                            conn = IPConnection(IP="localhost", PORT =10005)
                            # print("[Sender] connect() =", ok)
                            pw = fp
                            # print(pw)
                            finalpw = fPAKE(weakPW=pw, connection=conn,securityParam=seclvl).init_Protocol(interLayer)
                        if role.lower() == "receiver":
                            conn = IPConnection(IP="localhost", PORT = 10005)
                            # print("[Receiver] waiting for connection on 0.0.0.0:10005 ...")
                            pw = fp
                            # print(f"[Receiver] pw = {pw!r}", file=sys.stderr, flush=True)
                            finalpw = fPAKE(weakPW=pw, connection=conn, securityParam=seclvl).receive_protocol(interLayer)
                        interLayer["negotiated_key"] = ''.join(format(byte, '08b') for byte in finalpw)
                        network_timings.append(interLayer["total_network_time"])
                        computation_timings.append(interLayer["total_calculation_time"])
                        if(role.lower()=="sender"):
                            SenderCommunicationOverhead.append(interLayer["total_sender_communication"])
                        if(role.lower()=="receiver"):
                            RreceiverCommunicationOverhead.append(interLayer["total_receiver_communication"])
                        # print("Final pw: ",''.join(format(byte, '02x') for byte in finalpw))
                    stamplayer[s]["avg_network_time"] = statistics.mean(network_timings)
                    stamplayer[s]["avg_calculation_time"] = statistics.mean(computation_timings)
                    
                    if(role.lower()== "sender"): stamplayer[s]["avg_Sender_CommOverhead"] = statistics.mean(SenderCommunicationOverhead)
                    if(role.lower()== "receiver"): stamplayer[s]["avg_Receiver_CommOverhead"] = statistics.mean(RreceiverCommunicationOverhead)
                    os.makedirs(result_filepath,exist_ok=True)
                    with open(os.path.join(result_filepath, "result_" + k1 + "_" + k2 + role + ".json"), "w") as f:
                        json.dump(benchrun, f,indent=4,sort_keys=True)
                        print("results saved: " + result_filepath)


print("finished")




