class Router
  include Inesita::Router

  def routes
    route '/', to: Main, props: {}
  end
end
