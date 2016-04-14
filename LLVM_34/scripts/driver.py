import os,sys,math,pdb,filecmp,logging,argparse,subprocess
from argparse import RawTextHelpFormatter
import time
import numpy as np
import imp

retry_count=3

# Check if OpenCV library is available 
# Abort further execution if image handling is required
cv2_flag = True
try:
	imp.find_module("cv2")
except:
	print "WARNING: Couldn't find OpenCV module: cv2"
	print "WARNING: All image related operation will fail!!"
	cv2_flag = False
else:
	import cv2


def isFloat(data):
	try:
		float(data)
	except ValueError:
		return False
	return True

def processFICSV(arglist):
	ficsv = {}
	if os.path.exists(str(arglist.ficsv)):
		fcsv = open(str(arglist.ficsv),'r')
		content = fcsv.readlines()
		for i in range(1,len(content)):
			line = content[i]
			fields = line.split(";")
			fields[len(fields)-1]=fields[len(fields)-1].replace('\n','')
			key = fields[0]
			del fields[0]
			if key not in ficsv.keys():
				value=[]
				value.append(fields)
				ficsv.update({key:value})
			else:
				ficsv[key].append(fields)
		fcsv.close()
	return ficsv
	
def compareTxtfiles(file1,file2):
	if filecmp.cmp(file1,file2):
		print "Both files are identical!"
		return [True,0.0]
	else:
		print "Both files are not identical!"
		return [False,1.0]

def genImgfiles(file1,file2,file3):	
	img1 = cv2.imread(file1)
	img2 = cv2.imread(file2)
	
	if len(img1)==0 or len(img2)==0:
		print "Error:invalid image file"
		sys.exit(-1)

	if len(img1) != len(img2):
		print "Error:image resolution mismatch"
		sys.exit(-1)
	
	if img1.size != img2.size:
		print "Error:Image resolution mismatch"
		sys.exit(-1)

	if img1.size == 0 or img2.size==0:
		print "Error:Invalid image resolution"
		sys.exit(-1)
		
	img3 = img1.copy()
	x = img1.shape[1]
	y = img1.shape[0]
	for i in range(y):
		for j in range(x):
			img3[i][j][0] = float(img1[i][j][0])-float(img2[i][j][0])
			img3[i][j][1] = float(img1[i][j][1])-float(img2[i][j][1])
			img3[i][j][2] = float(img1[i][j][2])-float(img2[i][j][2])
	
	cv2.imwrite(file3,img3)
	return

def compareNumfiles(file1,file2,threshold):
	if not os.path.exists(file1) or not os.path.exists(file1):
		print "Error:one of the file does not exist.."
		sys.exit(-1)
	f1 = open(file1,"r")
	f2 = open(file2,"r")
	content1 = f1.readlines()
	content2 = f2.readlines()
	if len(content1) != len(content2):
		return [False,-1.0] # sdc - number of values are unequal
	if len(content1)==0 or len(content2)==0:
		return [True,-1.0] # not a sdc - one of the file has no content
	l2norm = 0.0
	linf=0.0
	for i in range(len(content1)):
		if not isFloat(content1[i].split('\n')[0]) or not isFloat(content2[i].split('\n')[0]):
			continue
		val1 = float(content1[i].split('\n')[0])
		val2 = float(content2[i].split('\n')[0])
		l2norm = l2norm + ((val1-val2)*(val1-val2))
		diff = abs(val1-val2)
		if diff > linf:
			linf=diff
	l2norm = math.sqrt(l2norm)
	if linf > threshold:
		print "L2 Norm is: " + str(l2norm)
		print "L-infinity Norm is: " + str(linf)
		return [False,linf]
	elif linf != linf or linf == float('Inf') or linf == -float('Inf'):
		return [False,-1.0]
	f1.close()
	f2.close()
	return [True,linf]

