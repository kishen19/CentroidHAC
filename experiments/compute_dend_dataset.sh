dataset=$1
method=$2 # centroid or centroid_bucket or centroid_exact
eps=$3
R=32
L=64
a=1.2
dt='float'
base_path=/ssd2/kishen/centroidHAC/${dataset}/
# base_path=/home/kishen/CentroidHAC/data1/${dataset}/

if [ $method == 'centroid_exact' ] 
then 
    bazel run //src:${method}_main -- -data_type $dt -base_path ${base_path}${dataset}.txt -test -graph_path ${base_path}${dataset}_32_64 -dendrogram_outfile ${base_path}dend/${dataset}_dend_${method}.txt
else
    bazel run //src:${method}_main -- -R $R -L $L -alpha $a -data_type $dt -base_path ${base_path}${dataset}.txt -test -graph_path ${base_path}${dataset}_32_64 -dendrogram_outfile ${base_path}dend/${dataset}_dend_${method}_${eps}.txt -eps ${eps}
fi
