<template>
  <tr v-on:click="queue">
    <td>{{track.title}}</td>
    <td>{{artists}}</td>
    <td>{{track.album.title}}</td>
    <td>{{track.play_count}}</td>
    <td>{{track.skip_count}}</td>
  </tr>
</template>

<script>
  export default {
    props: [
      'track'
    ],

    data: function () {
      return {
      }
    },

    computed: {
      artists: function () {
        return this.track.artists.map((artist) => { return artist.name }).join(', ')
      }
    },

    methods: {
      queue: function () {
        this.$musciteer.send({ event: 'queue', data: this.track.id })
      }
    }
  }
</script>

<style lang="scss">
</style>
