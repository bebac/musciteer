<template>
  <div id="app">
    <div id="header">
      <m-header></m-header>
    </div>
    <div id="main">
      <m-queue-update></m-queue-update>
      <keep-alive>
        <router-view></router-view>
      </keep-alive>
    </div>
    <div id="footer">
      <m-footer></m-footer>
    </div>
    <m-connection></m-connection>
  </div>
</template>

<script>
  import Header from '@/components/Header'
  import Footer from '@/components/Footer'
  import QueueUpdate from '@/components/QueueUpdate'
  import Connection from '@/components/Connection'

  export default {
    name: 'app',

    components: {
      'm-header': Header,
      'm-footer': Footer,
      'm-queue-update': QueueUpdate,
      'm-connection': Connection
    }
  }
</script>

<style lang="scss">
  @import "styles/mixins.scss";
  @import "styles/variables.scss";
  @import "styles/base.scss";

  #app
  {
    display: flex;
    flex-direction: column;
    height: 100%;

    #header
    {
      flex: 0 0 auto;
      background: linear-gradient(to bottom, saturate($color-base, 2%), transparentize($color-base, 0.4));
      //border-top: 1px solid desaturate(darken($color-base, 5%), 10%);
      border-bottom: 1px solid desaturate(darken($color-base, 10%), 5%);
    }

    #main
    {
      position: relative;
      flex: 1;
      overflow: auto;
    }

    #footer
    {
      flex: 0 0 4em;
      background-color: $color-base;
    }
  }
</style>
