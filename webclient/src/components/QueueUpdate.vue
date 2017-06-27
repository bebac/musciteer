<template>
  <div id="notification" v-if="showing">
    <div class="header">
      <h1>{{queue_update.queue_size}}</h1>
    </div>
    <div class="body">
      <h2>{{queue_update.track.title}}</h2>
      <h3>{{queue_update.track.artists[0].name}}</h3>
    </div>
  </div>
</template>

<script>
  export default {
    data () {
      return {
        showing: false,
        queue_update: null
      }
    },

    methods: {
      reset_timer: function () {
        let self = this

        if (this.timer) {
          clearTimeout(this.timer)
        }

        this.timer = setTimeout(function () {
          self.showing = false
        }, 4000)
      }
    },

    musciteer: {
      queue_update: function (data) {
        this.queue_update = data
        this.showing = true
        this.reset_timer()
      }
    }
  }
</script>

<style lang="scss">
  #notification
  {
    position: fixed;
    top: 4em;
    left: 50%;
    transform: translate(-50%);
    background: rgba(33, 33, 33, 0.9);
    width: 40%;
    min-width: 320px;
    max-width: 100%;
    height: 6em;
    box-shadow: 0px 0px 60px 0px rgba(0,0,0,0.75);
    display: flex;
    align-items: center;
    z-index: 9999;

    .header, .body
    {
      background: rgba(33, 33, 33, 0);
      border: none;
    }

    .header
    {
      flex: 0 0 auto;
      min-width: 0;
      color: #eee;

      h1
      {
        padding: 1em;
      }
    }

    .body
    {
      flex: 1;
      min-width: 0;
      padding: 0 1em 0 0;

      h2
      {
        color: #eee;
      }

      h3
      {
        color: #999;
      }
    }

    h1, h2, h3
    {
      text-overflow: ellipsis;
      white-space: nowrap;
      overflow: hidden;
    }
  }
</style>
