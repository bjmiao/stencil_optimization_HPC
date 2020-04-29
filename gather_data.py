import os
import pandas as pd
import re
df = pd.DataFrame(columns = ["arr_size", "arr_bytes", "t_max", "cache_reference", "cache_misses", "total_time"])
folder_name = "profile_opt/"
all_file = os.listdir(folder_name)
print("Total file num:" + str(len(all_file)))
print(list(df.keys()))
for re_filename in all_file:
    filename = folder_name + re_filename

    data_point = {}
    matchObj = re.match("profile_n([0-9]+)_t([0-9]+)_[0-9]+.txt",re_filename)
    data_point['arr_size'] = int(matchObj.group(1))
    data_point['arr_bytes'] = data_point['arr_size'] * 8
    data_point['t_max'] = int(matchObj.group(2))
    prof_file = open(filename)
    prof_txt = prof_file.readlines()
    for line in prof_txt:
        if "cache-misses" in line:
            line = line.strip().split()
            # print(line)
            data_point["cache_misses"] = int(line[0].replace(",",""))
            data_point["misses_precent"] = float(line[3])
        if "cache-references" in line:
            line = line.strip().split()
            # print(line)
            data_point["cache_reference"] = int(line[0].replace(",",""))
        if "Total use time:" in line:
            line = line.strip().split()
            data_point["total_time"] = float(line[-1]) 
            # print(line)
    # print(data_point)
    
    # if (list(df.keys()) in list(data_point.keys())):
    df = df.append(data_point, ignore_index=True)
    # else:
        # print("This data point is missing some metrics", data_point)
print(df)
df.to_csv(folder_name + "data.csv   ")