<template>
  <input type="text" placeholder="Enter tags, comma separated" v-model="tags" v-on:keyup.enter="commit"/>
</template>

<script>
  export default {
    props: [
      'value'
    ],

    data: function () {
      return {
      }
    },

    computed: {
      tags: {
        get: function () {
          return (this.value || []).join(',')
        },
        set: function (value) {
          this.$emit('input', value.split(','))
        }
      }
    },

    methods: {
      commit: function () {
        this.$emit('commit')
      }
    }
  }
</script>

<style lang="scss">
</style>
