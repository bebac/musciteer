<template>
  <div class="connection-container" v-bind:style="{ display: display }">
    <div class="connection">
      <h1>Connection Lost</h1>
    </div>
  </div>
</template>

<script>
  export default {
    data: function () {
      return {
        display: 'none'
      }
    },

    musciteer: {
      onopen: function (data) {
        this.display = 'none'
      },
      onclose: function (data) {
        this.display = 'flex'
      }
    }
  }
</script>

<style lang="scss">
  @import "../styles/mixins.scss";

  .connection-container
  {
    position: fixed;
    top: 0;
    bottom: 0;
    overflow: auto;
    width: 100%;
    display: none;
    align-items: center;
    background:rgba(0,0,0,0.75);
    z-index: 10;

    .connection
    {
      width: 50%;
      text-align: center;
      padding: 1em;
      font-size: 2em;
      color: #eee;
      margin: 0 auto;
    }
  }
</style>
