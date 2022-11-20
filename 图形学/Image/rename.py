import os

g = os.walk(r".")  

for path,dir_list,file_list in g:
    for file_name in file_list:
        # print(os.path.join(path, file_name))
        # file_path = os.path.join(path, file_name)
        os.rename(file_name, "0" + file_name)