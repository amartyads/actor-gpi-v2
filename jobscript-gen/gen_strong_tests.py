import individual_script

numNodesArr = [1,2,4,8,16,32]
xlen = [16384, 16384, 16384, 16384, 16384, 16384]
ylen = [16384, 16384, 16384, 16384, 16384, 16384]
patchSize = [1024, 1024, 512, 512, 256, 256]

for i in range(6):
    jobtemp = 'GPIstrong' + str(numNodesArr[i])
    f = open('./StrongJobs/' + jobtemp + '.sh', 'w')
    f.write(individual_script.generateGPI(jobtemp, numNodesArr[i], xlen[i], ylen[i], patchSize[i]))
    f.close()

    jobtemp = 'UPCstrong' + str(numNodesArr[i])
    f = open('./StrongJobs/' + jobtemp + '.sh', 'w')
    f.write(individual_script.generateUPCrank(jobtemp, numNodesArr[i], xlen[i], ylen[i], patchSize[i]))
    f.close()