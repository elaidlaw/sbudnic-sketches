
def getByteList(dir):   
    with open(dir) as f:
        lines = f.readlines()

        # Filter out non command lines
        lines = list(filter(lambda a: "RSSI" not in a and "SNR" not in a and "Freq" not in a, lines))

        # cut off timestamp from data
        lines = list(map(lambda a: a[8:], lines))

        def removePacketStr(line):
            if "Packet" in line:
                return "\t"
            else:
                return line.strip()

        # adds a tab between the Packet line to split between dif packets
        lines = list(map(removePacketStr, lines))
        
        # String of all packet data split by tabs
        fullPacketString = "".join(lines)

        # splits it back into a list of packet data in hex
        hexList = fullPacketString.split("\t")

        def getHexToString(line):
            try:
                return bytes.fromhex(line)[:7].decode('utf-8')
            except:
                return  ""
        
        def IsImage(line):
            return getHexToString(line)[0:2] == "PD"
        
        def ReturnImagesBytes(line):
            try:
                return bytes.fromhex(line)[7:]
            except:
                return ""

        def ReturnFullBytes(line):
            try:
                return bytes.fromhex(line)
            except:
                return ""
        
        # creates tuple of command in utf 8 and command data in bytes
        def ReturnImagesCommandBytes(line):
            try:
                return (bytes.fromhex(line)[:7].decode("utf-8"), bytes.fromhex(line)[7:])
            except:
                return ""

        byteList = list(map(ReturnImagesCommandBytes, hexList))
        byteList = list(filter(lambda a: a != "", byteList))

        """
        finBytes = []
        commands = []
        for j, b in enumerate(byteList):
            try:
                finBytes.append(byteList[j])
                commands.append(b[:6].decode('utf-8'))
            except:
                pass
        """       
        print(len(byteList))
        input("Hit Enter To Start Reading Data")

        return byteList

        #hexImageList = list(filter(IsImage, hexList))

        #print(hexImageList)

        #imageBytesList = list(map(ReturnImagesBytes, hexImageList))

        #imageBytes = b''.join(imageBytesList)
    
        # Open in "wb" mode to
        # write a new file, or 
        # "ab" mode to append
        #with open("image.bin", "wb") as binary_file:
            
            # Write bytes to file
        #    binary_file.write(imageBytes)