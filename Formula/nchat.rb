class Nchat < Formula
  desc "Terminal-based Telegram, WhatsApp, and Signal client"
  homepage "https://github.com/asemarafa/nchat"
  url "https://github.com/asemarafa/nchat.git", branch: "feature/command-bar"
  version_file = File.expand_path("../lib/common/src/version.h", __dir__)
  version File.exist?(version_file) ? File.read(version_file)[/NCHAT_VERSION "([^"]+)"/, 1] : "5.19.10"
  license "AGPL-3.0-or-later"

  head "https://github.com/asemarafa/nchat.git", branch: "master"

  depends_on "ccache" => :build
  depends_on "cmake" => :build
  depends_on "go" => :build
  depends_on "gperf" => :build
  depends_on "pkg-config" => :build
  depends_on "protobuf" => :build
  depends_on "rust" => :build
  depends_on "libmagic"
  depends_on "ncurses"
  depends_on "openssl@3"
  depends_on "readline"
  depends_on "sqlite"

  def install
    require "etc"
    user_config = "#{Etc.getpwuid(Process.uid).dir}/.config/ccache/ccache.conf"
    ENV["CCACHE_CONFIGPATH"] = user_config if File.exist?(user_config)
    ENV["CCACHE_DIR"] = "#{HOMEBREW_CACHE}/ccache"
    ENV["CCACHE_BASEDIR"] = "/private/tmp"
    ENV["CCACHE_NOHASHDIR"] = "true"
    ENV["CCACHE_SLOPPINESS"] = "time_macros,include_file_mtime,include_file_ctime"
    ENV["CCACHE_IGNOREOPTIONS"] = "-ffile-prefix-map=*,-fdebug-prefix-map=*,-fmacro-prefix-map=*"

    args = std_cmake_args + %W[
      -DCMAKE_C_COMPILER_LAUNCHER=#{Formula["ccache"].opt_bin}/ccache
      -DCMAKE_CXX_COMPILER_LAUNCHER=#{Formula["ccache"].opt_bin}/ccache
      -DCCACHE_FOUND=ON
      -DHAS_SIGNAL=ON
      -DDOWNLOAD_LIBSIGNAL=OFF
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
