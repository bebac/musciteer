export default {

  install (Vue, url, opts = {}) {
    Vue.mixin({
      methods: {
        format_mmss: function (milliseconds) {
          let seconds = milliseconds / 1000
          let min = Math.floor(seconds / 60)
          let sec = seconds % 60

          min = min.toFixed(0)
          sec = sec.toFixed(0)

          if (sec < 10) {
            sec = '0' + sec
          }

          return `${min}:${sec}`
        }
      }
    })
  }
}
