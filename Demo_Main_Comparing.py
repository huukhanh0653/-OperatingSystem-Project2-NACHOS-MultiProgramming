nachosFolderPath = ".."
demoFolderPath = "/Demo/nachos/NachOS-4.0/code"
outputfile = "Result.txt"


def Compare(nachosFile: str, demoFile: str, resultFileVar):
    nachos = open(nachosFolderPath+nachosFile,"r")
    demo = open(demoFolderPath+demoFile,"r")    
    
    nachosLines = nachos.readlines()
    demoLines = demo.readlines()
    minline = min(len(nachosLines),len(demoLines))
    
    resultFileVar.write(demoFile+"\n")
    
    for i in range(0,minline):
        if (nachosLines[i] != demoLines[i]):
            resultFileVar.write(f"line {i}: {demoLines[i]}\n")
    resultFileVar.write("\n\n")

fileContainer = open("filePath.txt","r")

fileContainerLines = fileContainer.readlines()

resultFile = open("result.txt","w+")

for line in fileContainerLines:
    Compare(nachosFolderPath+line.replace("\n",""),demoFolderPath+line.replace("\n",""),resultFile)