def compareImgfiles(file1,file2,threshold):
	maxfile1=0
	mse=0.0
	psnr=0.0
	img1 = cv2.imread(file1)
	img2 = cv2.imread(file2)
	
	if img1.size != img2.size:
		print "Warning:Image resolution mismatch"
		return [False,-1.0]

	if img1.size == 0 or img2.size==0:
		print "Warning:Invalid image resolution"
		return [False,-1.0]
		
	x = img1.shape[1]
	y = img1.shape[0]
	for i in range(y):
		for j in range(x):
			if max(img1[i][j]) > maxfile1:
				maxfile1=max(img1[i][j])

	mse = np.sum((img1.astype("float") - img2.astype("float")) ** 2)
	mse = mse / float(img1.size)
	
	print "Image resolution: " + str(x)+"x"+str(y)
	print "MSE: "+str(mse)
	print "MAX_I: "+str(maxfile1)
	
	if mse > 0:
		maxsqr = maxfile1**2
		ratio = maxsqr / mse
		if ratio > 0.0:
			psnr = math.log10(ratio) * 10.0
			print "PSNR: "+str(psnr)
			if psnr >= threshold:
				print "Info: psnr is above threshold!"
				return [True,psnr]
			else:
				print "Info: psnr is below threshold!"
				return [False,psnr]
		else:
			print "Warning: ratio of maxi and mse should always be gt 0"
			return [True,-1.0]
			
	print "Info: The two images are identical!"
	return [True,0.0]

def printg(txt):
	line = '\033[1;42m'
	line = line + txt
	line = line + '\033[1;m'
	print line 
	return
	 
def writeOverheadResult(arglist,exectime1,exectime2):
	key = str(time.time())
	fld2 = arglist.cmd1
	fld3 = arglist.cmd1
	fld4 = str(exectime1)
	fld5 = str(exectime2)
	
	outline = key + ","
	outline = outline + fld2 + ","
	outline = outline + fld3 + ","
	outline = outline + fld4 + ","
	outline = outline + fld5 + "\n"
	
	# write result
	if os.path.exists(arglist.rslt):
		#write result
		frslt = open(arglist.rslt,'a')
		frslt.write(outline)
		frslt.close()
	else:
		#write header and result
		frslt = open(arglist.rslt,'w')
		frslt.write("exec_key,exe1_name,exe2_name,\
		exe1_time,exe2_time\n")
		frslt.write(outline)
		frslt.close()
	return 

# field 1 - key
# field 2 - instructions
# field 3 - original value
# field 4 - corrupted value
# field 5 - corrupted bit position
# field 6 - sdc
# field 7 - benign
# field 8 - crash
# field 9 - error
def writeResult(arglist,result,diffval,fsCount,detectCount):
	key = ''
	fld2 = ''
	fld3 = ''
	fld4 = ''
	fld5 = ''
	fld6 = str(result[0])
	fld7 = str(result[1])
	fld8 = str(result[2])
	fld9 = str(result[3])
	fld10 = str(diffval)
	fld11 = str(fsCount)
	fld12 = str(detectCount)
	
	# read execution key
	if os.path.exists("fi.out"):
		fi = open("fi.out",'r')
		contentfi = fi.readlines()
		for i in range(len(contentfi)):
			if "timestamp" in contentfi[i]:
				key = str(contentfi[i].split(":")[1])
				key = key.replace("\n","")				
		fi.close()
	else:
		key = 'Not Available'
	
	# read fi data from fi.csv and combine it with the result
	ficsv = processFICSV(arglist)
	if key in ficsv.keys():
		value = ficsv[key]
		if len(value)>=1:
			fld2 = (value[0][0]).replace(",","#") # instr name
			fld3 = value[0][1] # original val
			fld4 = value[0][2] # corrupted val
			fld5 = value[0][3] # corrupted bit pos
		elif len(value)>=2:
			for j in range(1,len(value)):
				fld2 = fld2 + "\n" + value[j][0] # instr name
				fld3 = fld3 + "\n" + value[j][1] # original val
				fld4 = fld4 + "\n" + value[j][2] # corrupted val
				fld5 = fld5 + "\n" + value[j][3] # corrupted bit pos
	outline = key + ","
	outline = outline + fld2 + ","
	outline = outline + fld3 + ","
	outline = outline + fld4 + ","
	outline = outline + fld5 + ","
	outline = outline + fld6 + ","
	outline = outline + fld7 + ","
	outline = outline + fld8 + ","
	outline = outline + fld9 + ","
	outline = outline + fld10 + ","
	outline = outline + fld11 + ","
	outline = outline + fld12 + "\n"
	
	# write result
	if os.path.exists(arglist.rslt):
		#write result
		frslt = open(arglist.rslt,'a')
		frslt.write(outline)
		frslt.close()
	else:
		#write header and result
		frslt = open(arglist.rslt,'w')
		frslt.write("exec_key,instr_list,original_val,corrupted_val,affected_bit_pos,sdc,benign,crash,error,value_diff,dyn_instr_count,detection_count\n")
		frslt.write(outline)
		frslt.close()
	return 

