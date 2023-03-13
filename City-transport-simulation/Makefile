all:
	cd ./src && qmake -o Makefile icp.pro && $(MAKE)
	
run: all
	cd ./src && ./icp
	
doxygen: *
	mkdir ./doc/Doxygen && doxygen ./doc/Doxyfile

pack: clean
	zip xotcen01.zip ./src ./examples ./doc ./README.txt ./Makefile ./files
	
clean:
	rm -r -f ./doc/Doxygen
	find src/. -name 'Makefile' -delete
	find src/. -name 'icp.pro.user' -delete
	find src/. -name 'icp' -delete
	find src/. -name '*.o' -delete
	find src/. -name 'moc_*.cpp' -delete
	find src/. -name 'moc_*.h' -delete
	find src/. -name 'ui_*.h' -delete
