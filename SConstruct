
dyninst = '/home/giovanni/pre'
dyninstlib = '/home/giovanni/epre/lib'
dyninstinc = '/home/giovanni/pre/include'

vexlib = '/home/giovanni/vexjine/src/vex'
vexagentlib = '/home/giovanni/vexjine/src/jine'
vexinc = '/home/giovanni/vexjine/src/vex'

dwarf = '/home/giovanni/oss/deps/libdwarf'

#CC='clang', CXX='clang++'

test_env = Environment(LIBPATH = [dwarf, dyninstlib, vexlib, vexagentlib], CCFLAGS='-g')
test_env.Append(LIBS=['dyninstAPI', 'pcontrol', 'dynElf', 'symLite' ,'common', 'elf', 'dwarf', 
					  'instructionAPI', 'stackwalk', 'patchAPI', 'parseAPI', 'symtabAPI', 'dynDwarf', 
					  'vex', 'JVMTIAgent', 'papi'])
test_env.Append(CPPPATH=['launcher', 'profiler', dyninstinc, vexinc])

#test defines
bins = '\\"%s\\"' % Dir('ctests/testinputs').abspath

test_env.Append(CPPDEFINES={'BIN_PATH': bins})

test_sources = ['launcher/launcher.cpp', 'profiler/instrumentor.cpp', 'ctests/launcher_test.cpp']

test_env.Program('bin/launcher_test', test_sources)
