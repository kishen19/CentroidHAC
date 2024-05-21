eps=$1
rounds=$2
method=$3
R=32
L=64
a=1.2
dt='float'
base_path=/home/kishen/CentroidHAC/data1/sift/slices/
sizes=(32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1000000)

for size in "${sizes[@]}"
do
    echo "Running for sift_${size}"
    bazel run //src:${method}_main -- -R $R -L $L -alpha $a -data_type $dt -base_path ${base_path}sift_${size}.fbin -eps ${eps} -rounds $rounds
done