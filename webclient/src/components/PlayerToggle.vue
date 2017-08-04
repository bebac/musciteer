<template>
  <div
    id="player-toggle-button"
    class="button"
    v-bind:class="{ 'button-click': clicked }"
    v-on:click="toggle"
    v-on:animationend="clicked = !clicked"
  >
    <svg
      xmlns="http://www.w3.org/2000/svg"
      viewBox="0 0 200 200"
      >
      <g>
        <circle cx="100" cy="100" r="92" fill="none" stroke-width="5"></circle>
        <path v-if="playing" d="M75,75h50v50h-50"></path>
        <path v-else d="M90,65v70l35,-35"></path>
        <circle cx="100" cy="100" r="100" fill-opacity="0" stroke-width="0"></circle>
      </g>
    </svg>
  </div>
</template>

<script>
  export default {
    name: 'm-player-toggle',

    data () {
      return {
        clicked: false,
        playing: false
      }
    },

    created () {
      if (this.$musciteer.player_state) {
        this.playing = (this.$musciteer.player_state.state === 1)
      }
    },

    methods: {
      toggle: function (event) {
        this.clicked = !this.clicked
        this.playing === true ? this.stop() : this.play()
        // this.playing = !this.playing
      },
      play: function () {
        this.$musciteer.send({ event: 'play' })
      },
      stop: function () {
        this.$musciteer.send({ event: 'stop' })
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
