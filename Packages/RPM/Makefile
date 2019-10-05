NAME=RShell
VERSION=1.5

RPMS:

	wget https://github.com/KisuYami/RShell/archive/${VERSION}.tar.gz
	tar xf ${VERSION}.tar.gz

	cp Makefile-rpm RShell-${VERSION}/Makefile
	tar cfzv ${NAME}-${VERSION}.tar.gz ${NAME}-${VERSION}

	rpmdev-setuptree

	cp ${NAME}-${VERSION}.tar.gz ${HOME}/rpmbuild/SOURCES/${NAME}-${VERSION}.tar.gz
	cp rshell.spec ${HOME}/rpmbuild/SPECS/

	rpmbuild -bb ${HOME}/rpmbuild/SPECS/rshell.spec
	cp -r ${HOME}/rpmbuild/RPMS ./

x64: RPMS
	cp RPMS/x86_64/*.x86_64.rpm ./
	rpm --addsign *.x86_64.rpm

clear:
	rm -rd ${NAME}-${VERSION}/
	rm -rd ${HOME}/rpmbuild
	rm -rd RPMS
	rm *.tar*
