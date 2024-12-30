cd build
cmake .
make
./scuffedtorio
valgrind --error-limit=no --suppressions=./sdl.supp --gen-suppressions=all --log-file=errors.log ./scuffedtorio