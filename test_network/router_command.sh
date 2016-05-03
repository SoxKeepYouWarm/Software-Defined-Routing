#!bin/bash

#ldconfig -p | grep libstd
#strings /usr/lib/x86_64-linux-gnu/libstdc++.so.6 | grep LIBCXX
#../davidtow/assignment3 ${CONTROL_PORT}
#tcpdump net ${MY_IP} -K -nvXSs 0 & 
valgrind --track-origins=yes --leak-check=full --log-file=${VALGRIND_LOG} ../davidtow/assignment3 ${CONTROL_PORT}
