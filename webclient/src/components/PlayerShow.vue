<template>
  <div
    id="player-show-button"
    class="button"
    v-bind:class="{ 'button-click': clicked }"
    v-on:click="show"
    v-on:animationend="clicked = !clicked"
  >
    <svg
      xmlns="http://www.w3.org/2000/svg"
      viewBox="0 0 200 200"
      >
      <g>
        <path d="m25,125l75,-50l75,50" fill="none" stroke-width="12"></path>
        <circle cx="100" cy="100" r="100" fill-opacity="0" stroke-width="0"></circle>
      </g>
    </svg>
  </div>
</template>

<script>
  export default {
    name: 'm-player-show',

    data () {
      return {
        clicked: false
      }
    },

    methods: {
      show: function (event) {
        this.clicked = !this.clicked
        this.$router.push('/player')
      }
    }
  }
</script>

<style lang="scss">
  @import "../styles/mixins.scss";
  @import "../styles/button.scss";

  #player-show-button
  {
    @include size(2em);
  }
</style>
