require 'mkmf'

extension_name = 'spotify_ruby'
include_dirs = ['$(srcdir)']
dir_config(extension_name, include_dirs)

unless have_header('libspotify/api.h')
  puts "please install spotify headers"
  puts "please get spotify library and appkey.c"
  exit
end

if RUBY_PLATFORM =~ /linux/i
	unless have_header('alsa/asoundlib.h')
		puts "please install alsa sound development headers"
		exit
	end
	unless have_library('spotify')
	  puts "please get spotify library and appkey.c"
	  exit
	end		
	unless have_library('asound')
	  puts "please get alsa libasound"
	  exit
	end		
end

if RUBY_PLATFORM =~ /i686-linux/i
	#CFLAGS for LINUX
	#$CFLAGS << ' -DPKGLIBDIR="\"/usr/local/lib/mpg123\"" -DOPT_MULTI -DOPT_GENERIC -DOPT_GENERIC_DITHER -DOPT_I386 -DOPT_I586 -DOPT_I586_DITHER -DOPT_MMX -DOPT_3DNOW -DOPT_3DNOW_VINTAGE -DOPT_3DNOWEXT -DOPT_3DNOWEXT_VINTAGE -DOPT_SSE -DOPT_SSE_VINTAGE -DREAL_IS_FLOAT -DNEWOLD_WRITE_SAMPLE   -O2 -fomit-frame-pointer -funroll-all-loops -finline-functions -ffast-math '
elsif RUBY_PLATFORM =~ /darwin/i
	#CFLAGS for MACOS
	#$CFLAGS << ' -DHAVE_CONFIG_H -DPKGLIBDIR="\"/usr/local/lib/mpg123\"" -DOPT_MULTI -DOPT_X86_64 -DOPT_GENERIC -DOPT_GENERIC_DITHER -DREAL_IS_FLOAT   -O2 -fomit-frame-pointer -funroll-all-loops -finline-functions -ffast-math '
	$CFLATGS << ' -DOPEN_AL_BUILD '
	$LDFLAGS << ' -framework OpenAL -framework libspotify '
elsif RUBY_PLATFORM =~ /arm-linux/i
	#CFLAGS for ARM
	#$CFLAGS << ' -DHAVE_CONFIG_H -DPKGLIBDIR="\"/usr/local/lib/mpg123\"" -DOPT_ARM -DOPT -DREAL_IS_FIXED   -O2 -fomit-frame-pointer -funroll-all-loops -finline-functions -ffast-math '
	#$LDFLAGS << ' -O2 -fomit-frame-pointer -funroll-all-loops -finline-functions -ffast-math '
else
	puts "I don't know how to build on " + RUBY_PLATFORM.to_s
	exit
end
create_makefile(extension_name)    # spotify_ruby/spotify_ruby