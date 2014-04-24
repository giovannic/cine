#common paths
dyninst = ARGUMENTS.get('dyninst', '/home/giovanni/dyninst')
dyninstlib = dyninst + '/lib'
dyninstinc = dyninst + '/include'

vex = ARGUMENTS.get('vex', '/home/giovanni/vexjine')
vexagentlib = vex + '/src/jine'
vexinc = vex + '/src/vex'
vexlib = vexinc

dwarf = '/home/giovanni/oss/deps/libdwarf'

#tests

test_env = Environment(LIBPATH = [dwarf, dyninstlib, vexlib, vexagentlib], CCFLAGS='-g')
test_env.Append(LIBS=['dyninstAPI', 'pcontrol', 'dynElf', 'symLite' ,'common', 'elf', 'dwarf', 
					  'instructionAPI', 'stackwalk', 'patchAPI', 'parseAPI', 'symtabAPI', 'dynDwarf', 
					  'vex', 'JVMTIAgent', 'papi'])
test_env.Append(CPPPATH=['ctests/h', 'launcher/h', 'profiler/h', dyninstinc, vexinc])

#test defines
bins = '\\"%s\\"' % Dir('ctests/testinputs').abspath
test_env.Append(CPPDEFINES={'BIN_PATH': bins})

test_sources = ['launcher/launcher.cpp', 'profiler/instrumenter.cpp', 'ctests/launcher_test.cpp']
test_env.Program('bin/launcher_test', test_sources)

#libcine
cine_env = Environment()
cine_sources = ['profiler/cine.cpp']
cine_env.Append(CPPPATH=['profiler/h', dyninstinc])
cine_env.Append(LIBPATH=[dyninstlib])
cine_env.Append(LIBS=['dyninstAPI_RT'])
cine = cine_env.SharedLibrary('bin/cine.so', cine_sources)
