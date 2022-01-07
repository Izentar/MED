import sys, os
import pandas as pd
import argparse
import re

parser = argparse.ArgumentParser()
parser.add_argument('-m', type=str, nargs=1, required=True, choices=['a', 'c', 'cc', 'aa'], help="Mode = 'a' - average for singlethread; 'c' - combine all the data into one file; 'cc' - combine all files of the same sufix into one in the same folder")
parser.add_argument('-f', type=str, nargs='+', required=True, help='Path to files to average')
parser.add_argument('--sb', type=str, nargs=1, default=None, help='Sort by column')
parser.add_argument('-o', type=str, nargs=1, default=None, required=False, help='Where to save new file')
parser.add_argument('-r', type=str, nargs=1, default=None, required=False, help='Where to save new file')
parser.add_argument('--head', type=str, nargs='+', required=True, help="Column name to average. Use character ' to write string with space.")
args = parser.parse_args()

def getCSVs(files):
    toAvg = []
    numberOfFiles = len(files)
    for idx, arg in enumerate(files):
        toAvg.append(pd.read_csv(arg, sep=';', header=1).iterrows())
    return toAvg, numberOfFiles

def twoCSV(toAvg):
    if(len(toAvg) <= 1):
        print("No csv to average")
        exit(0)


def simpleAverage(inputFiles, output):
    toAvg, numberOfFiles = getCSVs(inputFiles)
    twoCSV(toAvg)
    newVals = {}
    for i in args.head:
        newVals[i] = []

    for pack in zip(*toAvg):
        for val in args.head:
            tmp = 0
            for idx, row in pack:
                tmp += row[val]
            newVals[val].append(tmp/numberOfFiles)
    df = pd.DataFrame(newVals)
    df.to_csv(output, header=True, sep=';', index=False)

def combine(inputFiles, output):
    toAvg, numberOfFiles = getCSVs(inputFiles)
    newVals = []
    for pack in zip(*toAvg):
        for idx, row in pack:
            newVals.append(row)
    if(args.sb is not None):
        newVals.sort(key=lambda x: x[args.sb[0]])

    df = pd.DataFrame(newVals)
    df.to_csv(output, header=True, sep=';', index=False)

def combineAll():
    for (dirpath, dirnames, filenames) in os.walk(args.f[0]):
        if(len(filenames) == 0):
            continue
        fnames = []
        for name in filenames:
            if args.r[0] in name:
                fnames.append(dirpath + '/' + name)
        combine(inputFiles=fnames, output=dirpath + f'/{args.r[0]}_comb.csv')

def averageAll():
    for (dirpath, dirnames, filenames) in os.walk(args.f[0]):
        if(len(filenames) == 0):
            continue
        fnames = []
        for name in filenames:
            if args.r[0] in name:
                fnames.append(dirpath + '/' + name)
        simpleAverage(inputFiles=fnames, output=dirpath + f'/{args.r[0]}_avg.csv')

if(args.m[0] == 'a'):
    simpleAverage()
elif(args.m[0] == 'c'):
    combine()
elif(args.m[0] == 'cc'):
    combineAll()
elif(args.m[0] == 'aa'):
    averageAll()