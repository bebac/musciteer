<template>
  <div id="albums-page">
    <m-header></m-header>
    <div id="albums" class="main">
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
    <m-footer></m-footer>
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
        element: null,
        top: 0
      }
    },

    methods: {
      set_scroll_top: function () {
        if (this.element) {
          this.element.scrollTop = this.top
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

    mounted: function (el) {
      this.element = document.getElementById('albums')
    },

    beforeRouteEnter (to, from, next) {
      next(
        vm => {
          vm.set_scroll_top()
        }
      )
    },

    beforeRouteLeave (to, from, next) {
      if (this.element) {
        this.top = this.element.scrollTop
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
