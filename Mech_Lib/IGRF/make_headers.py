# Script to autogenerate an IGRF coefficient C header file from IGRF12.COF

import datetime
import os

# Helper functions
def isFloat(string):
    try:
        float(string)
        return True
    except ValueError:
        return False

# Get user input
filename = input("Enter the name of the IGRF model file: ")
while not os.path.isfile(filename):
    print("File not found!")
    filename = input("Enter the name of the IGRF model: ")

desired_year = int(input("Enter the desired IGRF year: "))
igrf_year = desired_year - desired_year%5
desired_delim = 'IGRF'+str(igrf_year)
print('Looking for: '+ desired_delim)

# Open files
igrf = open(filename, "r") 
header = open("IGRFCOF.h", "w")

# Write file header
header.write("/**\n * IGRF12.h\n * Header file for IGRF coefficients\n")
now = datetime.datetime.now()
header.write(" * Autogenerated at "+datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y")+" by "+os.path.basename(__file__)+"\n")
header.write("*/\n\n")

minyear = 0
num_deg = 0
num_order = 0
deg = []
order = []
g1 = []
h1 = []
g2 = []
h2 = []
gh1 = [0.0]
gh2 = [0.0]

invars = [deg, order, g1, h1, g2, h2]

header_found = False;

# Read coefficient file
for line in igrf:
    
    if header_found:	
        floats = [float(n) for n in line.split() if isFloat(n)]
        for i in range(0, len(invars)):
            invars[i].append(floats[i])
        
        gh1.append(g1[-1])
        gh2.append(g2[-1])
        
        if order[-1] != 0:
            gh1.append(h1[-1])
            gh2.append(h2[-1])
    else:
        tokens = line.split()
        if tokens[0] == desired_delim:
            minyear = float(tokens[1])
            num_order = int(tokens[2])
            num_deg = int(tokens[3])
            header_found = True

if header_found == False:
    print("Could not find entered year!")
    quit()

outvars = [gh1, gh2]
varnames = ["float gh1", "float gh2"]

# Write constants to header file
header.write("float minyear = "+str(minyear)+";\n")
header.write("char num_order = "+str(num_order)+";\n")
header.write("char num_deg = "+str(num_deg)+";\n\n")

# Write coefficients to header file
vals_per_line = 10
for i in range(0, len(outvars)):
    header.write(varnames[i]+"[] = {")
    for j in range(0, len(outvars[i])):
        header.write(str(outvars[i][j]))
        if j != len(outvars[i])-1:
            header.write(", ")
        if (j+1)%vals_per_line == 0:
            header.write("\n\t")
    header.write("};\n\n")

# Close files
igrf.close()
header.close()

print("Successfully parsed IGRF model\n")
