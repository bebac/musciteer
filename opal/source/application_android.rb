require 'opal'
require 'opal-eventable'
require 'browser'
require 'browser/socket'
require 'browser/http'
require 'browser/location'
require 'browser/interval'
require 'browser/delay'
require 'browser/effects'
require 'json'
require 'inesita'

require 'store'
require 'router'
require 'layout_android'

require_tree './components'
require_tree './models'

$document.ready do
  Inesita::Application.new(
    router: Router,
    store: Store,
    layout: Layout
  ).mount_to($document.body)
end
