<template>
  <div id="playlists-page">
    <div class="header">
      <m-header></m-header>
    </div>
    <div class="main">
      <m-queue-update></m-queue-update>
      <div id="m-playlists">
        <m-loader v-if="loading"></m-loader>
        <div v-else>
          <ul>
            <li v-for="playlist in playlists" v-on:click="play(playlist)">
              <m-tag-icon></m-tag-icon>
              <div>
                {{ playlist }}
              </div>
            </li>
          </ul>
        </div>
      </div>
    </div>
    <div class="footer">
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
  import TagIcon from '@/components/TagIcon'

  export default {
    components: {
      'm-header': Header,
      'm-footer': Footer,
      'm-queue-update': QueueUpdate,
      'm-loader': Loader,
      'm-tag-icon': TagIcon
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
  @import "../styles/variables.scss";

  #playlists-page
  {
    background-color: #fff;
  }

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
          align-items: center;
          padding: 0.66em 1em;
          border-bottom: 1px solid #eee;

          &:hover {
            background-color: $color-lst-hover;
          }
        }
      }

      svg
      {
        @include size(1em);
        margin-right: 1em;
        stroke: #666;
        fill: #666;
      }
    }
  }
</style>
