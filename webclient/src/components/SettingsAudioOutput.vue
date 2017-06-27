<template>
  <div class="section-content">
    <label>Output</label>
    <div>
      <select v-model="current" v-on:change="set">
        <option v-for="device in devices">{{device}}</option>
      </select>
    </div>
    <span>The audio output device.</span>
  </div>
</template>

<script>
  import axios from 'axios'

  export default {
    data: function () {
      return {
        current: null,
        devices: []
      }
    },

    methods: {
      setData: function (data) {
        this.current = data.current
        this.devices = data.devices
      },
      set: function () {
        this.post({ current: this.current })
      },
      get: function () {
        axios.get('/api/player/output')
        .then(
          response => {
            this.setData(response.data)
          }
        )
      },
      post: function (data) {
        axios.post('/api/player/output', data)
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
