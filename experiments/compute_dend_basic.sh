method=$1 # centroid or centroid_bucket or centroid_exact
eps=$2
R=32
L=64
a=1.2
dt='float'
base_path='/ssd2/kishen/centroidHAC/basic/'
# base_path='/home/kishen/CentroidHAC/data1/basic/'
datasets=('iris' 'wine' 'breast_cancer' 'digits' 'faces')

for dataset in "${datasets[@]}"
do
    if [ $method == 'centroid_exact' ] 
    then 
        echo "Running for ${dataset} ${method}"
        bazel run //src:${method}_main -- -data_type $dt -base_path ${base_path}${dataset}/${dataset}.txt -test -dendrogram_outfile ${base_path}${dataset}/dend/${dataset}_dend_${method}.txt
    else
        echo "Running for ${dataset} ${method} ${eps}"
        bazel run //src:${method}_main -- -R $R -L $L -alpha $a -data_type $dt -base_path ${base_path}${dataset}/${dataset}.txt -test -dendrogram_outfile ${base_path}${dataset}/dend/${dataset}_dend_${method}_${eps}.txt -eps ${eps}
    fi
done