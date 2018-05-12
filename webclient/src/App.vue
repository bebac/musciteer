<template>
  <div id="app">
    <!--<transition :name=transition>-->
      <keep-alive>
        <router-view></router-view>
      </keep-alive>
    <!--</transition>-->
    <m-connection></m-connection>
  </div>
</template>

<script>
  import Connection from '@/components/Connection'

  export default {
    name: 'app',

    components: {
      'm-connection': Connection
    },

    data () {
      return {
        transition: 'fade-in'
      }
    },

    watch: {
      '$route' (to, from) {
        if (to.name === 'Player') {
          this.transition = 'slide-up'
        } else if (from.name === 'Player') {
          this.transition = 'slide-down'
        } else {
          this.transition = 'fade-in'
        }
      }
    }
  }
</script>

<style lang="scss">
  @import "styles/mixins.scss";
  @import "styles/variables.scss";
  @import "styles/base.scss";

  #app
  {
    height: 100%;

    >div
    {
      display: flex;
      flex-direction: column;
      height: 100%;

      .header
      {
        flex: 0 0 auto;
        //border-top: 1px solid desaturate(darken($color-base, 5%), 10%);
        border-bottom: 1px solid desaturate(darken($color-base, 10%), 5%);
      }

      .main
      {
        position: relative;
        flex: 1;
        overflow: auto;
      }

      .footer
      {
        flex: 0 0 4em;
        background-color: $color-base;
      }
    }
  }

  .fade-in-enter {
    opacity: 0;
  }

  .fade-in-enter-active {
    transition: all 1s ease;
  }

  .fade-in-leave-active {
  }

  .slide-up-enter {
    opacity: 0;
    -webkit-transform: translate(0, 100%);
    transform: translate(0, 100%);
  }

  .slide-up-enter-active, .slide-down-enter-active {
    transition: all .25s linear;
  }

  .slide-up-leave-active {
  }


  .slide-down-enter {
    opacity: 0;
    -webkit-transform: translate(0, -100%);
    transform: translate(0, -100%);
  }

  .slide-down-leave-active {
  }
</style>
