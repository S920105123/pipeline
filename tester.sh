#!/bin/bash

time_start=$(date +%s%N)

for dir in $(ls ./open)
do
	cp ./open/${dir}/dimage.bin ./simulator/dimage.bin
	cp ./open/${dir}/iimage.bin ./simulator/iimage.bin
	cp ./open/${dir}/dimage.bin ./golden/dimage.bin
        cp ./open/${dir}/iimage.bin ./golden/iimage.bin
	cd ./simulator
	./pipeline
	cd ../golden
	./pipeline
	cd ..
	diff ./golden/snapshot.rpt ./simulator/snapshot.rpt > diff_snapshot.tmp
	diff ./golden/error_dump.rpt ./simulator/error_dump.rpt > diff_error.tmp
	if [ "$(cat diff_snapshot.tmp)" = "" -a "$(cat diff_error.tmp)" = "" ]; then
		echo -e "   Testcase: ${dir}\t- [Accecpted]..."
	else
		echo -e "   Testcase: ${dir}\t- [Wrong Answer]..."
		echo "${dir}" > who
		break
	fi
done

time_end=$(date +%s%N)
echo "   Total runtime: $(((time_end-time_start)/1000000))ms"
