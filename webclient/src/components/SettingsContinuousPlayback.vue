<template>
  <div>
    <div class="section-content">
      <label>Enable</label>
      <div>
        <input type="checkbox" v-model="enabled" v-on:change="toggle"></input>
      </div>
      <span v-if="enabled">Uncheck to disable continuous playback.</span>
      <span v-else>Check to enable continuous playback.</span>
    </div>
    <div class="section-content">
      <label>Type</label>
      <div>
        <select v-model="type" v-on:change="setType">
          <option v-for="t in types">{{t}}</option>
        </select>
      </div>
      <span>Set continuous playback type.</span>
    </div>
  </div>
</template>

<script>
  import axios from 'axios'

  export default {
    data: function () {
      return {
        enabled: null,
        type: null,
        types: [
          'random', 'less-played', 'more-played', 'top-played'
        ]
      }
    },

    methods: {
      setData: function (data) {
        this.enabled = data.enabled
        this.type = data.type
      },
      toggle: function () {
        this.post({ enabled: this.enabled })
      },
      setType: function () {
        this.post({ type: this.type })
      },
      get: function () {
        axios.get('/api/player/ctpb')
        .then(
          response => {
            this.setData(response.data)
          }
        )
      },
      post: function (data) {
        axios.post('/api/player/ctpb', data)
        .then(
          response => {
            this.setData(response.data)
          }
        )
      }
    },

    created () {
      this.get()
    }
  }
</script>

<style lang="scss">
</style>
