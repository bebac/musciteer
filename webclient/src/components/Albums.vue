<template>
  <div id="albums-page">
    <div id="header">
      <m-header></m-header>
    </div>
    <div id="main">
      <m-queue-update></m-queue-update>
      <div id="m-albums">
        <m-loader v-if="loading"></m-loader>
        <ol v-else>
          <li
            is="m-album-thumb"
            v-for="album in albumsSortedByArtist"
            v-bind:key="album.id"
            v-bind:album="album"
          >
          </li>
        </ol>
      </div>
    </div>
    <div id="footer">
      <m-footer></m-footer>
    </div>
  </div>
</template>

<script>
  import axios from 'axios'
  import Header from '@/components/Header'
  import Footer from '@/components/Footer'
  import QueueUpdate from '@/components/QueueUpdate'
  import Loader from '@/components/Loader'
  import AlbumThumb from '@/components/AlbumThumb'

  export default {
    name: 'albums',

    components: {
      'm-header': Header,
      'm-footer': Footer,
      'm-queue-update': QueueUpdate,
      'm-loader': Loader,
      'm-album-thumb': AlbumThumb
    },

    data: function () {
      return {
        loading: false,
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
        response => {
          this.albums = response.data
          this.loading = false
        }
      )
      this.loading = true
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
  @import "../styles/mixins.scss";
  @import "../styles/variables.scss";

  #albums-page
  {
    background-color: #fff;
  }

  #m-albums
  {
    height: 100%;

    >div
    {
      height: 100%;
    }

    @media (min-width: $break)
    {
      ol
      {
        display: flex;
        flex-wrap: wrap;
        justify-content: center;
        padding: 0;
        margin: 0;
        margin-top: 1em;
      }
    }

    @media (max-width: $break)
    {
      ol
      {
        display: flex;
        flex-direction: column;

        li
        {
          display: flex;
          border-bottom: 1px solid #eee;
        }
      }
    }
  }
</style>
