dataset=$1
eps=$2
rounds=$3
R=32
L=64
a=1.2
dt='float'
base_path=/ssd2/kishen/centroidHAC/${dataset}/

bazel run //src:centroid_main -- -R $R -L $L -alpha $a -data_type $dt -base_path ${base_path}${dataset}.txt -test -eps ${eps} -rounds $rounds