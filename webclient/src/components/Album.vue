<template>
  <div id="album">
    <div v-if="album" key="album" >
      <div class="album-header">
        <div>
          <div key="album" class="cover">
            <img v-bind:src="album.cover" v-on:error="coverOk = false" v-if="coverOk">
          </div>
        </div>
        <div>
          <div class="title">
            <h1>{{album.title}}</h1>
            <h2>{{album.artist.name}}</h2>
          </div>
          <div class="actions">
            <div>
              <button class="button" v-on:click="play_album">play</button>
            </div>
          </div>
        </div>
      </div>
      <div class="album-tracks">
        <ol>
          <li
            is="m-item"
            v-for="item in discs"
            v-bind:key="item.id"
            v-bind:item="item"
          >
          </li>
        </ol>
      </div>
    </div>
  </div>
</template>

<script>
  import axios from 'axios'
  import AlbumItem from '@/components/AlbumItem'

  export default {
    name: 'album',

    components: {
      'm-item': AlbumItem
    },

    props: [
      'albumId'
    ],

    data () {
      return {
        album: null,
        coverOk: true
      }
    },

    computed: {
      discs: function () {
        // Compare dn / tn
        let compare = function (x, y) {
          if (x.tn < y.tn) {
            return -1
          } else if (x.tn > y.tn) {
            return 1
          } else {
            return 0
          }
        }

        let discs = {}

        for (let track of this.album.tracks) {
          discs[track.dn] = discs[track.dn] || []
          track.is_track = true
          track.duration_mmss = this.format_mmss(track.duration)
          discs[track.dn].push(track)
        }

        for (var dn in discs) {
          discs[dn] = discs[dn].sort(compare)
        }

        let keys = Object.keys(discs).sort(compare)
        let tracks = []

        for (var i = 0; i < keys.length; i++) {
          if (i > 0) {
            tracks.push({is_header: true, id: 'dn-' + keys[i], dn: keys[i]})
          }
          tracks = tracks.concat(discs[keys[i]])
        }

        return tracks
      }
    },

    methods: {
      play_album: function () {
        this.$musciteer.send({ event: 'play', data: this.album.id })
      },
      set_album: function (album) {
        this.album = album
      },
      set_scroll_top: function () {
        let main = document.getElementById('main')
        if (main) {
          main.scrollTop = 0
        }
      }
    },

    beforeRouteEnter (to, from, next) {
      axios.get('/api/albums/' + to.params.albumId)
      .then(
        response => next(
          vm => {
            vm.set_album(response.data)
            vm.set_scroll_top()
          }
        )
      )
    },

    beforeRouteUpdate (to, from, next) {
      console.log('album route update')
    },

    activated () {
      this.coverOk = true
    }
  }
</script>

<style lang="scss" scoped>
  @import "../styles/mixins.scss";
  @import "../styles/variables.scss";
  @import "../styles/button.scss";

  #album
  {
    color: #666;

    .album-header
    {
      display: flex;

      div:nth-child(1)
      {
        padding: 1em;
      }

      div:nth-child(2)
      {
        display: flex;
        flex-direction: column;
        flex: 1;

        >div
        {
          flex: 1;
        }

        .title
        {
          border-bottom: 1px solid #ddd;
        }
      }

      div
      {
        padding: 1em 1em 1em 0;
      }

      h1
      {
        font-size: 1.32em;
      }

      h2
      {
        font-size: 1em;
        font-weight: normal;
      }

      button
      {
        background-color: $color-btn-base;
        border: none;
        border-radius: 20px;
        padding: 0.5em 30px;
        outline: none;
        font-weight: bold;
        color: #666;
        position: relative;

        &:hover {
          background-color: $color-btn-hover;
        }
      }
    }

    .cover
    {
      position: relative;
      height: 12em;
      width: 12em;
      border: 1px solid #ddd;
      background-color: #eee;
      display: block;

      img
      {
        position: absolute;
        top: 0;
        left: 0;
        min-width: 100%;
        max-width: 100%;
        max-height: 12em;
      }
    }

    @media all and (orientation:portrait)
    {
      .album-header
      {
        display: flex;
        flex-direction: column;

        div:nth-child(1)
        {
          padding: 0 0 0 0;

          .cover {
            padding-top: 1em;
          }
        }

        div:nth-child(2)
        {
          padding: 1em;

          .title {
            padding: 1em 0;
          }
        }
      }

      .title
      {
        text-align: center;
      }

      .actions
      {
        text-align: center;
      }

      .cover
      {
        position: relative;
        width: 100%;
        border: none;
        background-color: #fff;

        img
        {
          position: absolute;
          top: 1em;
          left: 0;
          min-width: 100%;
          max-width: 100%;
          max-height: 12em;
          object-fit: contain;
        }
      }
    }

    ol
    {
      padding: 0;
      margin: 0;
    }
  }
</style>
