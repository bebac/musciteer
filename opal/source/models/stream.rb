class Stream
  attr_accessor :id
  attr_accessor :duration
  attr_accessor :length
  attr_accessor :track

  def initialize(attrs)
    attrs.each do |k,v|
      case k
      when "stream_id"
        @id = v
      else
        instance_variable_set("@#{k}", v) unless v.nil?
      end
    end
  end

  def duration_formatted
    if duration
      secs = duration / 1000
      min = Integer(secs / 60) % 60;
      sec = Integer(secs % 60);
      "%d:%02d" % [ min, sec ]
    else
      "-"
    end
  end

  def length_formatted
    if length
      secs = length / 1000
      min = Integer(secs / 60) % 60;
      sec = Integer(secs % 60);
      "%d:%02d" % [ min, sec ]
    else
      "-"
    end
  end

  def time_formatted
    "#{duration_formatted} / #{length_formatted}"
  end
end