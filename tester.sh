#!/bin/bash

time_start=$(date +%s%N)
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m'

for dir in $(ls ./open)
do
	cp ./open/${dir}/dimage.bin ./simulator/dimage.bin
	cp ./open/${dir}/iimage.bin ./simulator/iimage.bin
	cp ./open/${dir}/dimage.bin ./golden/dimage.bin
        cp ./open/${dir}/iimage.bin ./golden/iimage.bin
	cd ./golden
	./pipeline > tmp
	if [ "$(cat tmp)" != "" ]; then
		cd ..;	
		echo -e "   Testcase: ${dir}\t- ${YELLOW}[Illegal]${NC}... ($(cat golden/tmp))"
		continue;
	fi
	cd ../simulator
	./pipeline
	cd ..
	diff ./golden/snapshot.rpt ./simulator/snapshot.rpt > diff_snapshot.tmp
	diff ./golden/error_dump.rpt ./simulator/error_dump.rpt > diff_error.tmp
	if [ "$(cat diff_snapshot.tmp)" = "" -a "$(cat diff_error.tmp)" = "" ]; then
		echo -e "   Testcase: ${dir}\t- ${GREEN}[Accecpted]${NC}..."
	else
		echo -e "   Testcase: ${dir}\t- ${RED}[Wrong Answer]${NC}..."
		echo "${dir}" > who
		break
	fi
done

time_end=$(date +%s%N)
echo "   Total runtime: $(((time_end-time_start)/1000000))ms"
