dyninst = '/home/giovanni/project/DyninstAPI-8.1.2/dyninstAPI'
dyninstlib = dyninst + '/x86_64-unknown-linux2.4'
dyninstinc = dyninst + '/h'

#CC='clang', CXX='clang++'

test_env = Environment(CPPPATH=['launcher', 'profiler', dyninstinc], LIBS=['dyninstAPI', 'pcontrol', 'elf', 'dwarf'], CCFLAGS='-g', CC='clang', CXX='clang++')
test_sources = ['launcher/launcher.cpp', 'profiler/OProfile.cpp', 'ctests/launcher_test.cpp']

test_env.Program('bin/launcher_test', test_sources)
