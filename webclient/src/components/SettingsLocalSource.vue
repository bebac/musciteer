<template>
  <div class="section-content">
    <div class="setting-header">
      Directories
    </div>
    <div class="setting-body">
      <ul>
        <li v-for="dir in directories">
          <input type="text" v-model="dir.value" v-on:change="set" v-on:keyup.13="set"></input>
        </li>
        <li>
          <input type="text" placeholder="Add new directory" v-model="newDirectory" v-on:change="set"></input>
        </li>
        <li>
          <button v-on:click="scan">
            <span>scan</span>
            <div v-if="scanning" class="busy">
              <div></div>
            </div>
            <div v-else class="armed">
            </div>
          </button>
        </li>
      </ul>
    </div>
    <div class="setting-description">
      <span>Configure directories to scan for local music files.</span>
    </div>
  </div>
</template>

<script>
  import axios from 'axios'

  export default {
    data: function () {
      return {
        scanning: false,
        directories: [],
        newDirectory: ''
      }
    },

    methods: {
      // Convert directory list to an array of objects.
      set_directories: function (directories) {
        this.directories = directories.map(function (directory) {
          return { value: directory }
        })
      },
      // Convert directory inputs into a an array of strings.
      get_directories: function () {
        let directories = this.directories.map(function (dir) {
          return dir.value
        })

        if (this.newDirectory) {
          directories.push(this.newDirectory)
        }

        // Remove empty directories.
        directories = directories.filter(function (dir) { return dir })
        return directories
      },
      // Post directories to server
      set: function () {
        axios.post('/api/sources/local/directories', this.get_directories())
        .then(
          response => { this.set_directories(response.data) }
        )
        this.newDirectory = ''
      },
      scan: function () {
        if (!this.scanning) {
          axios.post('/api/sources/local/scan')
          .then(
            response => {
              this.scanning = false
            }
          )
          this.scanning = true
        }
      }
    },

    created () {
      axios.get('/api/sources/local/directories')
      .then(
        response => { this.set_directories(response.data) }
      )
    }
  }
</script>

<style lang="scss">
</style>
