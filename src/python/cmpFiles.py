import sys


with open(sys.argv[1], 'r') as file1:
    with open(sys.argv[2], 'r') as file2:
        file1.seek(0)
        line1 = file1.readlines()

        for f1 in line1:
            f1 = f1.rstrip('\n')
            if(len(f1) == 0):
                continue
            file2.seek(0)
            line2 = file2.readlines()
            same = False
            for f2 in line2:
                f2 = f2.rstrip('\n')
                if(len(f2) == 0):
                    continue
                if(f1 == f2):
                    same = True
                    break
            if(not same):
                print(f"Files do not match. Could not find in '{sys.argv[2]}' line '{f1}'")
                exit(0)

print("Files do match")