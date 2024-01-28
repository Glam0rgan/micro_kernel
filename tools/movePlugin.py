import os
import fnmatch

pwd = os.getcwd()
print(pwd)
#father_path = os.pardir

nblocks = 0
blockSize = 512

def move_elf(filename):
    global nblocks
    global blockSize
    stats = os.stat(root_dir+"/"+filename)
    blocknum = stats.st_size//blockSize
    if stats.st_size%blockSize != 0 :
        blocknum+=1;
    cmd = "dd if="+root_dir+"/"+filename+" of="+pwd+"/out/fs.img"+" bs=512 seek="+str(nblocks)+" count="+str(blocknum)
    print(filename)
    os.system(cmd)
    nblocks += blocknum 

root_dir = pwd+"/fs"
for root, dirs, files in os.walk(root_dir):
    for file in files:
    	move_elf(file)

