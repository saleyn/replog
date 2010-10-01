BOOST_ROOT=/opt/boost_1_41_0
CPPFLAGS = -g -O$(if $(optimize),3 -DBOOST_DISABLE_ASSERTS,0) \
		   $(if $(tr1),-std=c++0x) -I.. -isystem $(BOOST_ROOT)/include
LDFLAGS  = -L$(BOOST_ROOT)/lib -L.


all: test_replog

replog: replog.cpp util.cpp
	g++ -g -o $@ $^ -I.

test_replog: test_proto.cpp test_raw_char.cpp test_buffer.cpp \
		proto.cpp $(wildcard *.hpp)
	g++ -o $@ $(filter-out $(wildcard *.hpp),$^) $(CPPFLAGS) $(LDFLAGS) \
	-DBOOST_TEST_DYN_LINK -lboost_unit_test_framework