def printResult(arglist,result):
	# total execution count
	printg("Total number of executions: " + str(arglist.exec_count))
	# silent data corruptions
	printg("Number of executions with sdc: " + str(result[0]))
	# benign excutions
	printg("Number of benign executions: " + str(result[1]))
	# This should ideally not happen
	printg("Number of executions which potentially crashed: " + str(result[2]))
	# This should always be 0
	printg("Number of errorneous executions: " + str(result[3]))
	return 

def analyzeResult(arglist,retcode):
	# result[0/1/2/3] - sdc,benign,crash,error
	result = [0,0,0,0]
	val = True
	# potential error
	if not os.path.exists(arglist.out1) or os.stat(arglist.out1).st_size == 0:
		result[3]=result[3]+1
		return [result,1.0]
	# potential crash
	if retcode != 0 or not os.path.exists(arglist.out2) or os.stat(arglist.out2).st_size == 0:
		result[2]=result[2]+1
		return [result,1.0]
	# check for sdc
	if arglist.fcp_mode == 'bex':
		val = compareTxtfiles(arglist.out1,arglist.out2)
	elif arglist.fcp_mode == 'img':
		val = compareImgfiles(arglist.out1,arglist.out2,arglist.th)	
	elif arglist.fcp_mode == 'num':
		val = compareNumfiles(arglist.out1,arglist.out2,arglist.th)	
	if val[0] == False: # sdc found
		result[0]=result[0]+1
	else:
		result[1]=result[1]+1
	return [result,val[1]]

def genFIFile(arglist,pfs_flag,fsCount,procIdx):
	fl = open("fi.in",'w')
	# write fi algo
	fl.write("fia:"+str(arglist.fia).upper()+"\n")
	# write fib
	fl.write("fib:"+str(arglist.fib)+"\n")
	# write fid
	fl.write("fid:"+str(arglist.fid).upper()+"\n")
	# write fbu
	fl.write("fiu:"+str(arglist.fbu)+"\n")
	# write fbl
	fl.write("fil:"+str(arglist.fbl)+"\n")
	# write probability
	fl.write("pn:"+str(arglist.num_prob)+"\n")
	fl.write("pd:"+str(arglist.den_prob)+"\n")
	# write fault injection count
	fl.write("fc:"+str(fsCount)+"\n")
	# write pfs_flag
	if pfs_flag == True:
		fl.write("pfs:1")
	else:
		fl.write("pfs:0")
	if procIdx > -1:
		fl.write("\nprocid:"+str(procIdx))
	fl.close()
	return


def getProcIdx(arglist):
	procIdx=-1
	if os.path.exists("fs.out"):
		fl = open("fs.out",'r')
		content = fl.readlines()
		for i in range(len(content)):
			line = content[i]
			if "targetrank" in line:
				fields = line.split(":")
				procIdx=int(fields[1])
				break
		fl.close()
	if procIdx > -1:
		print "Chosen Process Index : " + str(procIdx)
	return procIdx

