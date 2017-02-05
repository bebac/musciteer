class Artist
  attr_reader :id
  attr_reader :name
  attr_reader :albums

  def initialize(attrs)
    attrs.each do |k,v|
      instance_variable_set("@#{k}", v) unless v.nil?
    end
    @albums = albums.map do |al|
      Album.new(al.merge({ artist: { id: id, name: name } }))
    end
  end
end