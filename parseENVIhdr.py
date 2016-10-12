#!/usr/bin/env python3

# gets info from file, strips whitespaces, returns list of data in file
def get_data_from_file():
    flag = False
    
    # absolute path, should probably change in future
    filename = "/Users/raco235/Desktop/Photos/2016-Hyperspectral/Pezzo2/2016_07_06_10_56_16/2016_07_06_10_56_16data.hdr"
    while flag == False:
        try:
            infile = open(filename,"r")
            flag = True
        except:
            print("Filename not valid")
            filename = input("Enter filename: ")
            
    # read lines into list
    lst1 = infile.readlines()
    
    
    # strip whitespaces from each line and replace current line with stripped line
    for line in (lst1):
        lst1[lst1.index(line)] = lst1[lst1.index(line)].strip()
        
    infile.close()
    
    # returns file data in list
    return lst1
    

def main():
    fileData = get_data_from_file()
    #print(fileData)
    
    wavelengths = []
    
    start = fileData.index("wavelength = {")
    
    num = 0
    for i in range(start+1,len(fileData)-1): #len(fileData)-1 maybe not for future sets?
        #print(fileData[i])
        fileData[i].strip(",")
        wavelengths.append(fileData[i].strip(","))
        print(wavelengths[num])
        num += 1
    
    
main()