with open('kPhone.txt') as f:
    lines = f.readlines()
    packetsData = []

    print(len(lines))

    lines = list(filter(lambda a: "RSSI" not in a and "SNR" not in a and "Freq" not in a, lines))
    print(len(lines))

    lines = list(map(lambda a: a[8:], lines))

    def removePacketStr(line):
        if "Packet" in line:
            return "\t"
        else:
            return line.strip()

    lines = list(map(removePacketStr, lines))

    print(len(lines))

    bigString = "".join(lines)

    hexList = bigString.split("\t")

    print(len(hexList))
    #print(hexList)

    def getHexToString(line):
        try:
            return bytes.fromhex(line)[:7].decode('utf-8')
        except:
            return  ""
    
    def IsImage(line):
        return getHexToString(line)[0:2] == "PD"
    
    def ReturnImagesBytes(line):
        return bytes.fromhex(line)[7:]
    
    #print(list(map(getHexToString, hexList)))

    hexImageList = list(filter(IsImage, hexList))

    #print(hexImageList)

    imageBytesList = list(map(ReturnImagesBytes, hexImageList))

    imageBytes = b''.join(imageBytesList)
  
    # Open in "wb" mode to
    # write a new file, or 
    # "ab" mode to append
    with open("image.bin", "wb") as binary_file:
        
        # Write bytes to file
        binary_file.write(imageBytes)