def getFaultSiteCount(arglist):	
	fsCount=0
	if os.path.exists("fs.out"):
		fl = open("fs.out",'r')
		content = fl.readlines()
		for i in range(len(content)):
			line = content[i]
			if "fault site count" in line:
				fields = line.split(":")
				fsCount=int(fields[1])
				break
		fl.close()
	print "Fault site count : " + str(fsCount)
	return fsCount

def getFaultInjCount(arglist):
	injCount=0
	found_flag = False
	if os.path.exists("fi.out"):
		fl = open("fi.out",'r')
		content = fl.readlines()
		for i in range(len(content)):
			line = content[i]
			if "fault injection count" in line:
				print line
				fields = line.split(":")
				injCount=int(fields[1])
				found_flag = True
				break
		fl.close()
	return injCount

def getErrorDetectionCount(arglist):
	detectCount=0
	found_flag = False
	if os.path.exists("fi.out"):
		fl = open("fi.out",'r')
		content = fl.readlines()
		for i in range(len(content)):
			line = content[i]
			if "error detection count" in line:
				print line
				fields = line.split(":")
				detectCount=int(fields[1])
				found_flag = True
				break
		fl.close()
	return detectCount


def isValidExecInst(arglist,retcode):
	# if a program crash occured then treat that as a valid instance
	injCount=0
	if  not os.path.exists(arglist.out2):
		injCount=injCount+1
	elif retcode != 0: # crash
		injCount=injCount+1
	else:
		injCount=getFaultInjCount(arglist)
	if injCount<=0:
		return False
	if arglist.prob_ov == True and injCount == 1:
		return True
	if arglist.ficount_flag == 'eql' and injCount == arglist.ficount:
		return True
	if arglist.ficount_flag == 'max' and injCount <= arglist.ficount:
		return True
	if arglist.ficount_flag == 'min' and injCount >= arglist.ficount:
		return True
	if arglist.ficount_flag == 'nlm':
		return True
	print "Error injection count is: " + str(injCount)
	return False

def runCmd(exe,arg):
	cmdline = "time "+ exe + " " + arg
	ret = subprocess.call(cmdline,shell=True)
	print cmdline
	if ret!=0:
		print "Info: execution of "+cmdline+" received a signal - " + str(ret)
	return ret

def getExecTime(flname):
	exec_time = 0.0
	if os.path.exists(flname):
		f = open(flname, "r")
		content = f.readlines()
		exec_time = float((content[0].split(":")[1]).replace(" ",""))
		f.close()
	return exec_time

