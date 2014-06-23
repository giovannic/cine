#common paths
dyninst = ARGUMENTS.get('dyninst', '/home/giovanni/dyninst')
dyninstlib = dyninst + '/lib'
dyninstinc = dyninst + '/include'

vex = ARGUMENTS.get('vex', '/home/giovanni/vexjine')
vexagentlib = vex + '/src/jine'
vexinc = vex + '/src/vex'
vexlib = vexinc

dwarf = '/home/giovanni/oss/deps/libdwarf'

debug = ARGUMENTS.get('debug', 0)
prof = ARGUMENTS.get('prof', 0)
dest = ARGUMENTS.get('dest', '')


#tests

dyninstLibs = ['dyninstAPI', 'common', 'symtabAPI', 'instructionAPI', 'parseAPI', 'patchAPI', 'stackwalk', 'pcontrol', 'dynElf', 'dynDwarf', 'symLite']

cflags = '-g -std=c++11'
if prof:
  cflags = cflags + ' -pg'
test_env = Environment(LIBPATH = [dwarf, dyninstlib, vexlib, vexagentlib], CCFLAGS= cflags)

#this really needs to be separated
test_env.Append(LIBS= dyninstLibs +  ['vex', 'JVMTIAgent', 'papi', 'pthread'])
test_env.Append(CPPPATH=['ctests/h', 'launcher/h', 'profiler/h', dyninstinc, vexinc])
if debug:
	test_env.Append(CPPDEFINES=['CINEDEBUG'])

#test defines
bins = '\\"%s\\"' % Dir('ctests/testinputs').abspath
test_env.Append(CPPDEFINES={'BIN_PATH': bins})

supporting_sources = ['launcher/DynamicLauncher.cpp', 'launcher/StaticLauncher.cpp', 'launcher/Launcher.cpp', 'profiler/Instrumenter.cpp', 'profiler/Controller.cpp', 'profiler/Analyser.cpp', 'profiler/AsyncListener.cpp', 'profiler/Listener.cpp']
launcher_test_sources = ['ctests/launcher_test.cpp'] + supporting_sources
#test_env.Program(dest + 'bin/tests/launcher_test', launcher_test_sources)
instrument_test_sources = ['ctests/instrument_test.cpp'] + supporting_sources
#test_env.Program(dest + 'bin/tests/instrument_test', instrument_test_sources)
result_test_sources = ['ctests/result_test.cpp'] + supporting_sources
#test_env.Program(dest + 'bin/tests/result_test', result_test_sources)

main_sources = ['launcher/main.cpp'] + supporting_sources
test_env.Program(dest + 'bin/cine', main_sources)

#libcine
cine_env = Environment()
cine_sources = ['profiler/collector/cine.cpp']
cine_env.Append(CPPPATH=['profiler/h', 'profiler/collector/h', dyninstinc, vexinc])
cine_env.Append(LIBPATH=[dyninstlib, vexlib])
cine_env.Append(LIBS=['pthread', 'vex', 'dyninstAPI_RT'])
cine_env.Append(CCFLAGS=cflags)
if debug:
	cine_env.Append(CPPDEFINES=['CINEDEBUG'])

cine = cine_env.SharedLibrary(dest + 'lib/cine.so', cine_sources)

