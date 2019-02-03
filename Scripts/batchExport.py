# Program to batch export every .ase file in folder to .png

import os

command = r"""D:\Repositorios\PixelArt\aseprite\build\bin\aseprite.exe --batch """
option = " --save-as "
fileExt = ".png"

# Get every file in this directory
fileList = os.listdir(".")

# Execute the command
for file in fileList:
	fileName = file.split(".ase")[0] + fileExt
	cmd = command + file + option + fileName
	print(cmd + '\n')
	os.system(cmd)
	
