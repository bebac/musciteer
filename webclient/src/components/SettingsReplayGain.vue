<template>
  <div class="section-content">
    <div class="setting-header">
      Replay Gain
    </div>
    <div class="setting-body">
      <input type="checkbox" v-model="enabled" v-on:change="toggle"></input>
    </div>
    <div class="setting-description">
      <span v-if="enabled">Uncheck to disable replay gain.</span>
      <span v-else>Check to enable replay gain.</span>
    </div>
  </div>
</template>

<script>
  import axios from 'axios'

  export default {
    data: function () {
      return {
        enabled: null
      }
    },

    methods: {
      toggle: function () {
        axios.post('/api/player/replaygain', { enabled: this.enabled })
        .then(
          response => {
            this.enabled = response.data.enabled
          }
        )
      }
    },

    created () {
      axios.get('/api/player/replaygain')
      .then(
        response => {
          this.enabled = response.data.enabled
        }
      )
    }
  }
</script>

<style lang="scss">
</style>