def runExec(arglist):
	overheadlst = []
	if arglist.overhead_flag == True:
		for i in range(arglist.exec_count):
			# calculate overhead
			start_time = time.time()
			runCmd(arglist.exec1,arglist.cmd1)
			e1 = time.time() - start_time
			#~ e1=getExecTime("execution_time")
			
			start_time = time.time()
			runCmd(arglist.exec2,arglist.cmd2)
			e2 = time.time() - start_time
			#~ e2=getExecTime("execution_time")
			writeOverheadResult(arglist,e1,e2)
			print "Overhead: " + str(((e2-e1)/e1)*100) + "%"
	else:
		# result[0/1/2/3] - sdc,benign,crash,error
		result = [0,0,0,0]
		val=[0,0,0,0]
		exec_count = arglist.exec_count
		for i in range(exec_count):
			if (arglist.cho == False) or \
			(arglist.cho == True and not os.path.exists(arglist.out1)):
				runCmd(arglist.exec1,arglist.cmd1)
			if arglist.cp_mode == True:
				if (arglist.cfs == False) or \
				(arglist.cfs == True and not os.path.exists("fs.out")):
					genFIFile(arglist,True,0,-1)
					runCmd(arglist.exec2,arglist.cmd2)
				fsCount = getFaultSiteCount(arglist)
				procIdx = getProcIdx(arglist)				
				if fsCount<=0:
					print "Error:fault site count should be always > 0."
					sys.exit(-1)
				if arglist.prob_ov == True:
					# override user provided fault injection probability
					arglist.num_prob=1
					arglist.den_prob=fsCount
				genFIFile(arglist,False,fsCount,procIdx) # writes to fi.in
				runCmd("rm","-f fi.out")
				retcode = runCmd(arglist.exec2,arglist.cmd2)
				print "return code is: " + str(retcode)
				retval = analyzeResult(arglist,retcode) 
				val = retval[0]
				#~ while not isValidExecInst(arglist,retcode):
				exec_success = False
				for j in range(retry_count):
					if isValidExecInst(arglist,retcode):
						exec_success = True
						break
					print "Current execution instance is not valid!"
					print "Retrying.."
					runCmd("rm","-f fi.out")
					retcode = runCmd(arglist.exec2,arglist.cmd2)
					print "return code is: " + str(retcode)
					retval = analyzeResult(arglist,retcode) 
					val = retval[0]
				if exec_success == True:
					result[0] = result[0] + val[0]
					result[1] = result[1] + val[1]
					result[2] = result[2] + val[2]
					result[3] = result[3] + val[3]
					detectCount = getErrorDetectionCount(arglist)
					writeResult(arglist,val,retval[1],fsCount,detectCount)
					runCmd("rm","-f "+arglist.out2)
		printResult(arglist,result)	
	return

