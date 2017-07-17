export default {

  install (Vue, url, opts = {}) {
    // Create a new Vue intance to house the websocket.
    this.vm = new Vue({

      data: function () {
        return {
          timer: null,
          player_state: null
        }
      },

      created () {
        this.create_websocket(this)
        this.$on('player_state', function (data) {
          this.player_state = data
        })
      },

      methods: {
        create_websocket (self) {
          let ws = new WebSocket(url)

          ws.onopen = function (event) {
            self.$emit('onopen', ws)
          }

          ws.onclose = function (event) {
            self.$emit('onclose', ws)
            self.reconnect()
          }

          ws.onerror = function (event) {
            self.$emit('onerror', ws)
            self.close()
          }

          ws.onmessage = function (event) {
            let m = JSON.parse(event.data)
            self.$emit(m.event, m.data)
          }

          this.ws = ws
        },
        reconnect () {
          let self = this
          this.timer = setTimeout(
            function () {
              self.create_websocket(self)
            },
            5000
          )
        },
        close () {
          this.ws.close()
        },
        send (data) {
          this.ws.send(JSON.stringify(data))
        }
      }
    })

    Vue.mixin({
      created () {
        let musciteer = this.$options['musciteer']
        for (var event in musciteer) {
          let self = this
          let func = musciteer[event]
          this.$musciteer.$on(event, function (data) {
            func.call(self, data)
          })
        }
      }
    })

    Vue.prototype.$musciteer = this.vm
  }
}
