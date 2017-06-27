<template>
  <div id="m-albums">
    <ol>
      <li
        is="album-thumb"
        v-for="album in albumsSortedByArtist"
        v-bind:key="album.id"
        v-bind:album="album"
      >
      </li>
    </ol>
  </div>
</template>

<script>
  import axios from 'axios'
  import AlbumThumb from '@/components/AlbumThumb'

  export default {
    name: 'albums',

    components: {
      AlbumThumb
    },

    data: function () {
      return {
        albums: [],
        top: 0
      }
    },

    methods: {
      set_scroll_top: function () {
        let main = document.getElementById('main')
        if (main) {
          main.scrollTop = this.top
        }
      }
    },

    created () {
      axios.get('/api/albums')
      .then(
        response => { this.albums = response.data }
      )
    },

    computed: {
      albumsSortedByArtist: function () {
        return this.albums.sort(function (x, y) {
          if (x.artist.name < y.artist.name) {
            return -1
          } else if (x.artist.name > y.artist.name) {
            return 1
          } else {
            return 0
          }
        })
      }
    },

    beforeRouteEnter (to, from, next) {
      next(
        vm => {
          vm.set_scroll_top()
        }
      )
    },

    beforeRouteLeave (to, from, next) {
      let main = document.getElementById('main')
      if (main) {
        this.top = main.scrollTop
      }
      next()
    }
  }
</script>

<style lang="scss">
  #m-albums
  {
    margin-top: 1em;

    ol
    {
      display: flex;
      flex-wrap: wrap;
      justify-content: center;
      padding: 0;
      margin: 0;
    }
  }
</style>
