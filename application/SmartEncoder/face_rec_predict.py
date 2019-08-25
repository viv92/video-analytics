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

ALLOWED_EXTENSIONS = {'png', 'jpg', 'jpeg'}
DIST_THRESH = .5


image_path = sys.argv[1]
model_save_path = sys.argv[2]

if not isfile(image_path) or splitext(image_path)[1][1:] not in ALLOWED_EXTENSIONS:
    raise Exception("Invalid image path: {}".format(image_path))

if model_save_path == "":
    raise Exception("Invalid model path")
else:
    with open(model_save_path, 'rb') as f:
        knn_clf = pickle.load(f)

X_img = face_recognition.load_image_file(image_path)

X_faces_loc = face_locations(X_img)

if len(X_faces_loc) == 0:
    print("PREDICTION<<<0<<<PREDICTION")
    sys.exit()

faces_encodings = face_recognition.face_encodings(X_img, known_face_locations=X_faces_loc)

closest_distances = knn_clf.kneighbors(faces_encodings, n_neighbors=1)

is_recognized = [closest_distances[0][i][0] <= DIST_THRESH for i in range(len(X_faces_loc))]

if is_recognized[0]:
    print("PREDICTION<<<" + knn_clf.predict(faces_encodings)[0] + "<<<PREDICTION")
else:
    print("PREDICTION<<<0<<<PREDICTION")
