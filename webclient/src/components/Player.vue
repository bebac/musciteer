<template>
  <div id="player">
    <div id="player-background">
      <img v-bind:src="stream.track.album.cover" v-if="stream">
    </div>
    <div id="player-header">
      <m-player-hide></m-player-hide>
    </div>
    <div id="player-content">
      <div class="container">
        <img v-bind:src="stream.track.album.cover" v-if="stream">
      </div>
      <div class="container">
        <div class="title">
          <div v-if="stream">{{stream.track.title}}</div>
        </div>
        <div class="artist-album">
          <div>
            <div v-if="stream">{{stream.track.artists[0].name}}</div>
            <div v-if="stream">{{stream.track.album.title}}</div>
          </div>
        </div>
        <div class="controls">
          <div class="controls-buttons">
            <div>
            </div>
            <m-player-toggle></m-player-toggle>
            <m-player-skip></m-player-skip>
          </div>
          <div class="progress">
            <div v-if="stream">{{stream_progress}}</div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
  import PlayerHide from '@/components/PlayerHide'
  import PlayerToggle from '@/components/PlayerToggle'
  import PlayerSkip from '@/components/PlayerSkip'

  export default {
    components: {
      'm-player-hide': PlayerHide,
      'm-player-toggle': PlayerToggle,
      'm-player-skip': PlayerSkip
    },

    data () {
      return {
        stream: null
      }
    },

    computed: {
      stream_progress: function () {
        return this.format_mmss(this.stream.duration) + ' / ' + this.format_mmss(this.stream.length)
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
    },

    mounted: function () {
      let h = document.getElementById('header')
      let f = document.getElementById('footer')

      if (h) {
        h.style.display = 'none'
      }

      if (f) {
        f.style.display = 'none'
      }
    }
  }
</script>

<style lang="scss">
  @import "../styles/mixins.scss";
  @import "../styles/button.scss";

  #player
  {
    display: flex;
    flex-direction: column;
    height: 100%;
    background-color: #222;
    padding: 1em;

    .button
    {
      svg
      {
        stroke: #aaa;
        fill: #aaa;
      }
    }

    .button-click
    {
      &::after
      {
        background: rgba(0, 0, 0, 0.1);
      }
    }

    #player-background
    {
      position: fixed;
      top: -1em;
      left: -1em;
      right: 0;
      bottom: 0;

      img
      {
        width: 100%;
        height: 100%;
        filter: blur(100px) contrast(50%);
        -webkit-filter: blur(100px) contrast(50%);
        opacity: 0.5;
      }
    }


    #player-hide-button
    {
      flex: 0 0 auto;
    }

    #player-content
    {
      display: flex;
      flex: 1;
      z-index: 1;

      .container
      {
        flex: 1;
        padding: 1em 1em;

        &:nth-child(1)
        {
          display: flex;

          img
          {
            align-self: center;
            width: 100%;
            max-width: 100%;
            height: auto;
            max-height: 100%;
          }
        }

        &:nth-child(2)
        {
          display: flex;
          flex-direction: column;
          padding: 1em 1em;

          .title
          {
            display: flex;
            justify-content: center;
            align-items: center;
            flex: 1;
            color: #eee;
            font-size: 2.5em;
            font-weight: bold;

            >div {
              text-align: center;
            }
          }

          .artist-album
          {
            display: flex;
            justify-content: center;
            align-items: center;
            flex: 1;

            >div
            {
              >div:nth-child(1)
              {
                color: #eee;
                font-size: 1.25em;
                font-weight: bold;
                text-align: center;
              }

              >div:nth-child(2)
              {
                color: #aaa;
                font-size: 1em;
                font-weight: normal;
                text-align: center;
              }
            }
          }

          .controls
          {
            display: flex;
            flex-direction: column;
            flex: 1;
            //background-color: #08f;

            .controls-buttons
            {
              display: flex;
              align-items: center;
              flex: 1;

              .button
              {
                @include size(4em);
              }

              >div
              {
                display: flex;
                justify-content: center;
                flex: 1;
              }
            }

            .progress
            {
              display: flex;
              align-items: center;
              flex: 1;

              >div
              {
                display: flex;
                justify-content: center;
                flex: 1;
                color: #aaa;
                font-size: 1em;
                font-weight: normal;
              }
            }
          }
        }
      }
    }
  }
</style>
