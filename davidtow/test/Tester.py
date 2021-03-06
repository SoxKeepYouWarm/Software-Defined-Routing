import subprocess
import argparse
import random
import sys
import time

CONTROLLER = "../../controller/controller"
TOPOLOGY_EXAMPLE = "./topology_demo"
TOPOLOGY_FILE = "./topology_file"

APPLICATION_OUTPUT = "./output/application_output.txt"
APPLICATION_ERROR = "./output/application_error.txt"

CONTROLLER_OUTPUT = "./output/controller_output.txt"
CONTROLLER_ERROR = "./output/controller_error.txt"

OUTPUT_RULES = " 1>> " + CONTROLLER_OUTPUT + " 2>> " + CONTROLLER_ERROR

IP_STEM = "172.20.0."
IP_ADDRESS = "172.20.0.2"

ROUTER_ID = " 1 "
ROUTING_UPDATE_INTERVAL = " 3 "

AUTHOR_PREFIX = " -a "
INIT_PREFIX = " -i "
ROUTING_TABLE_PREFIX = " -r "
UPDATE_PREFIX = " -u "
CRASH_PREFIX = " -c "
SENDFILE_PREFIX = " -f "
LAST_PACKET_PREFIX = " -l "
PENULTIMATE_PREFIX = " -p "
SENDFILE_STATS_PREFIX = " -s "

def build_topology_file(num_of_routers, seed, shape):
    topology_file = open("./topology_file", "w")
    topology_file.write(str(num_of_routers) + '\n')

    random.seed(seed)

    for num in range(0, num_of_routers):
        line = str(num + 1) + " "
        line += IP_STEM + str(2 + num) + " "
        line += str(40000 + num) + " "
        line += str(2000 + num) + " "
        line += str(1000 + num) + "\n"
        topology_file.write(line)
        sys.stdout.write(line)

    for num in range(1, num_of_routers):
        line = str(num) + " "
        line += str(num + 1) + " "
        line += str(random.randint(1, 10)) + "\n"
        topology_file.write(line)
        sys.stdout.write(line)

    if shape == "ring":
        line = "1 " + str(num_of_routers) + " "
        line += str(random.randint(1, 10)) + '\n'
        topology_file.write(line)
        sys.stdout.write(line)

#def send_cmd(CMD, ROUTER_ID=" 1 ", UPDATE_INTERVAL=" 3 "):


if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument("-N", "--router_count", type=int, required=True,
                        help="Enter the number of routers to test (up to 5)")

    parser.add_argument("-S", "--topology_seed", type=int, required=True,
                        help="Enter seed for topology file delay")

    parser.add_argument("-a", "--author", type=str, choices=["1", "2", "3", "4", "5"],
                        help="Enter the target router id", default=False)

    parser.add_argument("-i", "--init", type=str,
                        help="Enter update interval", default=False)

    parser.add_argument("-r", "--routing_table", type=str, choices=["1", "2", "3", "4", "5"],
                        help="Enter the target router id")

    parser.add_argument("-u", "--update", type=str, nargs=3,
                        help="format: [router_one_id] [router_two_id] [new_cost]")

    parser.add_argument("-c", "--crash", type=str, choices=["1", "2", "3", "4", "5"],
                        help="Enter the target router id")

    parser.add_argument("-s", "--sendfile", type=str, nargs=6,
                        help="format: [src_id] [dest_id] [ttl] [transfer_id] [seqnum] [filename]")

    parser.add_argument("-ss", "--sendfile_stats", type=str, nargs=2,
                        help="sendfile_stats: format: [target_router_id] [transfer_id]")

    parser.add_argument("-l", "--last_packet", type=str, nargs=1,
                        help="last_packet: format: [target_router_id]")

    parser.add_argument("-p", "--penultimate", type=str, nargs=1,
                        help="penultimate_packet: format: [target_router_id]")

    parser.add_argument("-t", "--target_router", type=str, choices=["1", "2", "3", "4", "5"],
                        help="Enter the target router id (1 to 5")

    parser.add_argument("--shape", type=str, choices=["line", "ring", "triangle"],
                        help="Choose line or ring for topology layout")

    args = parser.parse_args()

    if args.shape == "triangle":
        TOPOLOGY_FILE = "./topology_race_to_infinity"
    else:
        build_topology_file(args.router_count, args.topology_seed, args.shape)

    subprocess.call("rm -R ./output", shell=True)
    subprocess.call("mkdir ./output", shell=True)

    CMD = CONTROLLER + " -t " + TOPOLOGY_FILE + OUTPUT_RULES

    if args.author:
        CMD += AUTHOR_PREFIX + args.author + " &"
        print CMD
        subprocess.call(CMD, shell=True)
    elif args.init:
        CMD += INIT_PREFIX + args.init + " &"
        print CMD
        subprocess.call(CMD, shell=True)
    elif args.routing_table:
        CMD += ROUTING_TABLE_PREFIX + args.routing_table + " &"
        print CMD
        subprocess.call(CMD, shell=True)
    elif args.update:
        CMD += UPDATE_PREFIX + " ".join(args.update) + " &"
        print CMD
        subprocess.call(CMD, shell=True)
    elif args.crash:
        CMD += CRASH_PREFIX + args.crash + " &"
        print CMD
        subprocess.call(CMD, shell=True)
    elif args.sendfile:
        CMD += SENDFILE_PREFIX + " ".join(args.sendfile) + " &"
        print CMD
        subprocess.call(CMD, shell=True)
    elif args.last_packet:
        CMD += LAST_PACKET_PREFIX + " ".join(args.last_packet) + " &"
        print CMD
        subprocess.call(CMD, shell=True)
    elif args.penultimate:
        CMD += PENULTIMATE_PREFIX + " ".join(args.penultimate) + " &"
        print CMD
        subprocess.call(CMD, shell=True)
    elif args.sendfile_stats:
        CMD += SENDFILE_STATS_PREFIX + " ".join(args.sendfile_stats) + " &"
        print CMD
        subprocess.call(CMD, shell=True)

    exit(0)
