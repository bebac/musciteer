<template>
  <div id="m-player-mini">
    <div v-if="stream" class="cover">
      <img v-bind:src="stream.track.album.cover">
    </div>
    <div v-if="stream" class="label">
      <h1>{{stream.track.title}}</h1>
      <h2>{{stream.track.artists[0].name}}</h2>
    </div>
  </div>
</template>

<script>
  export default {
    name: 'm-player-mini',

    data () {
      return {
        stream: null
      }
    },

    methods: {
      stream_sync: function (streamId) {
        this.$musciteer.send({ event: 'stream_data_sync', data: streamId })
      }
    },

    musciteer: {
      player_state: function (data) {
        if (data.state !== 1) {
          this.stream = null
        }
      },
      stream_begin: function (data) {
        this.stream_sync(data.stream_id)
      },
      stream_data: function (data) {
        this.stream = { id: data.stream_id, track: data.track, duration: 0, length: 0 }
      },
      stream_progress: function (data) {
        if (this.stream === null) {
          this.stream_sync(data.stream_id)
        } else if (this.stream.id === data.stream_id) {
          this.stream.duration = data.duration
          this.stream.length = data.length
        } else {
          this.stream_sync(data.stream_id)
        }
      },
      stream_end: function (data) {
        this.stream = null
      }
    }
  }
</script>

<style lang="scss" scoped>
  @import "../styles/mixins.scss";

  #m-player-mini
  {
    display: flex;
    align-items: center;
    height: 4em;
    color: #666;

    .cover
    {
      height: 100%;
      padding: 0 1em;

      img
      {
        height: 100%;
      }
    }

    .label
    {
      h1
      {
        font-size: 0.88em;
        font-weight: bold;
      }

      h2
      {
        font-size: 0.71em;
        font-weight: normal;
      }
    }
  }
</style>
