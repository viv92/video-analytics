import sys
import os
import csv

groundFrame = []
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
  imageFrames = []
  imageExtension = ".jpeg"
  for image in os.listdir(path):
    if image.endswith(imageExtension):
      image = image[:-len(imageExtension)]
      imageList = image.split("-")
      image = imageList[0]
      imageFrames.append(int(image))
  imageFrames.sort()
  print "Keyframes:"
  print "\t" + str(imageFrames)
  shotScore = 0
  intersectionList = list(set(imageFrames).intersection(groundFrame))
  shotScore = len(intersectionList)
  shotScoreScaled = float(shotScore)/len(groundFrame)
  print "Shot Score " + str(shotScoreScaled)
  if "FacilityLocation" in experiment:
    FL.append("="+str(shotScore)+"/"+str(len(groundFrame)))
  elif "DisparityMin" in experiment:
    DM.append("="+str(shotScore)+"/"+str(len(groundFrame)))
  elif "FeatureBasedFunctionWithSqrtOverModular" in experiment:
    FB1.append("="+str(shotScore)+"/"+str(len(groundFrame)))
  elif "FeatureBasedFunctionWithInverseFunction" in experiment:
    FB2.append("="+str(shotScore)+"/"+str(len(groundFrame)))
  elif "FeatureBasedFunctionWithLogFunction" in experiment:
    FB3.append("="+str(shotScore)+"/"+str(len(groundFrame)))

if __name__ == "__main__":
  if len(sys.argv) < 6:
    print "\n\nUSAGE: calculateQuerySummaryAccuracy.py <ground/set/folder> <summary/folder> <output/csv/file/path> <query/concept> <query/value>\n\n"
    sys.exit()
  groundsetFolder = sys.argv[1]
  resultsFolder = sys.argv[2]
  outputFilePath = sys.argv[3]
  queryConcept = sys.argv[4]
  queryValue = sys.argv[5]
  csvFile = open(outputFilePath, "wb")
  writer = csv.writer(csvFile, delimiter=",", quotechar='"', quoting=csv.QUOTE_ALL)
  rowData = ["Ground Set Folder", groundsetFolder]
  writer.writerow(rowData)
  rowData = ["Summary Folder", resultsFolder]
  writer.writerow(rowData)
  for file in os.listdir(groundsetFolder):
    if file.endswith(".jpeg"):
      file = file[:-5]
      splitData = file.split("-")
      groundFrame.append(int(splitData[0]))
  groundFrame.sort()
  print "Ground Frame:"
  print "\t" + str(groundFrame)
  print "Ground count " + str(len(groundFrame))
  print "Ground data ready"

  subFolders = [int(d[:-1]) for d in os.listdir(resultsFolder) if os.path.isdir(os.path.join(resultsFolder, d))]
  print subFolders
  subFolders.sort()
  print subFolders
  writer.writerow([""]+subFolders)
  for folder in subFolders:
    folderName = str(folder) + "%"
    folderPath = os.path.join(resultsFolder, folderName)
    folderPath = os.path.join(folderPath,"QueryBasedSummary")
    folderPath = os.path.join(folderPath,queryConcept)
    folderPath = os.path.join(folderPath,queryValue)
    experimentFolders = [d for d in os.listdir(folderPath) if os.path.isdir(os.path.join(folderPath, d)) and d != "GroundSet"]
    print experimentFolders
    for experiment in  experimentFolders:
        path = os.path.join(folderPath,experiment)
        runAlgo(folderPath, experiment)
  writer.writerow(FL)
  writer.writerow(DM)
  writer.writerow(FB1)
  writer.writerow(FB2)
  writer.writerow(FB3)
