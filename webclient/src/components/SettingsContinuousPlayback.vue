<template>
  <div>
    <div class="section-content">
      <div class="header">
        Enable
      </div>
      <div class="body">
        <input type="checkbox" v-model="enabled" v-on:change="toggle"></input>
      </div>
      <div class="description">
        <span v-if="enabled">Uncheck to disable continuous playback.</span>
        <span v-else>Check to enable continuous playback.</span>
      </div>
    </div>
    <div class="section-content">
      <div class="header">
        Type
      </div>
      <div class="body">
        <select v-model="type" v-on:change="setType">
          <option v-for="t in types">{{t}}</option>
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
      <div class="description">
        <span>Set continuous playback type.</span>
      </div>
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
