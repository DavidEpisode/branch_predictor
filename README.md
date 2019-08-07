# branch_predictor
This is the project for UCSD CSE240A Computer Architecture. In this project we implemented three kinds of branch predictors. 

## Traces
These predictors will make predictions based on traces of real programs.  Each line in the trace file contains the address of a branch in hex as well as its outcome (Not Taken = 0, Taken = 1):

```
<Address> <Outcome>
Sample Trace from int_1:

0x40d7f9 0
0x40d81e 1
0x40d7f9 1
0x40d81e 0
```

We provide test traces to you to aid in testing your project but we strongly suggest that you create your own custom traces to use for debugging.

## Running your predictor

In order to build your predictor you simply need to run `make` in the src/ directory of the project.  You can then run the program on an uncompressed trace as follows:   

`./predictor <options> [<trace>]`

If no trace file is provided then the predictor will read in input from STDIN. Some of the traces we provided are rather large when uncompressed so we have distributed them compressed with bzip2 (included in the Docker image).  If you want to run your predictor on a compressed trace, then you can do so by doing the following:

`bunzip2 -kc trace.bz2 | ./predictor <options>`

In either case the `<options>` that can be used to change the type of predictor
being run are as follows:

```
  --help       Print usage message
  --verbose    Outputs all predictions made by your
               mechanism. Will be used for correctness
               grading.
  --<type>     Branch prediction scheme. Available
               types are:
        static
        gshare:<# ghistory>
        tournament:<# ghistory>:<# lhistory>:<# index>
        custom
```
An example of running a gshare predictor with 10 bits of history would be:   

`bunzip2 -kc ../traces/int1_bz2 | ./predictor --gshare:10`


