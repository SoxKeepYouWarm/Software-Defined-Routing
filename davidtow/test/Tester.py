import subprocess
import argparse
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

AUTHOR_COMMAND = " -a " + ROUTER_ID
INIT_COMMAND = " -i " + ROUTING_UPDATE_INTERVAL
ROUTING_TABLE_COMMAND = " -r " + ROUTER_ID
CRASH_COMMAND = " -c " + ROUTER_ID

def build_topology_file(num_of_routers):
    topology_file = open("./topology_file", "w")
    topology_file.write(str(num_of_routers) + '\n')
    for num in range(0, num_of_routers):
        print num
        topology_file.write(str(num + 1) + " ")
        topology_file.write(IP_STEM + str(2 + num) + " ")
        topology_file.write(str(40000 + num) + " ")
        topology_file.write(str(1000) + " ")
        topology_file.write(str(2000))
        topology_file.write("\n")

    for num in range(1, num_of_routers):
        topology_file.write(str(num) + " " + str(num + 1) + " 10\n")

def send_cmd(CMD, ROUTER_ID="1", UPDATE_INTERVAL="3"):


if __name__ == '__main__':
    print "running"

    parser = argparse.ArgumentParser()
    parser.add_argument("router_count", type=int, help="Enter the number of routers to test (up to 5)")

    parser.add_argument("-c", "--command", type=str, choices=["a", "i", "r", "c"],
                        help="Enter the command type: a (author), i (init), r (routing table), c (crash)")

    parser.add_argument("-t", "--target_router", type=str, choices=["1", "2", "3", "4", "5"],
                        help="Enter the target router id (1 to 5")

    args = parser.parse_args()

    build_topology_file(args.router_count)

    subprocess.call("rm -R ./output", shell=True)
    subprocess.call("mkdir ./output", shell=True)

    if args.command == "a":
        print "got a"
        #subprocess.call(CONTROLLER + " -t " + TOPOLOGY_FILE + OUTPUT_RULES + AUTHOR_COMMAND + " &", shell=True)
    elif args.command == "i":
        print "got i"
        #subprocess.call(CONTROLLER + " -t " + TOPOLOGY_FILE + OUTPUT_RULES + INIT_COMMAND + " &", shell=True)
    elif args.command == "r":
        print "got r"
        #subprocess.call(CONTROLLER + " -t " + TOPOLOGY_FILE + OUTPUT_RULES + ROUTING_TABLE_COMMAND + " &", shell=True)
    elif args.command == "c":
        print "got c"
        #subprocess.call(CONTROLLER + " -t " + TOPOLOGY_FILE + OUTPUT_RULES + CRASH_COMMAND + " &", shell=True)


    #time.sleep(1)
    #subprocess.call(CONTROLLER + " -t " + TOPOLOGY_FILE + OUTPUT_RULES + INIT_COMMAND + " &", shell=True)
    #time.sleep(1)
    #subprocess.call(CONTROLLER + " -t " + TOPOLOGY_FILE + OUTPUT_RULES + ROUTING_TABLE_COMMAND + " &", shell=True)


    exit(0)
