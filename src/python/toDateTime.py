import sys
import pandas

td = pandas.Timedelta(float(sys.argv[1]), unit='ns')
print(td)