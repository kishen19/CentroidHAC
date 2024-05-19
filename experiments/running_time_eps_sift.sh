rounds=$1
method=$2
R=32
L=64
a=1.2
dt='float'
dataset=/home/kishen/CentroidHAC/data1/sift/slices/sift_1000000.fbin
epss=(0.01 0.02 0.04 0.08 0.16 0.32 0.64 1.28)

for eps in "${epss[@]}"
do
    echo "Running for sift_${eps}"
    bazel run //src:${method}_main -- -R $R -L $L -alpha $a -data_type $dt -base_path ${dataset} -eps ${eps} -rounds ${rounds}
done