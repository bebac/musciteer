<template>
  <div id="album">
    <div v-if="album">
      <div class="album-header">
        <div>
          <img v-bind:src="album.cover">
        </div>
        <div>
          <div class="title">
            <h1>{{album.title}}</h1>
            <h2>{{album.artist.name}}</h2>
          </div>
          <div class="actions">
          </div>
        </div>
      </div>
      <div class="album-tracks">
        <ol>
          <li
            v-for="item in discs"
            v-on:click="queue(item)"
            :class="item.is_header ? 'disc-header' : 'track'"
          >
            <div v-if="item.is_track">
              <div class="tn">{{ item.tn }}</div>
              <div class="title">{{ item.title }}</div>
            </div>
            <div v-if="item.is_track">
              <div class="duration">{{ item.duration_mmss }}</div>
            </div>
            <div v-if="item.is_header">
              Disc {{item.dn}}
            </div>
          </li>
        </ol>
      </div>
    </div>
  </div>
</template>

<script>
  import axios from 'axios'

  export default {
    name: 'album',

    props: [
      'albumId'
    ],

    data () {
      return {
        album: null
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
            tracks.push({is_header: true, dn: keys[i]})
          }
          tracks = tracks.concat(discs[keys[i]])
        }

        return tracks
      }
    },

    methods: {
      queue: function (item) {
        if (item.is_track) {
          this.$musciteer.send({ event: 'queue', data: item.id })
        }
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

    mounted () {
      console.log('album mounted')
    }
  }
</script>

<style lang="scss" scoped>
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
    }

    img
    {
      max-width: 12em;
      min-width: 12em;
    }

    ol
    {
      padding: 0;
      margin: 0;

      li
      {
        display: flex;
        padding: 0.66em 1em;
        border-bottom: 1px solid #eee;

        >div
        {
          display: flex;
        }

        >div:nth-child(1)
        {
          justify-content: flex-start;
          flex: 3;
          min-width: 0;
        }

        >div:nth-child(2)
        {
          justify-content: flex-end;
          flex: 1;
        }

        .tn
        {
          width: 1.33em;
          text-align: right;
          flex: 0 0 auto;
        }

        .title
        {
          padding-left: 0.66em;
          text-overflow: ellipsis;
          white-space: nowrap;
          overflow: hidden;
        }

        .duration
        {
          padding-right: 0.66em;
        }
      }

      li:not(.disc-header):hover {
        background-color: lighten(#60656B, 55%);
      }
    }
  }
</style>
