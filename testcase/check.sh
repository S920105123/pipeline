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
echo UP is golden, DOWN is mine.
echo ----------snapshot-----------
diff ../golden/snapshot.rpt ../simulator/snapshot.rpt
echo ------------error------------
diff ../golden/error_dump.rpt ../simulator/error_dump.rpt
