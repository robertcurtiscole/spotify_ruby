# coding: utf-8
# should this be removed since rubygem will manage the loadpath?
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'spotify_ruby/version'

Gem::Specification.new do |spec|
  spec.name          = "spotify-ruby"
  spec.version       = SpotifyRuby::VERSION
  spec.authors       = ["Curtis Cole"]
  spec.email         = ["RCurtisCole@gmail.com"]
  spec.description   = "api background thread running spotify"
  spec.summary       = "api to spotify library service"
  spec.homepage      = "http://www.github.com/robertcurtiscole/spotifyruby"
  spec.license       = "MIT"

  # TODO: specify platforms where this works
  #spec.platform

  # should the spec.files only have the ruby file?  don't copy the others
  #spec.files         = ['lib/spotify-ruby.rb'] + Dir.glob("ext/**/*.{c,rb}") + Dir.glob("ext/**/maci686inc/*.h")
  #spec.files         = ['lib/spotify-ruby.rb', 'lib/spotify_ruby/version.rb'] + Dir.glob("ext/*/*.c") + Dir.glob("ext/spotify_ruby/include/*.h")
  spec.files         = ['lib/spotify-ruby.rb', 'lib/spotify_ruby/version.rb'] + Dir.glob("ext/*/*.c") + Dir.glob("ext/spotify_ruby/include/*.h")
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib"]

  spec.extensions << "ext/spotify_ruby/extconf.rb"
  spec.add_development_dependency "bundler", "~> 1.3"
  spec.add_development_dependency "rake"
  spec.add_development_dependency "rake-compiler"
end
