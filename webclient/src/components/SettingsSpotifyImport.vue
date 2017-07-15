<template>
  <div>
    <div class="section-content">
      <div class="header">
        URL
      </div>
      <div class="body">
        <input type="text" v-model="url"></input>
        <button class="body-button" v-on:click="importURL">
          <span>import</span>
          <div v-if="importing" class="busy">
            <div></div>
          </div>
          <div v-else class="armed">
          </div>
        </button>
      </div>
      <div class="description">
        <span>Enter spotify track or album url to import. Examples:
          <ul>
            <li>/v1/albums/08ibdX8K0GETv20UEku3sm</li>
            <li>https://open.spotify.com/album/6WDjkO2VpWFLugL4kl13db</li>
          </ul>
        </span>
      </div>
    </div>
<!--    <div class="section-content">
      <div class="header">
      </div>
      <div class="body">
        <button v-on:click="importURL">
            <span>import</span>
            <div v-if="importing" class="busy">
              <div></div>
            </div>
            <div v-else class="armed">
            </div>
          </button>
      </div>
      <div class="description">
        <span>{{temp}}</span>
      </div>
    </div>-->
  </div>
</template>

<script>
  import axios from 'axios'

  const inputFormats = [
    { re: /^https:\/\/open\.spotify\.com\/(album)\/(.*)/ },
    { re: /^https:\/\/play\.spotify\.com\/(album)\/(.*)/ },
    { re: /^https:\/\/open\.spotify\.com\/(track)\/(.*)/ },
    { re: /^https:\/\/play\.spotify\.com\/(track)\/(.*)/ }
  ]

  export default {
    data: function () {
      return {
        importing: false,
        url: '',
        temp: ''
      }
    },

    methods: {
      importURL: function () {
        let url = null

        for (let format of inputFormats) {
          let match = format.re.exec(this.url)
          if (match) {
            url = '/v1/' + match[1] + 's/' + match[2]
            break
          } else {
            continue
          }
        }

        if (!url) {
          url = this.url
        }

        axios.post('/api/spotify/import', { url: url })
        .then(
          response => {
            this.importing = false
          }
        )
        this.importing = true
      }
    },

    created () {
    }
  }
</script>

<style lang="scss">
</style>
