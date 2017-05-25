guard 'rake', :task => 'html' do
  watch('index.html.erb')
end

guard 'rake', :task => 'css' do
  watch(%r{styles/.+\.scss})
end

guard 'rake', :task => 'javascript' do
  watch(%r{source/.+\.(rb|opalerb)})
end
