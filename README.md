# Watch-On-Windows

Watch command is frequently used on linux while there is no such things on windows. To use watch command on windows, one might need to install Cygwin or something similar, which is not acceptable for some users like me. To use this command on windows, I wrote a naive watch command.

## How to compile this
Just run the following command. (GCC is required, or you can us visual studio)
```bat
g++ -Wall -O2 -static watch.cpp -o watch.exe
```

## How to use
Only the basic functionality is implenmented. The command line arguments are as follows:
```bat
watch <interval> <your exe>
```
Here are some examples:
```bat
watch 0.1 nvidia-smi
watch 1 nvidia-smi -q -d CLOCK
```