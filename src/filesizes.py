import os
import sys

if __name__ == "__main__":
	file_list = open(sys.argv[1])
	file_sum = 0
	for line in file_list:
		file_sum += os.path.getsize(line)

	println(file_sum)

