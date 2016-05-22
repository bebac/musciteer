class QueueUpdate
  attr_reader :queue_size
  attr_reader :track

  def initialize(attrs)
    attrs.each do |k,v|
      case k
      when "track"
        @track = Track.new(v)
      else
        instance_variable_set("@#{k}", v) unless v.nil?
      end
    end
  end
end

class StreamBegin
  attr_reader :stream_id

  def initialize(attrs)
    attrs.each do |k,v|
      instance_variable_set("@#{k}", v) unless v.nil?
    end
  end
end
