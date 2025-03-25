mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
cd ..
mv build/CMakeFiles/PSXecute-BOF.dir/main.c.o ./psxecute.o
echo "BOF saved at psxecute.o"