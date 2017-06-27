<template>
  <div
    id="menu-show-button"
    class="button"
    v-bind:class="{ 'button-click': clicked }"
    v-on:click="show"
    v-on:animationend="clicked = !clicked"
  >
    <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 200 200">
      <g stroke-width="16" stroke-linecap="round">
        <path d="M45,60h110"/>
        <path d="M45,100h110"/>
        <path d="M45,140h110"/>
      </g>
    </svg>
  </div>
</template>

<script>
  export default {
    data () {
      return {
        clicked: false
      }
    },

    methods: {
      show: function (event) {
        this.clicked = !this.clicked
        this.$emit('click')
      }
    }
  }
</script>

<style lang="scss">
  @import "../styles/mixins.scss";
  @import "../styles/variables.scss";

  #menu-show-button
  {
    @include size(2em);

    svg
    {
      stroke: #666;
    }

    @media (min-width: $break)
    {
      display: none;
    }

    @media (max-width: $break)
    {
      display: inline-block;
    }
  }
</style>
