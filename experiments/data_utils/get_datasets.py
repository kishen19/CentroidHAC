from sklearn import datasets
import numpy as np

# base_path = "/ssd2/kishen/centroidHAC/"
base_path = "/home/kishen/CentroidHAC/data1/"

def get_iris_dataset(save = True):
    iris = datasets.load_iris()
    data, labels = iris.data, iris.target
    if save:
        np.savetxt(base_path + "basic/iris/iris.txt", data, fmt="%.5f", delimiter=" ", header = "%i %i"%(data.shape[0],data.shape[1]),comments='')
        np.savetxt(base_path + "basic/iris/iris.gt", labels, fmt="%i")
    return data, labels


def get_digits_dataset(save = True):
    digits = datasets.load_digits()
    data, labels = digits.data, digits.target
    if save:
        np.savetxt(base_path + "basic/digits/digits.txt", data, fmt="%.5f", delimiter=" ", header = "%i %i"%(data.shape[0],data.shape[1]),comments='')
        np.savetxt(base_path + "basic/digits/digits.gt", labels, fmt="%i")
    return data, labels

def get_wine_dataset(save = True):
    wine = datasets.load_wine()
    data, labels = wine.data, wine.target
    if save:
        np.savetxt(base_path + "basic/wine/wine.txt", data, fmt="%.5f", delimiter=" ", header = "%i %i"%(data.shape[0],data.shape[1]),comments='')
        np.savetxt(base_path + "basic/wine/wine.gt", labels, fmt="%i")
    return data, labels

def get_breast_cancer_dataset(save = True):
    breast_cancer = datasets.load_breast_cancer()
    data, labels = breast_cancer.data, breast_cancer.target
    if save:
        np.savetxt(base_path + "basic/breast_cancer/breast_cancer.txt", data, fmt="%.5f", delimiter=" ", header = "%i %i"%(data.shape[0],data.shape[1]),comments='')
        np.savetxt(base_path + "basic/breast_cancer/breast_cancer.gt", labels, fmt="%i")
    return data, labels

def get_covtype_dataset(save = True):
    covtype = datasets.fetch_covtype()
    data, labels = covtype.data, covtype.target
    if save:
        np.savetxt(base_path + "covtype/covtype.txt", data, fmt="%.5f", delimiter=" ", header = "%i %i"%(data.shape[0],data.shape[1]),comments='')
        np.savetxt(base_path + "covtype/covtype.gt", labels, fmt="%i")
    return data, labels

def get_faces_dataset(save = True):
    faces = datasets.fetch_olivetti_faces()
    data, labels = faces.data, faces.target
    if save:
        np.savetxt(base_path + "basic/faces/faces.txt", data, fmt="%.5f", delimiter=" ", header = "%i %i"%(data.shape[0],data.shape[1]),comments='')
        np.savetxt(base_path + "basic/faces/faces.gt", labels, fmt="%i")
    return data, labels

if __name__ == "__main__":
    get_iris_dataset()
    get_digits_dataset()
    get_wine_dataset()
    get_breast_cancer_dataset()
    get_covtype_dataset()
    get_faces_dataset()