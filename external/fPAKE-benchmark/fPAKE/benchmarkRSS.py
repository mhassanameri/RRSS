from fPAKERSS import *
import argparse
import configparser
import json
import os
import glob
import re
import statistics


def get_finger_prints_gzip(path):
    return glob.glob(os.path.join(path, "**", "*.gz"), recursive=True)


def get_fingerprints_json(path):
    return glob.glob(
        os.path.join(path, "[1-2]", "**", "result_*.json"),
        recursive=True
    )


numberoOfExecution = range(1)
bench = "benchmark"
role = None

data = {}
data[bench] = []
start = [0]


parser = argparse.ArgumentParser()
parser.add_argument(
    "role",
    nargs="?",
    choices=["Sender", "Receiver"],
    help="Protocol role to run."
)
parser.add_argument(
    "--config",
    required=True,
    help="Path to the benchmark configuration file."
)

args = parser.parse_args()
role = args.role

config = configparser.ConfigParser()

if not config.read(args.config):
    raise FileNotFoundError(
        f"Could not read configuration file: {args.config}"
    )

IP = config["DEFAULT"]["IP"]
PORT = int(config["DEFAULT"].get("PORT", "20005"))
seclvl = int(config["DEFAULT"]["SECPARAM"])
jsonpath = config["DEFAULT"]["JSONDIR"]

if role is None:
    print("Role read from configuration file.")
    role = config["DEFAULT"]["ROLE"]

if seclvl == 0:
    input_root = "../results128"
    output_root = "RSSresults128"
elif seclvl == 1:
    input_root = "../results244"
    output_root = "RSSresults244"
else:
    raise ValueError(f"Unsupported SECPARAM: {seclvl}")

print(f"Input directory:  {jsonpath}")
print(f"Output directory: {output_root}")
print(f"Role:             {role}")
print(f"Port:             {PORT}")

fps = get_fingerprints_json(jsonpath)
fps.sort()

if not fps:
    raise FileNotFoundError(
        f"No result JSON files found under JSONDIR: {jsonpath}"
    )


def get_timing(interLayer):
    c = 0
    n = 0

    for iteration in interLayer["LiPAKE"]:
        c += interLayer["LiPAKE"][iteration]["crypto_time"]
        n += interLayer["LiPAKE"][iteration]["network_time"]

    return c, n


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
    rel_path = os.path.relpath(filepath, input_root)
    result_filepath = os.path.join(output_root, rel_path)

    benchrun = {}
    benchrun["file"] = name
    benchrun["results"] = {}
    stamplayer = benchrun["results"]

    with open(jsonfile, "r") as jf:
        parsed = json.load(jf)
        stamps = parsed["results"]

        for s in stamps:
            prints = stamps[s]
            fp = prints["fp"]

            if role.lower() != file_role.lower():
                continue

            stamplayer[s] = {}
            stamplayer[s]["role"] = role.lower()
            stamplayer[s]["fp"] = fp
            stamplayer[s]["execution"] = {}

            exec_iteration = stamplayer[s]["execution"]
            network_timings = []
            computation_timings = []
            sender_communication_overhead = []
            receiver_communication_overhead = []

            for i in numberoOfExecution:
                print("Number of execution:", i)

                interLayer = {}
                exec_iteration["{:03d}".format(i)] = interLayer

                conn = IPConnection(IP=IP, PORT=PORT)
                pw = fp

                if role.lower() == "sender":
                    finalpw = fPAKE(
                        weakPW=pw,
                        connection=conn,
                        securityParam=seclvl
                    ).init_Protocol(interLayer)

                elif role.lower() == "receiver":
                    finalpw = fPAKE(
                        weakPW=pw,
                        connection=conn,
                        securityParam=seclvl
                    ).receive_protocol(interLayer)

                interLayer["negotiated_key"] = "".join(
                    format(byte, "08b") for byte in finalpw
                )

                network_timings.append(interLayer["total_network_time"])
                computation_timings.append(
                    interLayer["total_calculation_time"]
                )

                if role.lower() == "sender":
                    sender_communication_overhead.append(
                        interLayer["total_sender_communication"]
                    )

                elif role.lower() == "receiver":
                    receiver_communication_overhead.append(
                        interLayer["total_receiver_communication"]
                    )

            stamplayer[s]["avg_network_time"] = statistics.mean(
                network_timings
            )
            stamplayer[s]["avg_calculation_time"] = statistics.mean(
                computation_timings
            )

            if role.lower() == "sender":
                stamplayer[s]["avg_Sender_CommOverhead"] = statistics.mean(
                    sender_communication_overhead
                )

            elif role.lower() == "receiver":
                stamplayer[s]["avg_Receiver_CommOverhead"] = statistics.mean(
                    receiver_communication_overhead
                )

            os.makedirs(result_filepath, exist_ok=True)

            output_file = os.path.join(
                result_filepath,
                f"result_{k1}_{k2}{role}.json"
            )

            with open(output_file, "w") as f:
                json.dump(benchrun, f, indent=4, sort_keys=True)

            print(f"Results saved: {output_file}")

print("Finished.")