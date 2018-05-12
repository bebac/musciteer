<template>
  <div class="header">
    <div id="m-header">
      <div>
        <m-header-back></m-header-back>
        <div id="m-header-logo">
          Musciteer
        </div>
        <m-menu-show v-on:click="show_nav = !show_nav"></m-menu-show>
      </div>
      <div>
        <m-header-search v-on:click=""></m-header-search>
      </div>
      <div v-show="show_nav">
        <nav>
          <router-link to='/albums'>Albums</router-link>
          <router-link to='/tracks'>Tracks</router-link>
          <router-link to='/playlists'>Playlists</router-link>
          <router-link to='/settings'>Settings</router-link>
        </nav>
      </div>
    </div>
  </div>
</template>

<script>
  import HeaderBack from '@/components/HeaderBack'
  import HeaderSearch from '@/components/HeaderSearch'
  import MenuShow from '@/components/MenuShow'

  export default {
    components: {
      'm-header-back': HeaderBack,
      'm-header-search': HeaderSearch,
      'm-menu-show': MenuShow
    },

    data () {
      return {
        show_nav: true,
        menu_button_shown: false
      }
    },

    methods: {
      resize: function () {
        let x = document.getElementById('menu-show-button')
        let d = window.getComputedStyle(x)
        if (d.display !== 'none') {
          if (this.menu_button_shown === false) {
            this.show_nav = false
            this.menu_button_shown = true
          }
        } else {
          this.show_nav = true
          this.menu_button_shown = false
        }
      }
    },

    created: function () {
      window.addEventListener('resize', this.resize)
    },

    beforeDestroy: function () {
      window.removeEventListener('resize', this.resize)
    }
  }
</script>

<style lang="scss">
  @import "../styles/variables.scss";

  #m-header-logo
  {
    padding: 0.33em;
    color: rgba(201, 161, 0, 1);
    font-size: 1.16em;
    font-weight: 400;
    text-shadow: 2px 2px 2px rgba(128, 128, 128, 0.3);
    letter-spacing: 0.15em;
  }

  #m-header
  {
    display: flex;
    @media (max-width: $break) {
      flex-direction: column;
    }
    height: 100%;
    font-family: 'Montserrat', sans-serif;

    >div
    {
      display: flex;
      align-items: center;
      flex: 1;
    }

    >div:nth-child(1)
    {
      display: flex;
      align-items: center;
      flex: 1;
      padding: 1em;

      >div:nth-child(2)
      {
        display: flex;
        align-items: center;
        justify-content: center;
        flex: 1;
        height: 2em;
      }

      >div:nth-child(3)
      {
        flex: 0 0 auto;
      }
    }

    >div:nth-child(2)
    {
      @media (min-width: $break) {
        justify-content: center;
      }
      @media (max-width: $break) {
        justify-content: center;
        background-color: #444;
      }
    }

    nav
    {
      @media (min-width: $break)
      {
        display: flex;
        align-items: center;
        height: 2em;

        a
        {
          padding: 0 1em;
          color: #666;
        }

        a:link
        {
          text-decoration: none;
        }

        a:visited
        {
          text-decoration: none;
        }

        a:hover
        {
        }

        a.router-link-active
        {
          color: rgba(201, 161, 0, 1);
        }
      }

      @media (max-width: $break)
      {
        display: flex;
        flex-direction: column;
        text-align: center;
        flex: 1;

        a
        {
          padding: 0.5em;
          color: #aaa;
        }

        a:link
        {
          text-decoration: none;
        }

        a:visited
        {
          text-decoration: none;
        }

        a:hover
        {
          background-color: #333333;
        }

        a.router-link-active
        {
        }
      }
    }
  }
</style>
