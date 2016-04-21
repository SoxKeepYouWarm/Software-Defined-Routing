#!bin/bash

tcpdump net ${MY_IP} -nvXSs 0 & 
../davidtow/assignment3 ${CONTROL_PORT}