require 'erb'

require 'bundler'
Bundler.require

OUT_DIR            = "../public"
ASSET_DIR          = File.join(OUT_DIR, "assets")
INDEX_HTML         = File.join(OUT_DIR, "index.html")
APPLICATION_JS     = File.join(ASSET_DIR, "application.js")
APPLICATION_MIN_JS = File.join(ASSET_DIR, "application.min.js")
APPLICATION_CSS    = File.join(ASSET_DIR, "application.css")

Opal.append_path File.join(File.dirname(__FILE__), 'source')

directory OUT_DIR
directory ASSET_DIR

desc "Build index.html"
task :html => [ OUT_DIR ] do |t|
  renderer = ERB.new(File.read("index.html.erb"))
  File.binwrite INDEX_HTML, renderer.result(binding)
end

desc "Build application.js"
task :javascript => [ ASSET_DIR ] do
  File.binwrite APPLICATION_JS, Opal::Builder.build("application").to_s
end

desc "Build css"
task :css => [ ASSET_DIR ] do
  sh "bundle exec sass -I styles/scss styles/scss/main.scss #{APPLICATION_CSS}"
end

desc "Minify javascript"
task :minify => [ :javascript ] do
  File.binwrite APPLICATION_MIN_JS, Uglifier.compile(File.read(APPLICATION_JS))
end

task :default => [ :html, :javascript, :css ]
