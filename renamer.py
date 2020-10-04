import os, shutil

'''
replace 

C:/Users/ua18387/kicad_library
${WILLIAM_3D}
'''

spath = r'C:/Users/ua18387/kicad_library'
relpath = r'${WILLIAM_3D}'

kicad_footprint_extensions = ['.kicad_mod']

for root, folders, files in os.walk(os.getcwd()):    
    for file in files:
        fullpath = os.path.join(root, file)
        #print (fullpath)
        
        fn, ext = os.path.splitext(fullpath)
        if ext in kicad_footprint_extensions:
            print (fullpath)
            bkuppath = os.path.join(root, 'bkup_'+ file)
            #bkuppath = 'test.txt'
            shutil.copyfile(fullpath, bkuppath)
            
            with open(bkuppath, 'r') as reader:
                with open(fullpath, 'w') as writer:
                    for line in reader:
                        modline = line
                        if spath in line:
                            modline = modline.replace(spath, relpath)
                            modline = modline.replace(r'/william/', r'/')
                            modline = modline.replace(r'.3d', r'.3dshapes')
                        writer.write(modline)# + '\n')
            
