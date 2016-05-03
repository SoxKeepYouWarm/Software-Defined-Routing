#!/bin/bash

export control_port_one="40000"
export data_port_one="1000"
export router_port_one="2000"

export control_port_two="40001"
export data_port_two="1001"
export router_port_two="2001"

export control_port_three="40002"
export data_port_three="1002"
export router_port_three="2002"

export control_port_four="40003"
export data_port_four="1003"
export router_port_four="2003"

export control_port_five="40004"
export data_port_five="1004"
export router_port_five="2004"

export valgrind_log_one="log/valgrind_log_one.log"
export valgrind_log_two="log/valgrind_log_two.log"
export valgrind_log_three="log/valgrind_log_three.log"
export valgrind_log_four="log/valgrind_log_four.log"
export valgrind_log_five="log/valgrind_log_five.log"

export ip_one="172.20.0.2"
export ip_two="172.20.0.3"
export ip_three="172.20.0.4"
export ip_four="172.20.0.5"
export ip_five="172.20.0.6"

if [ $# -eq 1 ]; then

    if [ "$1" = "build" ]; then
        ./router/docker build -t router --no-cache .;
    else
        usage;
    fi

fi

if [ $# -eq 0 ]; then

    docker-compose up;

fi




