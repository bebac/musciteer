require 'erb'

require 'bundler'
Bundler.require

OUT_DIR            = "../public"

ASSET_DIR          = File.join(OUT_DIR, "assets")
INDEX_HTML         = File.join(OUT_DIR, "index.html")
APPLICATION_JS     = File.join(ASSET_DIR, "application.js")
APPLICATION_MIN_JS = File.join(ASSET_DIR, "application.min.js")
APPLICATION_CSS    = File.join(ASSET_DIR, "application.css")

ANDROID_HTML       = File.join(OUT_DIR, "android.html")
ANDROID_JS         = File.join(ASSET_DIR, "android.js")
ANDROID_MIN_JS     = File.join(ASSET_DIR, "android.min.js")
ANDROID_CSS        = File.join(ASSET_DIR, "android.css")

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

desc "Build application css"
task :css => [ ASSET_DIR ] do
  sh "bundle exec sass -I styles/scss styles/scss/main.scss #{APPLICATION_CSS}"
end

desc "Minify javascript"
task :minify => [ :javascript ] do
  File.binwrite APPLICATION_MIN_JS, Uglifier.compile(File.read(APPLICATION_JS))
end

desc "Build android.html"
task :android_html => [ OUT_DIR ] do |t|
  renderer = ERB.new(File.read("android.html.erb"))
  File.binwrite ANDROID_HTML, renderer.result(binding)
end

desc "Build android.js"
task :android_javascript => [ ASSET_DIR ] do
  File.binwrite ANDROID_JS, Opal::Builder.build("application-android").to_s
end

desc "Build android css"
task :android_css => [ ASSET_DIR ] do
  sh "bundle exec sass -I styles/scss styles/scss/main-android.scss #{ANDROID_CSS}"
end

desc "Minify android javascript"
task :android_minify => [ :javascript ] do
  File.binwrite ANDROID_MIN_JS, Uglifier.compile(File.read(ANDROID_JS))
end

task :default => [ :html, :android_html, :javascript, :android_javascript, :css, :android_css ]
