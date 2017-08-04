<template>
  <div
    id="player-skip-button"
    class="button"
    v-bind:class="{ 'button-click': clicked }"
    v-on:click="toggle"
    v-on:animationend="clicked = !clicked"
  >
    <svg v-once xmlns="http://www.w3.org/2000/svg" viewBox="0 0 200 200"
      >
      <g>
        <path d="M 90,60 120,100 90,140" fill="none" stroke-width="5"></path>
        <circle cx="100" cy="100" r="100" fill-opacity="0" stroke-width="0"></circle>
      </g>
    </svg>
  </div>
</template>

<script>
  export default {
    name: 'm-player-skip',

    data () {
      return {
        clicked: false,
        playing: false
      }
    },

    created () {
    },

    methods: {
      toggle: function (event) {
        this.clicked = !this.clicked
        this.skip()
      },
      skip: function () {
        this.$musciteer.send({ event: 'skip' })
      }
    },

    musciteer: {
      player_state: function (data) {
        this.playing = data.state === 1
      }
    }
  }
</script>

<style lang="scss" scoped>
  @import "../styles/mixins.scss";
  @import "../styles/button.scss";

  .button
  {
    @include size(2.5em);
  }
</style>
