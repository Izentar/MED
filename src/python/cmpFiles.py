import sys

def getFileDict(fileName):
    dictFile = {}
    with open(fileName, 'r') as file1:
        line1 = file1.readlines()
        lineNumb = -1
        for f1 in line1:
            lineNumb += 1
            f1 = f1.rstrip('\n')
            if(len(f1) == 0):
                continue
            if(f1 in dictFile.items()):
                print(f"File '{fileName}' have two identical prefixes '{f1}' in line {lineNumb} and {dictFile[f1]}")
                exit(0)
            dictFile[f1] = lineNumb
    return dictFile


dictFile1 = getFileDict(sys.argv[1])
dictFile2 = getFileDict(sys.argv[2])
set1 = set(dictFile1.keys())
set2 = set(dictFile2.keys())

if(set1 != set2):
    print(f"Files do not match. Found difference in '{set1 ^ set2}'")
    exit(0)


print("Files do match")