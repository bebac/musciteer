// The Vue build version to load with the `import` command
// (runtime-only or standalone) has been set in webpack.base.conf with an alias.
import Vue from 'vue'
import App from './App'
import router from './router'
import TimeFormat from '@/plugins/time_format'
import Socket from '@/plugins/musciteer_socket'

Vue.config.productionTip = false

Vue.use(TimeFormat)
Vue.use(Socket, 'ws://' + window.location.host)
// Vue.use(Socket, 'ws://localhost:8214')

router.afterEach((to, from, next) => {
  let h = document.getElementById('header')
  let f = document.getElementById('footer')

  if (h) {
    if (to.name === 'Player') {
      h.style.display = 'none'
    } else {
      h.style.display = 'block'
    }
  }

  if (f) {
    if (to.name === 'Player') {
      f.style.display = 'none'
    } else {
      f.style.display = 'block'
    }
  }
})

/* eslint-disable no-new */
new Vue({
  el: '#app',
  router,
  template: '<App/>',
  components: { App }
})
