<template>
  <div id="m-footer">
    <div class="left">
      <m-player-show></m-player-show>
      <m-player-mini></m-player-mini>
    </div>
    <div class="center">
      <m-player-toggle></m-player-toggle>
    </div>
    <div class="right"></div>
  </div>
</template>

<script>
  import PlayerShow from '@/components/PlayerShow'
  import PlayerToggle from '@/components/PlayerToggle'
  import Player from '@/components/FooterPlayer'

  export default {
    components: {
      'm-player-show': PlayerShow,
      'm-player-toggle': PlayerToggle,
      'm-player-mini': Player
    },

    data () {
      return {}
    }
  }
</script>

<style lang="scss">
  @import "../styles/mixins.scss";
  @import "../styles/variables.scss";

  #m-footer
  {
    display: flex;
    align-items: center;
    justify-content: center;
    height: 100%;
    box-shadow: 0px -2px 15px rgba(140, 139, 127, 0.4);
    z-index: 1;
    overflow: hidden;

    >div
    {
      height: 100%;
    }

    .left
    {
      display: flex;
      align-items: center;
      justify-content: flex-start;
      flex: 1;
      padding-left: 1em;
      text-overflow: ellipsis;
      white-space: nowrap;
      overflow: hidden;
    }

    .center
    {
      display: flex;
      align-items: center;
      flex: 0 0 auto;
    }

    .right
    {
      flex: 1;
    }

    @media (max-width: $break)
    {
      .center
      {
        display: none;
      }

      .right
      {
        display: none;
      }
    }
  }
</style>
