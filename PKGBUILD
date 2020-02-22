# Maintainer: Reberti Carvalho Soares <6reberti6@gmail.com>
pkgname=RShell
pkgver=2.0
pkgrel=4
epoch=1
pkgdesc="KisuYami own shell."
arch=('i686' 'x86_64')
url='www.github.com/KisuYami/RShell'
license=('GPLv2')
depends=('ncurses' 'readline')
makedepends=('ncurses' 'readline')
options=()
source=('git://github.com/KisuYami/RShell')
md5sums=('SKIP')

build() {
  cd "$pkgname/"
  make
}

package() {
  cd "$pkgname/"

  mkdir -p "$pkgdir"/usr/bin/
  mkdir -p "$pkgdir"/usr/share/man/man1/
  mkdir -p "$pkgdir"/usr/share/man/pt_BR/man1/

  make DESTDIR="$pkgdir" install
}

# vim:set ts=2 sw=2 et:
