class Router
  include Inesita::Router

  def routes
    route '/', to: Albums, props: {}
    route '/albums', to: Albums, props: {}
    route '/tracks', to: Tracks, props: {}
  end
end
