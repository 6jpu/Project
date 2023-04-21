all: 
	@echo "Start compile library..."
	make -C src
	@echo "Start compile program..."
	make -C cli 

clean:
	make clean -C src
	make clean -C cli

distclean: clean
	rm -rf lib/*.a
	
