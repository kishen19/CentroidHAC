dataset=$1
dt='float'
# base_path=/ssd2/kishen/centroidHAC/${dataset}/
base_path=/home/kishen/CentroidHAC/data1/${dataset}/

bazel run //src:centroid_exact_main -- -data_type $dt -base_path ${base_path}${dataset}.txt -test -dendrogram_outfile ${base_path}${dataset}_dend_exact.txt