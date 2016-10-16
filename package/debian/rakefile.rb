require 'erb'

# Debian package control file.
CONTROL_TPL = ERB.new <<-EOF
Package: <%= name %>
Version: <%= version %>
Section: base
Priority: optional
Architecture: <%= architecture %>
Depends: libasound2, libflac++6v5, libcrypto++9v5, libkyotocabinet16v5
Maintainer: Benny Bach <benny.bach@gmail.com>
Description: Musciteer music player
EOF

# /usr/bin/musciteer
LAUNCH = <<eos
#!/bin/bash
INSTDIR=/usr/share/musciteer
export LD_LIBRARY_PATH+=$INSTDIR/lib/
export SNAP=$INSTDIR
exec $INSTDIR/bin/musciteer
eos

PKG_DIRS = %w(stage/DEBIAN
              stage/usr/bin
              stage/usr/share/musciteer/bin
              stage/usr/share/musciteer/lib
              stage/usr/share/musciteer/public)

desc "Build debian package"
task :dpkg do

  # Create directory structure.
  PKG_DIRS.each do |dir|
    sh "mkdir -p #{dir}"
  end

  name = "musciteer"
  version = "0.1-2"
  architecture = %x[dpkg --print-architecture].strip

  # Create debian control file.
  File.open("stage/DEBIAN/control", "w") do |file|
    file.write(CONTROL_TPL.result(binding))
  end

  # Create musciteer launch script.
  File.open("stage/usr/bin/musciteer", "w") do |file|
    file.write(LAUNCH)
  end
  sh "chmod +x stage/usr/bin/musciteer"

  # Copy binaries.
  sh "cp ../../build/musciteer stage/usr/share/musciteer/bin/"
  sh "cp ../../build/spotify_import stage/usr/share/musciteer/bin/"
  sh "cp ../../build/flac_rg_tagger stage/usr/share/musciteer/bin/"
  sh "cp #{Dir['../../lib/libspotify*'].first}/lib/libspotify.so* stage/usr/share/musciteer/lib/"

  # Copy public.
  sh "cp -r ../../public/* stage/usr/share/musciteer/public/"

  # Build package.
  sh "dpkg-deb --build stage"

  # Rename debian.dep
  sh "mv stage.deb #{name}_#{version}-#{architecture}.deb"
end
