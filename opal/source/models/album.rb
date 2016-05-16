class Album
  attr_accessor :id
  attr_accessor :title
  attr_accessor :cover

  def initialize(attrs)
    attrs.each do |k,v|
      instance_variable_set("@#{k}", v) unless v.nil?
    end
  end

  def artist
    @artist['name']
  end

  def tracks
    @tracks ||= []
  end
end