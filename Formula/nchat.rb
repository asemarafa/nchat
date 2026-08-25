class Nchat < Formula
  desc "Terminal-based Telegram and WhatsApp client"
  homepage "https://github.com/asemarafa/nchat"
  url "https://github.com/asemarafa/nchat.git", branch: "feature/command-bar"
  version_file = File.expand_path("../lib/common/src/version.h", __dir__)
  version File.read(version_file)[/NCHAT_VERSION "([^"]+)"/, 1]
  license "GPL-3.0-or-later"

  head "https://github.com/asemarafa/nchat.git", branch: "master"

  depends_on "ccache" => :build
  depends_on "cmake" => :build
  depends_on "go" => :build
  depends_on "gperf" => :build
  depends_on "pkg-config" => :build
  depends_on "libmagic"
  depends_on "ncurses"
  depends_on "openssl@3"
  depends_on "readline"
  depends_on "sqlite"

  def install
    ENV["CCACHE_DIR"] = "#{HOMEBREW_CACHE}/ccache"
    ENV["CCACHE_BASEDIR"] = buildpath

    args = std_cmake_args + %W[
      -DCCACHE_FOUND=ON
      -DNCURSES_ROOT_DIR=#{Formula["ncurses"].opt_prefix}
      -DOPENSSL_ROOT_DIR=#{Formula["openssl@3"].opt_prefix}
      -DSQLITE_ROOT_DIR=#{Formula["sqlite"].opt_prefix}
      -DREADLINE_ROOT_DIR=#{Formula["readline"].opt_prefix}
      -DCMAKE_INSTALL_MANDIR=#{man}
    ]

    system "cmake", "-S", ".", "-B", "build", *args
    system "cmake", "--build", "build"
    system "cmake", "--install", "build"
  end

  test do
    assert_match "nchat", shell_output("#{bin}/nchat --version")
  end
end
