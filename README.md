Meta-Prism
===========
Meta-Prism aims to conduct quick query in large-scale metagenomics database. The method is based on three computational techniques: dual-indexing approach for sample sub-grouping, refined scoring function that could scrutinize the minute differences among samples, and parallel computation based on GPU computing. 


Environment variable
===========
Export MetaPrism = The root path of MetaPrism 

Export PATH = “$PATH: The bin path of MetaPrism”

Usage
===========
Make index:

make index [options] value
	
	option:
	-i samples input path
	-o index output path
	-n database name
	-h help

E.g. to make a new database named "DB1" using all samples locatedat /opt/data/database1, and save the database at ./DB1.

$make_index -n DB1 -i /opt/data/database1 -o ./DB1





add_index [options] value
	
	option:
	-i samples input path
	-d database index name
	-o index output path
	-h help


Query:
	
query_index [option] value
	
	Option:
	-d database index name
	-i query sample path
	-l query sample list
	-o result output file, default is to output on screen
	-n hit numver, default is 5
	-e exhaustive search
	-s whether use Difference-based scoring funcion, T or F, default is F
	-f whether use alpha diversity filter,T or F, default is T
	-b biome name
	-t cpu core numver, default is 1
	-h help

E.g. to find the top 10 hits of a sample from marine located /opt/data/query_samples/marine/sample_1 in database DB1 using 8 CPU cores

$query_index -d ./DB1 -i /opt/data/query_samples/marine/query_samples/sample_1/classification.txt -n 10 -t 8

E.g. to find 10 samples' top 10 hit respectively, using 8 CPU cores, and save the result in ./result. These 10 samples' path are in a plain-text file "list.txt" with each file in a row like:
/opt/data/query/marine/sample_0/classificiation.txt
/opt/data/query/plant/sample_1/classificiation.txt
/opt/data/query/human_gut/sample_2/classificiation.txt
/opt/data/query/marine/sample_3/classificiation.txt
...
/opt/data/query/lake/sample_9/classificiation.txt


The command line is 

$query_index -d ./DB1 -l list.txt -n 10 -t 8 -o ./results






gpu_query_index [option] value
	
	Option:
	-d database index name
	-i query sample path
	-l query sample list
	-o result output file, default is to output on screen
	-n hit numver, default is 5
	-e exhaustive search
	-s whether use Difference-based scoring funcion, T or F, default is F
	-f whether use alpha diversity filter,T or F, default is T
	-m gpu id, such as 1,2
	-h help



E.g. to find 10 samples' top 10 hit respectively, using gpu device with id 1, and save the result in ./result.These 10 samples' path are in a plain-text file "list.txt" with each file in a row like:

/opt/data/query/marine/sample_0/classificiation.txt
/opt/data/query/plant/sample_1/classificiation.txt
/opt/data/query/human_gut/sample_2/classificiation.txt
/opt/data/query/marine/sample_3/classificiation.txt
...
/opt/data/query/lake/sample_9/classificiation.txt

The command line is 
$gpu_query_index -d ./DB1 -l list.txt -n 10 -m 1 -o ./results



Notice
===========

Meta-Prism now accepts the metagenomic samples analysis result pre-computed by Parallel-Meta.
When making database index, the samples input path must be the absolute path containing metagenomic samples analysis results by Parallel-META.
When quering, the query samples should also be pre-computed by Parallel-META and input file is the "classification.txt" file
