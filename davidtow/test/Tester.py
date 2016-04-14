import subprocess
import time

CONTROLLER = "../../controller/controller"
TOPOLOGY_EXAMPLE = "./topology_demo"
TOPOLOGY_FILE = "./topology_file"

APPLICATION_OUTPUT = "./output/application_output.txt"
APPLICATION_ERROR = "./output/application_error.txt"

CONTROLLER_OUTPUT = "./output/controller_output.txt"
CONTROLLER_ERROR = "./output/controller_error.txt"

IP_ADDRESS = "127.0.0.1"

def build_topology_file(num_of_routers):
    topology_file = open("./topology_file", "w")
    topology_file.write(str(num_of_routers) + '\n')
    for num in range(0, num_of_routers):
        print num
        topology_file.write(str(num + 1) + " ")
        topology_file.write(IP_ADDRESS + " ")
        topology_file.write(str(40000 + num) + " ")
        topology_file.write(str(1000) + " ")
        topology_file.write(str(2000))
        topology_file.write("\n")

    for num in range(0, num_of_routers - 1):
        topology_file.write(str(num) + " " + str(num + 1) + " 10\n")

if __name__ == '__main__':
    print "running"

    build_topology_file(1)

    #subprocess.call("pkill assignment3", shell=True)
    #subprocess.call("../assignment3 40000 1> " + APPLICATION_OUTPUT + " 2> " + APPLICATION_ERROR + " &", shell=True)

    time.sleep(1)

    subprocess.call(CONTROLLER + " -t " + TOPOLOGY_FILE + " 1> " + CONTROLLER_OUTPUT + " 2> " + CONTROLLER_ERROR + " &", shell=True)



    exit(0)
