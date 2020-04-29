#!/bin/sh

perf_path=profile_opt
mkdir -p ${perf_path}
for size in 32768 65536 131072 1000000 10000000 20000000 30000000 33554432
do
	for tmax in 10 100 1000 10000 50000  
	do
		echo "${size} ${tmax}"
		echo "${size} ${tmax}" > size.txt
		for run_time in $(seq 1 2)
		do 
			perf_filename=${perf_path}/profile_n${size}_t${tmax}_${run_time}.txt
			perf stat -e branch-instructions,branch-misses,bus-cycles,cache-misses,cache-references,cpu-cycles,instructions,ref-cycles ./main_1d_opt  2>&1 | tee ${perf_filename}
		done
	done
done
