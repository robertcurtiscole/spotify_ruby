require "bundler/gem_tasks"
require 'rake/extensiontask'

spec = Gem::Specification.load('spotify_ruby.gemspec')

#Rake::ExtensionTask.new('spotify_ruby', spec)
Rake::ExtensionTask.new do |ext|
  ext.name = 'spotify_ruby'                # indicate the name of the extension.
  #ext.ext_dir = 'ext/weird_world'         # search for 'hello_world' inside it.
  ext.lib_dir = 'lib/spotify_ruby'              # put binaries into this folder.
  ext.config_script = 'spotify_ruby/extconf.rb' # use instead of the default 'extconf.rb'.
  #ext.tmp_dir = 'tmp'                     # temporary folder used during compilation.
  #ext.source_pattern = "*.{c,cpp}"        # monitor file changes to allow simple rebuild.
  #ext.config_options << '--with-foo'      # supply additional options to configure script.
  #ext.config_includes << "ext/spotify_ruby/maci686inc"
  ext.gem_spec = spec                     # optionally indicate which gem specification will be used.
end
