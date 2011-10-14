#!/bin/bash

echo -n "Running this script will delete data files from previous run. Continue? [Y/n]"
read run
if [[ $run != "" && $run != "Y" && $run != "y" ]]; then exit; fi

if [ $# -ne 1 ]; then
    echo "Usage: $0 <program>"
    exit
fi

strategy="0 1 2 3 4"
iterations="20000 40000 60000 80000 100000"
runs=`seq 5`
program="$1"

rm -f make_output

for s in $strategy; do
    echo "Strategy $s"
    
    # Use system malloc if strategy is 0
    if [ $s -eq 0 ]; then
        make clean all MALLOC= &>> make_output
    else
        make clean all STRATEGY=$s &>> make_output
    fi

    # make sure make did not fail
    if [ $? -ne 0 ]; then
        echo "make failed"
        exit $?
    fi

    dat_file="`basename ${program}`.$s.dat"
    rm -f $dat_file

    echo "# Iterations\tMemory\tTime" >> $dat_file

    for i in $iterations; do

        # Run the test a couple of times.
        for j in $runs; do
            output=$(/usr/bin/time --format="Time: %e\n" $program $i 2>&1)
            m[$j]=$(echo "$output" | grep -o "Memory usage: [0-9]* b" | awk '{print $3}')
            t[$j]=$(echo "$output" | grep -o "Time: [0-9]*.[0-9]*" | awk '{print $2}')
        done

        m_median=$(echo ${m[1]} ${m[2]} ${m[3]} ${m[4]} ${m[5]} | tr " " "\n" | sort | head -n 3 | tail -n 1)
        t_median=$(echo ${t[1]} ${t[2]} ${t[3]} ${t[4]} ${t[5]} | tr " " "\n" | sort | head -n 3 | tail -n 1)

        printf "%d\t%10d\t%f\n" $i $m_median $t_median | tee -a $dat_file
    done
done

