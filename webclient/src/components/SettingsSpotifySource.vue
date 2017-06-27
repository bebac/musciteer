<template>
  <div>
    <div class="section-content">
      <label>Username</label>
      <div>
        <input type="text" v-model="username"></input>
      </div>
      <span v-if="ok">{{message}}</span>
      <span v-else>Spotify premium username</span>
    </div>
    <div class="section-content">
      <label>Password</label>
      <div>
        <input type="password" v-model="password"></input>
      </div>
      <span v-if="ok"></span>
      <span v-else>Spotify premium password</span>
    </div>
    <div class="section-content">
      <label></label>
      <div>
        <button v-on:click="activate">activate</button>
      </div>
      <span></span>
    </div>
  </div>
</template>

<script>
  import axios from 'axios'

  export default {
    data: function () {
      return {
        username: '',
        password: '',
        ok: false,
        message: ''
      }
    },

    methods: {
      activate: function () {
        axios.post('/api/sources/spotify/settings', { username: this.username, password: this.password })
        .then(
          response => {
            console.log(response.data)
          }
        )
      }
    },

    musciteer: {
      source_notification: function (data) {
        if (data.source_name === 'spotify') {
          if (data.message === 'No error') {
            this.ok = true
            this.message = 'Spotify OK!'
          } else {
            this.ok = false
            this.message = data.message
          }
        }
      }
    },

    created () {
      axios.get('/api/sources/spotify/settings')
      .then(
        response => {
          this.username = response.data.username
        }
      )
    }
  }
</script>

<style lang="scss">
</style>
