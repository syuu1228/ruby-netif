# -*- encoding: utf-8 -*-
$:.push File.expand_path("../lib", __FILE__)
require "netif/version"

Gem::Specification.new do |s|
  s.name        = "netif"
  s.version     = Netif::VERSION
  s.authors     = ["Takuya ASADA"]
  s.email       = ["syuu@dokukino.com"]
  s.homepage    = "https://github.com/syuu1228/ruby-netif"
  s.summary     = %q{Network Interface configuration APIs for Ruby}
  s.description = %q{Network Interface configuration APIs for Ruby}

  s.files         = `git ls-files`.split("\n")
  s.test_files    = `git ls-files -- {test,spec,features}/*`.split("\n")
  s.executables   = `git ls-files -- bin/*`.split("\n").map{ |f| File.basename(f) }
  s.extensions    = `git ls-files -- ext/*.rb`.split("\n")
  s.require_paths = ["lib"]

  s.add_development_dependency 'rake-compiler'
  s.required_ruby_version = ">= 1.9.2"
end

