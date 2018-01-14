<template>
  <div class="section-content">
    <div class="setting-header">
      Output
    </div>
    <div class="setting-body">
      <select v-model="current" v-on:change="set">
        <option v-for="device in devices">{{device}}</option>
      </select>
      <div class="select-dropdown-icon">
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 200 200"
          >
          <g>
            <path d="m30,40l60,80l60,-80" stroke-width="0"></path>
          </g>
        </svg>
      </div>
    </div>
    <div class="setting-description">
      <span>The audio output device.</span>
    </div>
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
