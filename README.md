# Meta-Prism
## Introduction
Meta-Prism aims to conduct quick query in large-scale metagenomics database. The method is based on three computational techniques: dual-indexing approach for sample sub-grouping, refined scoring function that could scrutinize the minute differences among samples, and parallel computation based on GPU computing.
## Requirement
We successfully compiled and tested the executable files by CUDA 10.1 for GPU code, and GCC 4.8.5 for CPU code, running on CentOS 7.6. It is highly possible to run on server with Linux and GCC support C++11 and CUDA 10 or higher.
## Install and Build
Download the source code:
`git clone https://github.com/HUST-NingKang-Lab/metaPrism.git`
Build index manage tools:
`make tool`
Build cpu query tool:
`make cpu`
Build gpu query tool (::**highly recommend**::):
`make gpu`
## Structure of sample data
Meta-Prism now accepts the metagenomic samples analysis result pre-computed by Parallel-Meta.
## Usage
### Make index:

make index [options] value
```
option:
-i samples input path
-o index output path
-n database name
-h help
```
E.g. to make a new database named "DB1" using all samples locatedat /opt/data/database1, and save the database at ./DB1.

`./make_index -n DB1 -i /opt/data/database1 -o ./DB1`
### Add index:
add_index [options] value
```
option:
-i samples input path
-d database index name
-o index output path
-h help
```
### Query by CPU:

query_index [option] value
```
Option:
-d database index name
-i query sample path
-l query sample list
-n hit numver, default is 5
-e exhaustive search
-s whether use Difference-based scoring funcion, T or F, default is F
-f whether use alpha diversity filter,T or F, default is T
-b biome name
-t cpu core numver, default is 1
-h help
```
E.g. to find the top 10 hits of a sample from marine located /opt/data/query_samples/marine/sample_1 in database DB1 using 8 CPU cores

`./query_index -d ./DB1 -i /opt/data/query_samples/marine/query_samples/sample_1/classification.txt -n 10 -t 8`

E.g. to find 10 samples' top 10 hit respectively, using 8 CPU cores, and save the result in ./result. These 10 samples' path are in a plain-text file "list.txt" with each file in a row like:

/opt/data/query/marine/sample_0/classificiation.txt

/opt/data/query/plant/sample_1/classificiation.txt

/opt/data/query/human_gut/sample_2/classificiation.txt

/opt/data/query/marine/sample_3/classificiation.txt

...

/opt/data/query/lake/sample_9/classificiation.txt

The command line is

`$query_index -d ./DB1 -l list.txt -n 10 -t 8 > ./results`
### Query by GPU:
gpu_query_index [option] value
```
Option:
-i query sample path
-l query sample list
-n hit numver, default is 5
-e exhaustive search
-f whether use alpha diversity filter,T or F, default is T
-h help
```
E.g. to find 10 samples' top 10 hit respectively, and save the result in ./result.These 10 samples' path are in a plain-text file "list.txt" with each file in a row like:

/opt/data/query/marine/sample_0/classificiation.txt

/opt/data/query/plant/sample_1/classificiation.txt

/opt/data/query/human_gut/sample_2/classificiation.txt

/opt/data/query/marine/sample_3/classificiation.txt

...

/opt/data/query/lake/sample_9/classificiation.txt

The command line is

`./gpu_query_index -d ./DB1 -l list.txt -n 10 > ./results`
## Notice
When making database index, the samples input path must be the absolute path containing metagenomic samples analysis results by Parallel-META.

When quering, the query samples should also be pre-computed by Parallel-META and input file is the "classification.txt" file