def initParser():
	parser = argparse.ArgumentParser(description='Process commandline options..',formatter_class=RawTextHelpFormatter)	
	# execution mode args
	parser.add_argument('-e',action='store_true',dest='exec_mode',help='execution mode.')
	parser.add_argument('-s',action='store_true',dest='sa_mode',help='standalone mode, always used in combination with -e option.')
	parser.add_argument('-c',action='store_true',dest='cp_mode',help='comparison mode, always used in combination with -e option.')
	parser.add_argument('--ov',action='store_true',dest='overhead_flag',help='Calculate overhead')
	#~ parser.add_argument('--ovfl1',action='store',dest='ov_file1',help='name of the file containing execution time')
	#~ parser.add_argument('--ovfl2',action='store',dest='ov_file2',help='name of the file containing execution time')
	parser.add_argument('--pv',action='store_true',dest='prob_ov',help='override user provided fault injection probability..')
	parser.add_argument('--iter',action='store',dest='exec_count',help='Number of iterations.',type=int,default=1)
	parser.add_argument('--fcp',action='store',dest='fcp_mode',help='file comparison mode.',choices=['bex','img','num'],default='bex')
	parser.add_argument('--th',action='store',dest='th',help='threshold value, only valid with --fcp=img or num.',type=float,default=30.0)
	parser.add_argument('--exec1',action='store',dest='exec1',help='location of the 1st executable to be executed with -s or -c mode.')
	parser.add_argument('--cmd1',action='store',dest='cmd1',help='command line args for the exe provided using --exec1 option.')
	parser.add_argument('--out1',action='store',dest='out1',help='name (including path) of the output generated by --exec1 option.')
	parser.add_argument('--cho',action='store_true',dest='cho',help='this flag indicates that use cached output for exec1 if available')
	parser.add_argument('--cfs',action='store_true',dest='cfs',help='this flag indicates that use cached fault site count')
	parser.add_argument('--exec2',action='store',dest='exec2',help='location of the instrumented executable to be executed only with -c.')
	parser.add_argument('--cmd2',action='store',dest='cmd2',help='command line args for the exe provided using --exec2 option.')
	parser.add_argument('--out2',action='store',dest='out2',help='name (including path) of the output generated by --exec2 option.')
	parser.add_argument('--rslt',action='store',dest='rslt',help='name of the output file where result will be written')
	parser.add_argument('--ficsv',action='store',dest='ficsv',help='name of the input csv file containing intermediate fi data.')
	# below options will be used by exec2
	parser.add_argument('--fia',action='store',dest='fia', choices=['cbr','abr','msk'],
	help='fault injection algorithm, valid options: \n \
	cbr - cumulative byte ordering; target one or more contiguous bytes for fault injection.\n\
	abr - absolute bit range;target a bit-range for fault injection\n',default='cbr')
	parser.add_argument('--fib',action='store',dest='fib', choices=[1,2,3,4,5,6,7,8],type=int,
	help='no. of contiguous bytes to be considered for fault injection\n\
	Note:this option must be provided when using -fia=cbr',default=8)
	parser.add_argument('--fid',action='store',dest='fid', choices=['msb','lsb'],
	help='direction from which no. of contiguous bytes to be considered for fault injection\n\
	Note:this option must be provided when using -fia=cbr',default='lsb')
	parser.add_argument('--fbu',action='store',dest='fbu', type=int,
	help='upper bound for the fault injection bit-range.\n\
	Note:this option must be provided when using -fia=abr',default=63)
	parser.add_argument('--fbl',action='store',dest='fbl', type=int,
	help='lower bound for the fault injection bit-range.\n\
	Note:this option must be provided when using -fia=abr',default=0)
	parser.add_argument('--np',action='store',dest='num_prob', type=int,
	help='numerator of fault injection probability expressed as a fraction.',default=1)
	parser.add_argument('--dp',action='store',dest='den_prob', type=int,
	help='denominator of fault injection probability expressed as a fraction.',default=1)
	parser.add_argument('--ff',action='store',dest='ficount_flag', choices=['eql','max','min','nlm'],
	help='puts restriction on no. of faults that will be injected, valid options:\n\
	eql - execution with fault injection count equal to the val provided by --fc.\n\
	max - execution with fault injection count at max equal to the val provided by --fc.\n\
	min - execution with fault injection count at least equal to the val provided by --fc.\n\
	nlm - execution with fault injection count having no limit.\n',default='eql')
	parser.add_argument('--fc',action='store',dest='ficount',type=int,
	help='fault injection countm this value is used in combination with --ff.',default=1)
	
	# parse args
	result = parser.parse_args()
	
	# perform additional validation
	if result.exec_mode == False:
		print "Error:-e must be chosen.."
		sys.exit(-1)
	
	if result.exec_mode == True:
		if result.sa_mode == True and result.cp_mode == True:
			print "Error:only one of the option -s or -c could be chosen.."
			sys.exit(-1)
		if result.sa_mode == False and result.cp_mode == False:
			print "Error:atleast one of the option -s or -c should be chosen.."
			sys.exit(-1)
			
	if result.sa_mode == True:
		if not os.path.exists(str(result.exec1)):
			print "Error:executable 1 does not exist!"
			sys.exit(-1)
		if result.out1 == "":
			print "Error:the output field for exec 1 can't be empty"
			sys.exit(-1)
	elif result.cp_mode == True:
		if not os.path.exists(str(result.exec1)) or not os.path.exists(str(result.exec2)):
			print "Error:executable 1 or executable 2 does not exist!"
			sys.exit(-1)
		if result.out1 == "" or result.out2 == "":
			print "Error:the output field for exec 1 or exec 2 can't be empty"
			sys.exit(-1)
	
	if result.num_prob <=0 or result.den_prob <=0 or result.num_prob > result.den_prob:
		print "Error: invalid value for fault injection probability!"
		sys.exit(-1)
	
	if result.fbu>63 or result.fbl<0 or result.fbu<result.fbl:
		print "Error: invalid bit-range!"
		sys.exit(-1)
		
	if cv2_flag == False and result.fcp_mode == 'img':
		print "Error: couldn't find OpenCV module cv2 while the image handling mode is enabled!!"
		print "Aborting rest of the execution.."
		sys.exit(-1)
		
	return result

if __name__ == "__main__":
	arglist = initParser()
	if arglist.exec_mode == True:
		runExec(arglist)
