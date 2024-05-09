dataset=$1
eps=$2
R=32
L=64
a=1.2
dt='float'
base_path=/ssd2/kishen/centroidHAC/${dataset}/

bazel run //src:centroid_main -- -R $R -L $L -alpha $a -data_type $dt -base_path ${base_path}${dataset}.txt -test -graph_path ${base_path}${dataset}_32_64 -dendrogram_outfile ${base_path}${dataset}_dend_${eps}.txt -eps ${eps}