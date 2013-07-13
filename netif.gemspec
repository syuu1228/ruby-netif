# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'netif/version'

Gem::Specification.new do |spec|
  spec.name          = "netif"
  spec.version       = Netif::VERSION
  spec.authors       = ["Takuya ASADA"]
  spec.email         = ["syuu@dokukino.com"]
  spec.description   = %q{Ruby wrapper of netif}
  spec.summary       = %q{Wrapper of netif}
  spec.homepage      = "https://github.com/syuu1228/ruby-netif"
  spec.license       = "Modified BSD License"

  spec.files         = `git ls-files`.split($/)
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib", "ext"]
  spec.extensions    = spec.files.grep(/extconf.rb$/)

  spec.add_development_dependency "bundler", "~> 1.3"
  spec.add_development_dependency "rake"
end
