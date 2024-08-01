#!/bin/bash


# Change this to your netid
netid=jts200006

CONFIGNAME="testConfig.txt"
# Root directory of your project
PROJDIR=/home/justin/Desktop/code/CS6380/LayeredBfs
# Directory where the config file is located on your local system
CONFIGLOCAL=$PROJDIR/${CONFIGNAME}
CONFIGDOCKER=/home/010/j/jt/jts200006/cs6380/LayeredBfs/${CONFIGNAME}

n=0

cat $CONFIGLOCAL | sed -e "s/#.*//" | sed -e "/^\s*$/d" |
(
    read i
    echo $i
    while [[ $n -lt $i ]]
    do
    	read line
        host=$( echo $line | awk '{ print $2 }' )

        echo $host
        gnome-terminal -- ssh $netid@$host "killall -u ${netid}" &

        n=$(( n + 1 ))
    done
   
)


echo "Cleanup complete"
