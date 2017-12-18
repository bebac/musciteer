<template>
  <div
    id="search-show-button"
    class="button"
    v-bind:class="{ 'button-click': clicked }"
    v-on:click="show"
    v-on:animationend="clicked = !clicked"
  >
    <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 200 200">
      <g stroke-width="8">
        <circle cx="100" cy="100" r="30" fill="none"></circle>
        <path d="M125,125l25,25"/>
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

  #search-show-button
  {
    @include size(2.66em);

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
      display: none;
    }
  }
</style>
