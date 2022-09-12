import os

samples = []
samples.append(("SingleElectron","2016"))
samples.append(("SingleElectron","2017"))
samples.append(("EGamma,2018"))
samples.append(("SingleMuon","2016"))
samples.append(("SingleMuon","2017"))
samples.append(("SingleMuon","2018"))
for datastream,year in samples:
    print(datastream,year)
    os.system(f'dasgoclient -query="dataset=/{datastream}/Run{year}*UL{year}_MiniAODv2_NanoAODv9-v*/NANOAOD" >> filenames/{datastream}_{year}_datasets.txt')

    with open(f"filenames/{datastream}_{year}_datasets.txt","r") as f:
        for i,line in enumerate(f):
            os.system(f'dasgoclient -query="file dataset={line}" >> filelist_{i}.txt'.replace("\n",""))
    # break
    # os.remove("filelists.txt")

    files = [x for x in os.listdir() if "filelist" in x and ".py" not in x]

    with open(f"filenames/{datastream}_{year}.txt","w") as output:
        with open(f"filenames/{datastream}_{year}apv.txt","w") as output_apv:
            for file in files:
                with open(file,"r") as f:
                    for line in f:
                        eos_path = f'/eos/cms{line}'.replace('\n','')
                        hipm = "hipm" in line.lower()
                        apv = "_apv" if hipm else ""
                        if os.path.exists(eos_path):
                            filepath = f'{eos_path}'
                        else:
                            local_path = f'/eos/user/s/siluo/cms_data/{datastream}/{year}{apv}'
                            if not os.path.isdir(local_path):
                                os.system(f'mkdir -p {local_path}')
                            filename = line.split("/")[-1].replace("\n","")
                            if filename not in os.listdir(local_path):
                                os.system(f'xrdcp root://cms-xrd-global.cern.ch/{line} {local_path}/{filename}'.replace("\n",""))
                                # print(f'xrdcp root://cms-xrd-global.cern.ch/{line} {local_path}/{filename}'.replace("\n",""))
                            filepath = f'{local_path}/{filename}'
                        print(filepath)
                        if hipm:
                            output_apv.write(f'{filepath}\n')
                        else:
                            output.write(f'{filepath}\n')

                os.remove(file)
