./i_generator
./d_generator
cp dimage.bin ../golden/dimage.bin
cp iimage.bin ../golden/iimage.bin
cp dimage.bin ../simulator/dimage.bin
cp iimage.bin ../simulator/iimage.bin

cd ../golden
./pipeline
cd ../simulator
./pipeline
cd ../testcase

echo -------------diff-------------
diff -c ../golden/snapshot.rpt ../simulator/snapshot.rpt
diff -c ../golden/error_dump.rpt ../simulator/error_dump.rpt
