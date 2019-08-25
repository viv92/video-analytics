from math import sqrt
from sklearn import neighbors
from os import listdir
from os.path import isdir, join, isfile, splitext
import pickle
from PIL import Image, ImageFont, ImageDraw, ImageEnhance
import face_recognition
from face_recognition import face_locations
from face_recognition.cli import image_files_in_folder
import cv2
import sys
import time

train_data_path = sys.argv[1]
model_save_path = sys.argv[2]
knn_algo = 'ball_tree'

X = []
y = []


for class_dir in listdir(train_data_path):
    if not isdir(join(train_data_path, class_dir)):
        continue
    for img_path in image_files_in_folder(join(train_data_path, class_dir)):
        image = face_recognition.load_image_file(img_path)
        faces_bboxes = face_locations(image)
        if len(faces_bboxes) != 1:
            continue
        X.append(face_recognition.face_encodings(image, known_face_locations=faces_bboxes)[0])
        y.append(class_dir)

    n_neighbors = int(round(sqrt(len(X))))

    knn_clf = neighbors.KNeighborsClassifier(n_neighbors=n_neighbors, algorithm=knn_algo, weights='distance')
    knn_clf.fit(X, y)

    with open(model_save_path, 'wb') as f:
        pickle.dump(knn_clf, f)
