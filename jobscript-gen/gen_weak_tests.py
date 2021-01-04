import individual_script

numNodesArr = [1,2,4,8,16,32]
xlen = [2048, 4096, 4096, 8192, 8192, 16384]
ylen = [4096, 4096, 8192, 8192, 16384, 16384]
patchSize = [512, 512, 512, 512, 512, 512]

for i in range(6):
    jobtemp = 'GPIweak' + str(numNodesArr[i])
    f = open('./WeakJobs/' + jobtemp + '.sh', 'w')
    f.write(individual_script.generateGPI(jobtemp, numNodesArr[i], xlen[i], ylen[i], patchSize[i]))
    f.close()

    jobtemp = 'UPCweak' + str(numNodesArr[i])
    f = open('./WeakJobs/' + jobtemp + '.sh', 'w')
    f.write(individual_script.generateUPCrank(jobtemp, numNodesArr[i], xlen[i], ylen[i], patchSize[i]))
    f.close()