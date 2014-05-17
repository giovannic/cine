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

supporting_sources = ['launcher/Launcher.cpp', 'profiler/Instrumenter.cpp', 'profiler/Controller.cpp', 'profiler/Analyser.cpp']
launcher_test_sources = ['ctests/launcher_test.cpp'] + supporting_sources
test_env.Program('bin/launcher_test', launcher_test_sources)
instrument_test_sources = ['ctests/instrument_test.cpp'] + supporting_sources
test_env.Program('bin/instrument_test', instrument_test_sources)
result_test_sources = ['ctests/result_test.cpp'] + supporting_sources
test_env.Program('bin/result_test', result_test_sources)

#libcine
cine_env = Environment()
cine_sources = ['profiler/cine.cpp']
cine_env.Append(CPPPATH=['profiler/h', dyninstinc, vexinc])
cine_env.Append(LIBPATH=[dyninstlib, vexlib])
cine_env.Append(LIBS=['pthread'])
#cine_env.Append(CCFLAGS=['-O0'])
cine = cine_env.SharedLibrary('lib/cine.so', cine_sources)
