<template>
  <div
    id="player-hide-button"
    class="button"
    v-bind:class="{ 'button-click': clicked }"
    v-on:click="hide"
    v-on:animationend="clicked = !clicked"
  >
    <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 200 200"
      >
      <g>
        <path d="m25,75l75,50l75,-50" fill="none" stroke-width="12"></path>
        <circle cx="100" cy="100" r="100" fill-opacity="0" stroke-width="0"></circle>
      </g>
    </svg>
  </div>
</template>

<script>
  export default {
    name: 'm-player-hide',

    data () {
      return {
        clicked: false
      }
    },

    methods: {
      hide: function (event) {
        this.clicked = !this.clicked
        // console.log(window.history)
        if (window.history.state !== null) {
          this.$router.go(-1)
        } else {
          this.$router.push('/')
        }
      }
    }
  }
</script>

<style lang="scss" scoped>
  @import "../styles/mixins.scss";
  @import "../styles/button.scss";

  .button
  {
    @include size(2em);
  }
</style>
