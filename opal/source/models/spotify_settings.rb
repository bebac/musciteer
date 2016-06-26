class SpotifySettings
  attr_accessor :username
  attr_accessor :password

  def initialize(attrs)
    attrs.each do |k,v|
      instance_variable_set("@#{k}", v) unless v.nil?
    end
    @password ||= ""
  end

  def to_json
    {
      "username" => @username,
      "password" => @password
    }.to_json
  end
end