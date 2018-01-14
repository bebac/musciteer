<template>
  <div>
    <div class="section-content">
      <div class="setting-header">
        Authorization
      </div>
      <div class="setting-body">
        <button v-if="authorized" v-on:click="revoke">revoke</button>
        <button v-else>
          <a v-bind:href="authURL()">authorize</a>
        </button>
      </div>
      <div class="setting-description">
        <span v-if="authorized">Revoke spotify web access</span>
        <span v-else>Authorize spotify web access</span>
      </div>
    </div>
  </div>
</template>

<script>
  import axios from 'axios'

  export default {
    data: function () {
      return {
        authorized: false
      }
    },

    methods: {
      authURL: function () {
        return '/api/spotify/authorize?host=' + encodeURIComponent(document.location.host)
      },
      revoke: function () {
        axios.post('/api/spotify/revoke')
        .then(
          response => {
            this.authorized = response.data.authorized
          }
        )
      }
    },

    created () {
      axios.get('/api/spotify/settings')
      .then(
        response => {
          this.authorized = response.data.authorized
        }
      )
    }
  }
</script>

<style lang="scss">
</style>
