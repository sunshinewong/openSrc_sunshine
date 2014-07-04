import os
import sys

TYPE_DLL_EXE = (".dll",".exe")
def operParam():
    path={}
    print(len(sys.argv))
    for i in range(1,len(sys.argv)) :
        print("i", i)
        if sys.argv[i] == '-h':
            print(compress.__doc__)
            sys.exit()
        elif sys.argv[i] =='-d':  
           path['dir'] = sys.argv[i+1]
        elif sys.argv[i] =='-f':  
           path['exe'] = sys.argv[i+1]
    if len(path) != 2:
        print(compress.__doc__)
        sys.exit()
    else : return path  

def check(filename):
    if os.path.splitext(filename)[1] in  TYPE_DLL_EXE :
         print('The filename of type in (dll,exe) ：', filename)
         return True      

def getFilesName(filePath):
    compressList=[]
    if os.path.isdir(filePath):
        for item in os.listdir(filePath) :
            if os.path.isdir(item) :
              getFilesName(item)
            else: 
                if  check(item):
                    path =filePath+"\\"+item
                    compressList.append(path)
    else :
        if check(filePath):
                compressList.append(path)
    return compressList

def compress(dirpath, exepath):
    '''
    -h :help,get more infomation
    -d  directorypath:dir,directory path to needed compress
    -f  directorypath:directory path of upx.exe
    Example:
    The location of upx.exe is D:\\UPX
    The directory which you would want to compress is D:\\dir
    The command is:
    python.exe upx.py -d D:\\dir -f d:\\UPX
    '''
    compressList=getFilesName(dirpath)
    cmpress_command = "%s\\upx.exe --best" % exepath
    for item in compressList :
         cmpress_command =cmpress_command +' ' +item
    print("upx command:", cmpress_command)
    if os.system(cmpress_command) == 0:
           print ('压缩成功')
    else:
         print (item,'压缩失败')

if len(sys.argv)<2 :
    print(compress.__doc__)
    sys.exit()
path=operParam()
print("path:", path)
compress(path['dir'], path['exe'])





