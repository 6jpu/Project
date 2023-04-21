all: 
	@echo "Start compile library..."
	make -C src
	@echo "Start compile program..."
	make -C ser 

clean:
	make clean -C src
	make clean -C ser	

distclean: clean
	rm -rf lib/*.a
	
