<template>
  <div id="m-playlists">
    <m-loader v-if="loading"></m-loader>
    <div v-else>
      <ul>
        <li v-for="playlist in playlists" v-on:click="play(playlist)">
          {{ playlist }}
        </li>
      </ul>
    </div>
  </div>
</template>

<script>
  import axios from 'axios'
  import Loader from '@/components/Loader'

  export default {
    components: {
      'm-loader': Loader
    },

    data () {
      return {
        playlists: [],
        loading: false
      }
    },

    methods: {
      play: function (playlist) {
        this.$musciteer.send({ event: 'play', data: 'pl:tag:' + playlist })
      }
    },

    created () {
      axios.get('/api/playlists').then(
        response => {
          this.playlists = response.data
          this.loading = false
        }
      )
      this.loading = true
    }
  }
</script>

<style lang="scss">
  @import "../styles/mixins.scss";

  #m-playlists
  {
    height: 100%;

    >div
    {
      color: #666;

      ul
      {
        list-style-type: none;

        li
        {
          display: flex;
          padding: 0.66em 1em;
          border-bottom: 1px solid #eee;

          &:hover {
            background-color: lighten(complement(rgba(241, 239, 236, 1)), 4%);
          }
        }
      }
    }
  }
</style>
