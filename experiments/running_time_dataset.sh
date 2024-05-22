dataset=$1
method=$2
eps=$3
rounds=$4
R=32
L=64
a=1.2
dt='float'
# base_path=/ssd2/kishen/centroidHAC/${dataset}/
base_path=/home/kishen/CentroidHAC/data1/${dataset}/

bazel run //src:${method}_main -- -R $R -L $L -alpha $a -data_type $dt -base_path ${base_path}${dataset}.txt -test -eps ${eps} -rounds $rounds