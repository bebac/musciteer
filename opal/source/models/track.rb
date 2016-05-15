class Track
  attr_accessor :id
  attr_accessor :title

  def initialize(attrs)
    attrs.each do |k,v|
      instance_variable_set("@#{k}", v) unless v.nil?
    end
  end

  def artists
    names = Array(@artists).collect { |artist| artist['name'] }
    names.join(",")
  end

  def album
    @album["title"]
  end
end
