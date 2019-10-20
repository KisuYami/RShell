NAME=RShell

${NAME}-${VERSION}.deb:

	wget https://github.com/KisuYami/RShell/archive/${VERSION}.tar.gz
	tar xf ${VERSION}.tar.gz
	rm ${VERSION}.tar.gz

	./build_rshell

	mkdir -p rshell/DEBIAN/
	mkdir -p rshell/usr/bin/
	mkdir -p rshell/usr/share/man/man1/
	mkdir -p rshell/usr/share/man/pt_BR/man1/

	cp rshell.control rshell/DEBIAN/control
	cp ${NAME}-${VERSION}/build/rshell rshell/usr/bin/
	cp ${NAME}-${VERSION}/Packages/man/rshell.1 rshell/usr/share/man/man1/
	cp ${NAME}-${VERSION}/Packages/man/pt_BR/rshell.1 rshell/usr/share/man/pt_BR/man1/

	gzip -f rshell/usr/share/man/man1/rshell.1
	gzip -f rshell/usr/share/man/pt_BR/man1/rshell.1

	dpkg-deb --build rshell
	mv rshell.deb ${NAME}-${VERSION}.x86_64.deb

clear:
	rm -f *.deb
	rm -rdf rshell/
	rm -rdf ${NAME}-${VERSION}
