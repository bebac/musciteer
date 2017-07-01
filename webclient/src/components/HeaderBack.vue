<template>
  <div
    id="header-back-button"
    v-bind:class="classes"
    v-on:click="toggle"
    v-on:animationend="clicked = !clicked"
  >
    <svg v-once xmlns="http://www.w3.org/2000/svg" viewBox="0 0 200 200"
      >
      <g>
        <path d="M 90,25 40,100 90,175" fill="none" stroke-width="12"></path>
        <circle cx="100" cy="100" r="100" fill-opacity="0" stroke-width="0"></circle>
      </g>
    </svg>
  </div>
</template>

<script>
  export default {
    name: 'm-header-back',

    data () {
      return {
        clicked: false,
        route: this.$router.currentRoute.name
      }
    },

    computed: {
      classes: function () {
        return {
          'button': true,
          'button-click': this.clicked
        }
      }
    },

    methods: {
      toggle: function (event) {
        this.clicked = !this.clicked
        this.back()
      },
      back: function () {
        this.$router.go(-1)
      }
    }
  }
</script>

<style lang="scss">
  @import "../styles/mixins.scss";
  @import "../styles/button.scss";

  #header-back-button
  {
    visibility: hidden;
    @include size(2em);
  }
</style>
