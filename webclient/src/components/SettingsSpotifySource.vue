<template>
  <div>
    <div class="section-content">
      <div class="header">
        Username
      </div>
      <div class="body">
        <input type="text" v-model="username"></input>
      </div>
      <div class="description">
        <span v-if="ok">{{message}}</span>
        <span v-else>Spotify premium username</span>
      </div>
    </div>
    <div class="section-content">
      <div class="header">
        Password
      </div>
      <div class="body">
        <input type="password" v-model="password"></input>
        <button class="body-button" v-on:click="activate">activate</button>
      </div>
      <div class="description">
        <span v-if="ok"></span>
        <span v-else>Spotify premium password</span>
      </div>
    </div>
    <!--<div class="section-content">
      <div class="header">
      </div>
      <div class="body">
        <button class="body-button" v-on:click="activate">activate</button>
      </div>
      <div class="description">
      </div>-
    </div>-->
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
