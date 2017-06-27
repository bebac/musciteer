<template>
  <tr v-on:click="queue">
    <td>{{track.title}}</td>
    <td>{{track.artists[0].name}}</td>
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

    methods: {
      queue: function () {
        this.$musciteer.send({ event: 'queue', data: this.track.id })
      }
    }
  }
</script>

<style lang="scss">
</style>
