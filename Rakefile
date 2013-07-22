begin
  load 'Rakefile.local'
rescue LoadError
  ## Ignore
end

## ======================================================================

require 'bundler'
require 'bundler/gem_tasks'

Bundler.setup

## ======================================================================

require 'rake/extensiontask'

Rake::ExtensionTask.new('netif') do |ext|
  ext.lib_dir = 'lib/netif'
end

## ======================================================================

require 'rake/testtask'

Rake::TestTask.new

## ======================================================================

require 'rake/clean'

CLEAN.include('pkg')

## ======================================================================

task :default => [:compile]
task :test => [:compile]

