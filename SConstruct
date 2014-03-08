dyninst = '/home/giovanni/oss/deps'
dyninstlib = dyninst + '/lib64'
dyninstinc = dyninst + '/include/dyninst'

dwarf = '/home/giovanni/oss/deps/libdwarf'

#CC='clang', CXX='clang++'

test_env = Environment(CPPPATH=['launcher', 'profiler', dyninstinc], LIBS=['dyninstAPI', 'pcontrol', 'dynElf', 'symLite' ,'common', 'elf', 'dwarf', 'instructionAPI', 'stackwalk', 'patchAPI', 'parseAPI', 'symtabAPI', 'dynDwarf'], LIBPATH = [dwarf, dyninstlib], CCFLAGS='-g')
test_sources = ['launcher/launcher.cpp', 'profiler/instrumentor.cpp', 'ctests/launcher_test.cpp']

test_env.Program('bin/launcher_test', test_sources)
