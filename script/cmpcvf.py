#! /usr/bin/env python

"""
  This script takes as cmd two files, containing results from running
  test-cases for the libiers10++ library, and compares output for
  each function.
  The input files are format-specific. Run the test.e programs in
  test/test.e and test/fortran/test.e to produce them.
"""

import os, sys
import re

##  print usage and exit
def usage () :
  print 'Invalid cmds; Runs as cmpcvf.py <result_file_1> <result_file_2>'
  sys.exit (0)

##  given an input file and a string, search the input file to find the string.
##+ The actual string searched for, is 'Function <string>' at the start of line
##+ using regular expressions. If the string is found, the input buffre is set 
##+ to the line the string was found and 0 is returned; else 1 is returned.
def gotostr (ifile,strng) :
  ifile.seek (0)
  string = '^ *Function '+strng.strip ()
  string = string.replace ('(','\(')
  string = string.replace (')','\)')
  line = ifile.readline ()
  while True :
    result = re.match (string,line)
    if result:
      return 0
    line = ifile.readline ()
    if not line:
      return 1

##  given an input file, with the buffer set at the start of function results
##+ (i.e. where the function gotostr has stoped), this function will make a
##+ a list with the results reported. I.e. for the input file:
##  Abs. differences in microarcseconds:
##  |pm(1)|  = 0.000000132875289438
##  |pm(2)|  = 0.000000444179713455
##+ it will return [microarcseconds,microarcseconds],[pm(1),pm(2)],[.00...,.0...]
def funres (ifin) :
    units = []
    vals  = []
    diffs = []
    linel = ifin.readline ().split ()
    if linel[len (linel) -1] == ':' :
        gunits = linel[len (linel) -2].replace (':','')
    else:
        gunits = linel[len (linel) -1].replace (':','')
    line = ifin.readline ()
    while True:
        if line.lstrip()[0] != '|' :
            return units,vals,diffs
        unit = gunits
        linel = line.split ()
        vals.append ( linel[0].replace('|','') )
        diffs.append ( linel[2] )
        if len (linel) > 3 :
            units.append ( linel[3].replace('(','').replace(')','') )
        else:
            units.append (unit)
        line = ifin.readline ()
        if not line :
            break
    return units,vals,diffs


## check number of command line arguments
if len (sys.argv) != 3 :
  print 'ERROR!'
  usage ()

## try opening input files
try :
  fin1 = open (sys.argv[1], 'r')
  try :
    fin2 = open (sys.argv[2], 'r')
  except:
    print 'ERROR. Could not open file',sys.argv[2]
    sys.exit (1)
except:
  print 'ERROR. Could not open file',sys.argv[1]
  sys.exit (1)

## get a list of all functions in the first file
function_list = []
for line in fin1 :
  gr = re.search (r"^ *Function ",line)
  if gr :
    l = line.split ()
    if len (l) > 2 :
      t = l[1:]
      fun_name = ''
      for i in t :
        fun_name += (i.strip () + ' ')
    else:
      fun_name = l[1]
    function_list.append ( fun_name )
fin1.seek (0)

print '---------------------------------------------------------------------------'
print 'Analysis of test results for libiers10++'
print 'Generated by cmpcvf.py'
print 'Comparisson of C++ and Fortran results per function'
print 'ARGUMENT     TEST-C++  TEST-FOR  FOR-C++'
print '========== : ========= ========= ========='

for f in function_list:
  i = gotostr (fin1,f)
  j = gotostr (fin2,f)
  if i != 0 :
    print '## Failed to locate function',f,'in file',sys.argv[1]
  if j !=0 :
    print '## Failed to locate function',f,'in file',sys.argv[2]
  if i+j == 0 :
    u1,v1,d1 = funres (fin1)
    u2,v2,d2 = funres (fin2)
    diffs1 = []
    diffs2 = []
    diffs12= []
    try :
      for i in range (0,len (u1)):
        diffs1.append ( float (d1[i]) )
        diffs2.append ( float (d2[i]) )
        diffs12.append( abs ( diffs1[i] - diffs2[i] ) )
    except:
      print '## Failed to resolve diffs for function',f
      u1 = []
    if len (u1) > 0 and len (u2) > 0 :
      print 'Function',f
      for i in range (0,len (u1)):
        #print 'cpp: ',v1[i],d1[i],u1[i]
        #print 'for: ',v2[i],d2[i],u2[i]
        if v1[i] == v2[i]:
          s = '%10s : %.3e %.3e %.3e %20s' % (v1[i],diffs1[i],diffs2[i],diffs12[i],u1[i])
          print s
        else:
          print '## Result values do not match for function',f

## close the input files
fin1.close ()
fin2.close ()

print '---------------------------------------------------------------------------'