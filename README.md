# Computer Architecture

Projects of the graduate class Computer Architecture taught by Professor Yan Solihin.

## Cache simulation

This project simulates a 2 level cache design with arbitrary associativity, blocksize and size. 3 replacement policies are coded : LRU, Pseudo-LRU and Optimal policy.

The inclusion policy can be non-inclusive or inclusive.

The write policy is WBWA (write back, write allocate)

### make it work

The makefile compile the necessary files.
The cache simulator takes 8 arguments in the following way:

```
<BLOCKSIZE> <L1_SIZE> <L1_ASSOC> <L2_SIZE> <L2_ASSOC> <REPLACEMENT_POLICY> <INCLUSION_PROPERTY> <trace_file>
```

BLOCKSIZE: Positive integer. Block size in bytes. (Same block size for all caches in the memory hierarchy.)\
L1_SIZE: Positive integer. L1 cache size in bytes.\
L1_ASSOC: Positive integer. L1 set-associativity (1 is direct-mapped).\
L2_SIZE: Positive integer. L2 cache size in bytes. L2_SIZE = 0 signifies that there is no L2 cache.
L2_ASSOC: Positive integer. L2 set-associativity (1 is direct-mapped).\
REPLACEMENT_POLICY: Positive integer. 0 for LRU, 1 for PLRU, 2 for Optimal.\
INCLUSION_PROPERTY: Positive integer. 0 for non-inclusive, 1 for inclusive.\
trace_file: Character string. Full name of trace file including any extensions.

example :
```
make

sim_cache 32 8192 4 262144 8 0 0 gcc_trace_cache.txt
```

the input should be of the following format:\
r|w hex_address

“r” (read) indicates a load and “w” (write) indicates a store

an example trace file is given in the traces folder


## Branch prediction

This project simulates the following branch productor : Smith n-bit counter predictor, GShare Branch Predictor and Hybrid Branch Predictor

### make it work

The makefile compile the necessary files.

To simulate a smith n-bit counter predictor the following arguments are needed:
```
smith <B> <tracefile>
```
B: number of counter bits


To simulate a bimodal predictor the following arguments are needed:
```
bimodal <M2> <tracefile>
```
M2: number of PC bits to index the bimodal table.


To simulate a gshare predictor the following arguments are needed:
```
gshare <M1> <N> <tracefile>
```
M1: number of PC bits to index gshare table\
N: number of global branch history register bits to index gshare table

To simulate a hybrid predictor the following arguments are needed:
```
hybrid <K> <M1> <N> <M2> <tracefile>
```
K: number of PC bits to index chooser table\
M1: number of PC bits to index gshare table\
N: number of global branch history register bits to index gshare table\
M2: number of PC bits used to index bimodal table\
tracefile: Character string. Full name of trace file including any extensions.

example :
```
make

./sim hybrid 8 14 10 5 gcc_trace_branch.txt
```

the input should be of the following format:\
hex_branch_PC t|n

hex_branch_PC is the address of the branch instruction in memory.\
“t” indicates the branch is actually taken and “n” indicates the branch is actually not-taken.

## Dynamic Instruction Scheduling

This project is a simulator for an out-of-order superscalar processor based on Tomasulo’s algorithm that fetches, dispatches, and issues N instructions per cycle. Only the dynamic scheduling mechanism will be modeled in detail, i.e., perfect caches and perfect branch prediction are assumed.

### make it work

The makefile compile the necessary files.
The cache simulator takes 3 argument in the following way:

```
<S> <N> <tracefile>
```

S: Scheduling Queue size\
N: peak fetch and dispatch rate (issue rate will be up to N+1)\
tracefile: Full name of trace file including any extensions.

```
make

./sim 2 8 gcc_trace_scheduling.txt
```

the input should be of the following format:

PC operation_type <dest reg #> <src1 reg #> <src2 reg #>

• PC is the program counter of the instruction (in hex)\
• operation_type is either “0”, “1”, or “2”\
• <dest reg#> is the destination register of the instruction. If it is -1, then the instruction does
not have a destination register. Otherwise, it is between 0 and 127\
• <src1 reg #> is the first source register of the instruction. If it is -1, then the instruction does
not have a first source register. Otherwise, it is between 0 and 127\
• <src2 reg #> is the second source register of the instruction. If it is -1, then the instruction
does not have a second source register. Otherwise, it is between 0 and 127.








