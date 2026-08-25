# Maintainer: Asem Arafa <asemarafa at users.noreply.github.com>

_pkgname="nchat"
pkgname="$_pkgname-git"
pkgver=5.20.9.r910.g2005c36
pkgrel=1
pkgdesc="Terminal-based Telegram, WhatsApp, and Signal client"
arch=('x86_64' 'aarch64')
url="https://github.com/asemarafa/nchat"
license=('AGPL-3.0-or-later')

depends=(
  'file'
  'libpng'
  'libxcb'
  'ncurses'
  'openssl'
  'readline'
  'sqlite'
  'zlib'
)

makedepends=(
  'ccache'
  'clang'
  'cmake'
  'git'
  'go'
  'gperf'
  'ninja'
  'pkgconf'
  'protobuf'
  'rust'
)

provides=("$_pkgname")
conflicts=("$_pkgname")
options=('!lto')

_branch="feature/command-bar"
source=("$_pkgname::git+https://github.com/asemarafa/nchat.git#branch=$_branch")
sha256sums=('SKIP')

pkgver() {
  cd "$_pkgname"
  local _ver
  _ver=$(grep -oP '(?<=NCHAT_VERSION ")[^"]+' lib/common/src/version.h 2>/dev/null || echo "5.20.9")
  printf "%s.r%s.g%s" "$_ver" "$(git rev-list --count HEAD)" "$(git rev-parse --short=7 HEAD)"
}

build() {
  cd "$_pkgname"

  export GOFLAGS+=' -buildvcs=false'

  local _cmake_options=(
    -B build
    -S .
    -G Ninja
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_INSTALL_PREFIX='/usr'
    -DCMAKE_INSTALL_MANDIR='share/man'
    -DHAS_SIGNAL=ON
    -DDOWNLOAD_LIBSIGNAL=OFF
    -Wno-dev
  )

  cmake "${_cmake_options[@]}"
  local _jobs=$(( $(nproc) > 6 ? 6 : $(nproc) ))
  cmake --build build -j "$_jobs"
}

package() {
  cd "$_pkgname"
  DESTDIR="$pkgdir" cmake --install build
  install -Dm644 LICENSE -t "$pkgdir/usr/share/licenses/$pkgname/"
  install -Dm644 build/THIRD_PARTY_LICENSES -t "$pkgdir/usr/share/licenses/$pkgname/"
}
