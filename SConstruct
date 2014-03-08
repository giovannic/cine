from os import path

dyninst = '/home/giovanni/oss/deps'
dyninstlib = dyninst + '/lib64'
dyninstinc = dyninst + '/include/dyninst'

dwarf = '/home/giovanni/oss/deps/libdwarf'

#CC='clang', CXX='clang++'

test_env = Environment(LIBPATH = [dwarf, dyninstlib], CCFLAGS='-g')
test_env.Append(LIBS=['dyninstAPI', 'pcontrol', 'dynElf', 'symLite' ,'common', 'elf', 'dwarf', 
					  'instructionAPI', 'stackwalk', 'patchAPI', 'parseAPI', 'symtabAPI', 'dynDwarf'])
test_env.Append(CPPPATH=['launcher', 'profiler', dyninstinc])

#test defines
bins = '\\"%s\\"' % Dir('ctests/testinputs').abspath
print bins
test_env.Append(CPPDEFINES={'BIN_PATH': bins})

test_sources = ['launcher/launcher.cpp', 'profiler/instrumentor.cpp', 'ctests/launcher_test.cpp']

test_env.Program('bin/launcher_test', test_sources)
