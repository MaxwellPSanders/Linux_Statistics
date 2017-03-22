This program can only be compiled and run on a linux system since linux is the only operating system with the proc file structure. It can be compiled on the command line using gcc.

If run without any parameters, it will print out:
The processor type
Kernel Version
The amount of memory configured into this computer
Amount of time since the system was last booted

If run with the parameters of two ints
./proc 2 60

The first parameter is how many seconds apart the program will parse the info from the proc files and the 2 parameter is how many seconds apart it will print out the average.

Given the example above it will print out an average consisting of 30 data points every 60 seconds.

The data it will output will be:
The percentage of time the processor(s) spend in user mode 
The percentage of time the processor(s) spend in system mode
The percentage of time the processor(s) are idle
The amount and percentage of available (or free) memory
The rate (number of sectors per second) of disk read/write in the system 
The rate (number per second) of context switches in the kernel 
The rate (number per second) of process creations in the system 

It will keep running until terminated.
