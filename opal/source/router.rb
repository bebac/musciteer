class Router
  include Inesita::Router

  def routes
    route '/', to: Main, props: {}
    route '/albums', to: Albums, props: {}
  end
end
