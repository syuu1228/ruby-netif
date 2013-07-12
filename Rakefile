begin
  load 'Rakefile.local'
rescue LoadError
  ## Ignore
end

GEM_SPEC = begin
  require 'bundler/gem_helper'
  helper = Bundler::GemHelper.new(Dir.pwd)
  helper.install
  helper.gemspec
rescue LoadError
  fname = File.basename(Dir.pwd).sub(%r#^(?:ruby-)?(.+?)(?:-\d.*)?$#, '\1.gemspec')
  contents = File.read(fname)
  eval(contents, TOPLEVEL_BINDING, fname)
end

EXT_NAME = GEM_SPEC.name.gsub(/-/, '_')

## ======================================================================

require 'rake/clean'

CLEAN.include('pkg', 'tmp', "lib/#{EXT_NAME}.so")

## ======================================================================

require 'rake/extensiontask'

Rake::ExtensionTask.new(EXT_NAME, GEM_SPEC) do |task|
  task.source_pattern = '*.{c,h}'
end

## ======================================================================

require 'rake/testtask'

Rake::TestTask.new

## ======================================================================

task :default => [:compile]

task :test => "lib/#{EXT_NAME}.so"

