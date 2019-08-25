import sys
import os
import csv

startFrames = []
endFrames = []
writer = 0
FL =["FL"]
DM = ["DM"]
FB1 = ["FB1"]
FB2 = ["FB2"]
FB3 = ["FB3"]
def runAlgo(folder, experiment):
  print folder
  print experiment
  path = os.path.join(folder, experiment)
  path = os.path.join(path, "keyFrames")
  imageFrames = []
  imageExtension = ".jpg"
  for image in os.listdir(path):
    if image.endswith(imageExtension):
      image = image[:-len(imageExtension)]
      imageFrames.append(int(image))
  imageFrames.sort()
  print "Keyframes:"
  print "\t" + str(imageFrames)
  shotIndex = 0
  truePositive = 0
  trueNegative = 0
  falsePositive = 0
  falseNegative = 0
  shotScore = 0
  for keyframe in imageFrames:
    for i in range(0, len(startFrames)-1):
      if keyframe >= startFrames[i] and keyframe <= startFrames[i+1]:
        shotIndex = i
        break
    # print "Keyframe " + str(keyframe)
    # print "Shot index " + str(shotIndex)
    # print "Start frame " + str(startFrames[shotIndex])
    # print "End frame " + str(endFrames[shotIndex])
    if keyframe >= startFrames[shotIndex] and keyframe <= endFrames[shotIndex]:
      truePositive = truePositive + 1
      print "Adding " + str(keyframe)
    else:
      falsePositive = falsePositive + 1
  for i in range(0, len(startFrames)):
    shotList = list(range(startFrames[i],endFrames[i]))
    # print "Start: " + str(startFrames[i])
    # print "End: " + str(endFrames[i])
    # print "ShotList " + str(shotList)
    exclusionList = set(shotList) - set(imageFrames)
    print "Exclusion " + str(exclusionList)
    falseNegative = falseNegative + len(exclusionList)
    commonSet = list(set(shotList).intersection(imageFrames))
    print "Common " + str(commonSet)
    if len(commonSet) > 0:
        shotScore = shotScore + 1
    # break
  print "True Positive " + str(truePositive)
  print "False Positive " + str(falsePositive)
  print "False Negative " + str(falseNegative)

  shotScoreScaled = float(shotScore)/len(startFrames)
  print "Shot Score " + str(shotScoreScaled)
  if "FL" in experiment:
    FL.append("="+str(shotScore)+"/"+str(len(startFrames)))
  elif "DM" in experiment:
    DM.append("="+str(shotScore)+"/"+str(len(startFrames)))
  elif "FB1" in experiment:
    FB1.append("="+str(shotScore)+"/"+str(len(startFrames)))
  elif "FB2" in experiment:
    FB2.append("="+str(shotScore)+"/"+str(len(startFrames)))
  elif "FB3" in experiment:
    FB3.append("="+str(shotScore)+"/"+str(len(startFrames)))

if __name__ == "__main__":
  if len(sys.argv) < 4:
    print "\n\nUSAGE: calculateExtractiveSummaryAccuracy.py <shot/video/location> <keyframes/folder> <output/csv/file/path>\n\n"
    sys.exit()
  shotFolder = sys.argv[1]
  resultsFolder = sys.argv[2]
  outputFilePath = sys.argv[3]
  csvFile = open(outputFilePath, "wb")
  writer = csv.writer(csvFile, delimiter=",", quotechar='"', quoting=csv.QUOTE_ALL)
  rowData = ["Shot Path", shotFolder]
  writer.writerow(rowData)
  rowData = ["Keyframe folder", resultsFolder]
  writer.writerow(rowData)
  for file in os.listdir(shotFolder):
    if file.endswith(".avi"):
      file = file[:-4]
      splitData = file.split("-")
      startFrames.append(int(splitData[0]))
      endFrames.append(int(splitData[1]))
  startFrames.sort()
  print "Start times:"
  print "\t" + str(startFrames)
  endFrames.sort()
  print "End times:"
  print "\t" + str(endFrames)
  print "Start Frame count " + str(len(startFrames))
  print "End Frame count " + str(len(endFrames))
  print "Shot data ready"
  subFolders = [int(d[:-1]) for d in os.listdir(resultsFolder) if os.path.isdir(os.path.join(resultsFolder, d))]
  # print subFolders
  subFolders.sort()
  # print subFolders
  writer.writerow([""]+subFolders)
  for folder in subFolders:
    folderName = str(folder) + "%"
    folderPath = os.path.join(resultsFolder, folderName)
    experimentFolders = [d for d in os.listdir(folderPath) if os.path.isdir(os.path.join(folderPath, d))]
    print experimentFolders
    for experiment in  experimentFolders:
        path = os.path.join(folderPath,experiment)
        path = os.path.join(path, "keyFrames")
        runAlgo(folderPath, experiment)

  writer.writerow(FL)
  writer.writerow(DM)
  writer.writerow(FB1)
  writer.writerow(FB2)
  writer.writerow(FB3)
