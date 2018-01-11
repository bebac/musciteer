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
})

router.beforeEach((to, from, next) => {
  let body = document.getElementsByTagName('BODY')[0]

  if (from.name === 'Player') {
    body.style.backgroundColor = '#333'
  } else {
    body.style.backgroundColor = '#fff'
  }

  next()
})

/* eslint-disable no-new */
new Vue({
  el: '#app',
  router,
  template: '<App/>',
  components: { App }
})
