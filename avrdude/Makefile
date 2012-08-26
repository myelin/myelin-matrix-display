install:
	mkdir -p /usr/local/lib
	mkdir -p /usr/local/include
	cp libftd2xx.1.1.12.dylib /usr/local/lib/
	ln -sf /usr/local/lib/libftd2xx.1.1.12.dylib /usr/local/lib/libftd2xx.dylib
	cp WinTypes.h ftd2xx.h /usr/local/include/

	cd avrdude-5.10 && ./configure CFLAGS="-g -O2 -DSUPPORT_FT245R" LIBS="-lftd2xx -lpthread -ldl" && make install
