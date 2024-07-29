#!/bin/bash

# Change this to your netid
netid=jts200006

# Root directory of your project
PROJDIR=/home/010/j/jt/jts200006/cs6380/SyncGHS

# Directory where the config file is located on your local system
CONFIGLOCAL=/home/justin/Desktop/code/CS6380/SyncGHS/testConfig.txt

CONFIGDOCKER=/home/010/j/jt/jts200006/cs6380/SyncGHS/testConfig.txt

# Directory your java classes are in
BINDIR=$PROJDIR/build

# Your main project class
PROG=SyncGhs

n=0

cat $CONFIGLOCAL | sed -e "s/#.*//" | sed -e "/^\s*$/d" |
(
    read i
    echo $i
    while [[ $n -lt $i ]]
    do
    	read line
    	p=$( echo $line | awk '{ print $1 }' )
        host=$( echo $line | awk '{ print $2 }' )
	
	    gnome-terminal -- ssh $netid@$host "cd ${BINDIR}/..;/${BINDIR}/${PROG} ${PROJDIR}/testConfig.txt ${p};" "exec bash" &

        n=$(( n + 1 ))
    done
)
