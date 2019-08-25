import matplotlib.pyplot as plt
import sys
import os

def parseLogFile(filename):
	lines = []

	with open(filename) as f:
		lines += f.readlines()


	train_loss = []
	train_accuracy = []
	for line in lines:
		if "Train net output #0" in line:
			# print line
			start_index = line.find("=")+2
			last_index = line.find(" ", start_index)
			train_accuracy.append(float(line[start_index:last_index]))
		elif "Train net output #1" in line:
			# print line
			start_index = line.find("=")+2
			last_index = line.find(" ", start_index)					
			train_loss.append(float(line[start_index:last_index]))

	test_loss = []
	test_accuracy = []
	for line in lines:
		if "Test net output #0" in line:
			# print line
			start_index = line.find("=")+2
			last_index = line.find(" ", start_index)
			test_accuracy.append(float(line[start_index:last_index]))
		elif "Test net output #1" in line:
		# print line
			start_index = line.find("=")+2
			last_index = line.find(" ", start_index)
			test_loss.append(float(line[start_index:last_index]))

	
	return (train_loss, train_accuracy, test_loss, test_accuracy)

'''
	Run this script from the aisaac root by passing the caffe log as a command line parameter
	eg. python tools/parseLog.py build/storageDir/fineTuning/FACE_VGG_AGE/age_vgg.log
'''

if __name__ == '__main__':
	filename = sys.argv[1]
	dir_name = filename[:filename.rfind('/')]
	filename = filename[filename.rfind('/')+1:]

	train_loss, train_accuracy, test_loss, test_accuracy = parseLogFile(dir_name + "/" + filename)
	# _, train_loss, test_loss, test_accuracy = parseLogFile(dir_name + "/" + filename)
	
	str = ""
	str += "Final training loss: {0}\n".format(train_loss[-1])
	str += "Final test loss: {0}\n".format(test_loss[-1])
	str += "Best training accuracy: {0}\n".format(train_accuracy[-1])
	str += "Best validation accuracy: {0}".format(test_accuracy[-1])

	with open(dir_name + "/metrics.log", "w") as f:
		f.write(str)

	plt.figure()
	plt.xlabel("Iteration")
	plt.ylabel("Train Loss")
	plt.plot([i for i in range(0,500,25)], train_loss)
	plt.savefig(dir_name + '/Train Loss Graph.png')

	plt.figure()
	plt.xlabel("Iteration")
	plt.ylabel("Train Accuracy")
	plt.plot([i for i in range(0,500,25)], train_accuracy)
	plt.savefig(dir_name + '/Train Accuracy Graph.png')

	plt.figure()
	plt.xlabel("Iteration")
	plt.ylabel("Test Loss")
	plt.plot([i for i in range(0,501,100)], test_loss)
	plt.savefig(dir_name + '/Test Loss Graph.png')

	plt.figure()
	plt.xlabel("Iteration")
	plt.ylabel("Test Accuracy")
	plt.plot([i for i in range(0,501,100)], test_accuracy)
	plt.savefig(dir_name + '/Test Accuracy Graph.png')

	plt.figure()
	plt.xlabel("Iteration")
	plt.ylabel("Accuracy")
	plt.plot([i for i in range(0,500,25)], train_accuracy, 'r')
	plt.plot([i for i in range(0,501,100)], test_accuracy, 'b')
	plt.savefig(dir_name + '/TrainVTest Accuracy Graph.png')