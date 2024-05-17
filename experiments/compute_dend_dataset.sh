dataset=$1
eps=$2
build_index=$3
R=32
L=64
a=1.2
dt='float'
base_path=/ssd2/kishen/centroidHAC/${dataset}/
# base_path=/home/kishen/CentroidHAC/data1/${dataset}/

if [ $build_index -eq 1 ]
then
    bazel run //src:centroid_main -- -R $R -L $L -alpha $a -data_type $dt -base_path ${base_path}${dataset}.txt -test -graph_outfile ${base_path}${dataset}_32_64 -dendrogram_outfile ${base_path}dend/${dataset}_dend_centroid_${eps}.txt -eps ${eps}
else
    bazel run //src:centroid_main -- -R $R -L $L -alpha $a -data_type $dt -base_path ${base_path}${dataset}.txt -test -graph_path ${base_path}${dataset}_32_64 -dendrogram_outfile ${base_path}dend/${dataset}_dend_centroid_${eps}.txt -eps ${eps}
fi