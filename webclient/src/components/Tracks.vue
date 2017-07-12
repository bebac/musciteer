<template>
  <div id="m-tracks">
    <m-loader v-if="loading"></m-loader>
    <div v-else>
      <table>
        <thead ref="thead">
          <tr class="item">
            <th>Track</th>
            <th>Artist</th>
            <th>Album</th>
            <th>Plays</th>
            <th>Skips</th>
            <th v-bind:style="{ paddingRight: srcollBarWidth + 'px' }"></th>
          </tr>
        </thead>
        <tbody ref="tbody" v-on:scroll="scroll">
          <tr class="vspace vspace-top" v-bind:style="{ height: vspaceTop + 'px' }"></tr>
          <tr
            is="m-track"
            class="item item-data"
            v-for="track in tracksSlice"
            v-bind:key="track.id"
            v-bind:track="track"
          >
          </tr>
          <tr class="vspace vspace-bottom" v-bind:style="{ height: vspaceBottom + 'px' }"></tr>
        </tbody>
      </table>
    </div>
  </div>
</template>

<script>
  import axios from 'axios'
  import Loader from '@/components/Loader'
  import TracksItem from '@/components/TracksItem'

  export default {
    components: {
      'm-loader': Loader,
      'm-track': TracksItem
    },

    data: function () {
      return {
        tracks: [],
        start: 0,
        end: 80,
        slice: 80,
        factor: 20,
        batch: 0,
        loading: false,
        top: 0,
        itemHeight: 0,
        vspaceTop: 0,
        vspaceBottom: null,
        tr: null,
        srcollBarWidth: 0
      }
    },

    computed: {
      tracksSlice: function () {
        return this.tracks.slice(this.start, this.end)
      }
    },

    methods: {
      sortTracksByPlays: function (tracks) {
        return tracks.sort(function (x, y) {
          let xscore = x.play_count - x.skip_count
          let yscore = y.play_count - y.skip_count

          if (xscore < yscore) {
            return 1
          } else if (xscore > yscore) {
            return -1
          } else {
            return 0
          }
        })
      },
      saveScrollTop: function (element) {
        if (element) {
          this.top = element.scrollTop
        }
      },
      restoreScrollTop: function (element) {
        if (element) {
          element.scrollTop = this.top
        }
      },
      scroll: function (evt) {
        let top = evt.target.scrollTop
        let len = this.tracks.length
        let factor = this.factor
        let batchHeight = this.factor * this.itemHeight
        let offset = top - batchHeight
        let batch = Math.floor(offset / batchHeight)
        let batchMax = len / factor
        if (this.batch !== batch) {
          if (batch >= 0 && batch < batchMax) {
            this.start = batch * factor
            if (this.start + this.slice > len) {
              this.end = this.start + (len - this.start)
            } else {
              this.end = this.start + this.slice
            }
            this.vspaceTop = this.start * this.itemHeight
            this.vspaceBottom = (len * this.itemHeight) - this.vspaceTop - ((this.end - this.start) * this.itemHeight)
            this.batch = batch
          } else {
            this.start = 0
            this.end = this.slice
            this.vspaceTop = 0
            this.vspaceBottom = (len * this.itemHeight) - this.vspaceTop
            this.batch = 0
          }
        }
      }
    },

    created () {
      axios.get('/api/tracks?brief=1').then(
        response => {
          this.tracks = this.sortTracksByPlays(response.data)
          this.loading = false
        }
      )
      this.loading = true
    },

    beforeUpdate () {
      this.$nextTick(() => {
        // Store a reference to a table row and use it to set the item height.
        if (!this.tr) {
          this.tr = this.$el.querySelector('.item-data')
          if (this.tr) {
            this.itemHeight = this.tr.clientHeight
            if (this.vspaceBottom === null) {
              this.vspaceBottom = (this.tracks.length - this.slice) * this.itemHeight
            }
          }
        }

        // Adjust thead width.
        let thead = this.$refs.thead
        let tbody = this.$refs.tbody

        if (thead && tbody) {
          this.srcollBarWidth = thead.clientWidth - tbody.clientWidth
          // console.log(this.srcollBarWidth)
        }
      })
    },

    beforeRouteEnter (to, from, next) {
      next(
        vm => {
          vm.restoreScrollTop(vm.$refs.tbody)
        }
      )
    },

    beforeRouteLeave (to, from, next) {
      this.saveScrollTop(this.$refs.tbody)
      next()
    }
  }
</script>

<style lang="scss">
  #m-tracks
  {
    height: 100%;

    >div
    {
      height: 100%;
    }

    table
    {
      display: flex;
      flex-direction: column;
      height: 100%;
      width: 100%;
      border-spacing: 0;

      thead
      {
        flex: 0;
        color: #333;
        //background-color: desaturate(darken(rgba(241, 239, 236, 1), 2%), 10%);
        //background-color: lighten(#D4D7DB, 13%);
        box-shadow: 0px -2px 12px rgba(140, 139, 127, 0.4);
        text-transform: uppercase;
        font-weight: bold;
        z-index: 1;

        th
        {
          transform: scaleY(0.68);
        }

        tr.item
        {
          padding: 0.66em 0;
        }
      }

      tbody
      {
        overflow-y: auto;
        color: #666;

        tr.item
        {
          padding: 0.66em 0;
          border-bottom: 1px solid #eee;
        }

        tr.item:hover {
          background-color: lighten(#D4D7DB, 13%);
          //background-color: lighten(#f1efec, 4%);
        }

        tr.vspace {
          display: block;
        }
      }

      tr.item
      {
        display: flex;
        justify-content: flex-start;

        th, td
        {
          flex: 1;
          text-overflow: ellipsis;
          white-space: nowrap;
          overflow: hidden;
        }

        th
        {
          text-align: left;
        }

        th:nth-child(1), td:nth-child(1)
        {
          flex: 5;
        }

        th:nth-child(2), td:nth-child(2)
        {
          flex: 3;
        }

        th:nth-child(3), td:nth-child(3)
        {
          flex: 3;
        }

        th
        {
          padding: 0em 1em;
        }

        th:not(:first-child)
        {
          border-left: 1px solid #888;
        }

        th:last-child
        {
          flex: 0 0 auto;
          border-left: none;
          padding: 0;
        }

        td
        {
          padding: 0em 1em;
        }
      }
    }
  }
</style>
