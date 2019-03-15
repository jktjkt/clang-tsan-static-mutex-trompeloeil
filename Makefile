TARGET = doit
INC = -Itrompeloeil/include

$(TARGET): main.o libcheck.a
	$(CXX) $(CXXFLAGS) $^ -o $@

libcheck.a: libcheck.o
	ar rcs $@ $^

main.o: main.cpp class.h mock.h
	$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<

libcheck.o: check.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<

clean:
	rm -f *.o *.a $(TARGET)
