begin
  load 'Rakefile.local'
rescue LoadError
  ## Ignore
end

## ======================================================================

require 'bundler/gem_tasks'

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

CLOBBER.include('pkg')

## ======================================================================

task :default => [:compile]
task :test => [:compile]

