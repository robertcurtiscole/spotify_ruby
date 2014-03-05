require 'mkmf'

extension_name = 'spotify_ruby'
include_dirs = ['$(srcdir)']
dir_config(extension_name, include_dirs)

unless have_header('libspotify/api.h')
  puts "please install spotify headers"
  puts "please get spotify library and appkey.c"
  exit
end

puts "ruby version " + RUBY_VERSION.to_s
puts "ruby platform " + RUBY_PLATFORM.to_s

#CFLAGS for LINUX
#$CFLAGS << ' -DPKGLIBDIR="\"/usr/local/lib/mpg123\"" -DOPT_MULTI -DOPT_GENERIC -DOPT_GENERIC_DITHER -DOPT_I386 -DOPT_I586 -DOPT_I586_DITHER -DOPT_MMX -DOPT_3DNOW -DOPT_3DNOW_VINTAGE -DOPT_3DNOWEXT -DOPT_3DNOWEXT_VINTAGE -DOPT_SSE -DOPT_SSE_VINTAGE -DREAL_IS_FLOAT -DNEWOLD_WRITE_SAMPLE   -O2 -fomit-frame-pointer -funroll-all-loops -finline-functions -ffast-math '
#CFLAGS for MACOS
#$CFLAGS << ' -DHAVE_CONFIG_H -DPKGLIBDIR="\"/usr/local/lib/mpg123\"" -DOPT_MULTI -DOPT_X86_64 -DOPT_GENERIC -DOPT_GENERIC_DITHER -DREAL_IS_FLOAT   -O2 -fomit-frame-pointer -funroll-all-loops -finline-functions -ffast-math '
$LDFLAGS << ' -framework OpenAL -framework libspotify '
#CFLAGS for ARM
#$CFLAGS << ' -DHAVE_CONFIG_H -DPKGLIBDIR="\"/usr/local/lib/mpg123\"" -DOPT_ARM -DOPT -DREAL_IS_FIXED   -O2 -fomit-frame-pointer -funroll-all-loops -finline-functions -ffast-math '
#$LDFLAGS << ' -O2 -fomit-frame-pointer -funroll-all-loops -finline-functions -ffast-math '
create_makefile(extension_name)    # spotify_ruby/spotify_ruby