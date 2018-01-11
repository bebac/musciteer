<template>
  <div id="settings-page">
    <div id="header">
      <m-header></m-header>
    </div>
    <div id="main">
      <m-queue-update></m-queue-update>
      <div id="m-settings">
        <div class="header">
          <h1>Settings</h1>
        </div>
        <div class="section-header">
          <h2>Audio</h2>
        </div>
        <m-audio-output></m-audio-output>
        <m-replay-gain></m-replay-gain>
        <div class="section-header">
          <h2>Continuous Playback</h2>
        </div>
        <m-continuous-playback></m-continuous-playback>
        <div class="section-header">
          <h2>Local Source</h2>
        </div>
        <m-local-source></m-local-source>
        <div class="section-header">
          <h2>Spotify Source</h2>
        </div>
        <m-spotify-source></m-spotify-source>
        <div class="section-header">
          <h2>Spotify Web</h2>
        </div>
        <m-spotify-web></m-spotify-web>
        <div class="section-header">
          <h2>Spotify Import</h2>
        </div>
        <m-spotify-import></m-spotify-import>
      </div>
    </div>
    <div id="footer">
      <m-footer></m-footer>
    </div>
  </div>
</template>

<script>
  import Header from '@/components/Header'
  import Footer from '@/components/Footer'
  import QueueUpdate from '@/components/QueueUpdate'
  import ReplayGain from '@/components/SettingsReplayGain'
  import AudioOutput from '@/components/SettingsAudioOutput'
  import ContinuousPlayback from '@/components/SettingsContinuousPlayback'
  import LocalSource from '@/components/SettingsLocalSource'
  import SpotifySource from '@/components/SettingsSpotifySource'
  import SpotifyWeb from '@/components/SettingsSpotifyWeb'
  import SpotifyImport from '@/components/SettingsSpotifyImport'

  export default {
    components: {
      'm-header': Header,
      'm-footer': Footer,
      'm-queue-update': QueueUpdate,
      'm-audio-output': AudioOutput,
      'm-replay-gain': ReplayGain,
      'm-continuous-playback': ContinuousPlayback,
      'm-local-source': LocalSource,
      'm-spotify-source': SpotifySource,
      'm-spotify-web': SpotifyWeb,
      'm-spotify-import': SpotifyImport
    },

    data: function () {
      return {
      }
    }
  }
</script>

<style lang="scss">
  @import "../styles/mixins.scss";
  @import "../styles/variables.scss";

  #settings-page
  {
    background-color: #fff;
  }

  #m-settings
  {
    color: #666;
    font-family: Arial;
    font-size: 1em;
    margin: 1em 1em 2em 1em;

    input, select
    {
      font-family: Arial;
    }

    h1
    {
      font-size: 1.125em;
    }

    h2
    {
      font-size: 1em;
    }

    .section, .section-header
    {
      padding: 1em;
    }

    .section-header
    {
      margin-top: 1em;
      border-top: 1px solid #eee;
    }

    .section-content
    {
      display: flex;
      align-items: flex-start;

      .header, .body, .description
      {
        flex: 1;
        padding: 0.33em 1em;
      }

      .header
      {
        font-weight: bold;
        text-align: right;
      }

      .body
      {
        position: relative;

        ul
        {
          list-style: none;

          li:not(:first-child)
          {
            padding-top: 1.33em;
          }
        }
      }

      .body-button
      {
        margin: 1.33em 0;
      }

      .description
      {
        text-align: left;
      }

      input[type="text"], input[type="password"]
      {
        color: #666;
        $padding-bottom: 0.33em;

        padding-bottom: calc(#{$padding-bottom} + 1px);
        width: 100%;
        border: none;
        border-bottom: 1px solid lighten(#D4D7DB, 5%);

        &:focus, &:hover
        {
          outline: none;
          padding-bottom: $padding-bottom;
          border-bottom: 2px solid lighten(#ecbb71, 0%);
        }

        @include placeholder
        {
          color: #ccc;
        }
      }

      .select-dropdown-icon
      {
        position: absolute;
        bottom: 0.66em;
        right: 1em;
        display: flex;
        align-items: center;
        justify-content: flex-end;
        pointer-events: none;

        svg
        {
          @include size(1em);
          stroke: #666;
          fill: #666;
        }
      }

      select
      {
        $padding-bottom: 0.33em;

        padding-bottom: calc(#{$padding-bottom} + 1px);
        width: 100%;
        outline: none;
        border: none;
        background-color: #fff;
        border-bottom: 1px solid lighten(#D4D7DB, 5%);
        -webkit-appearance: none;
        -moz-appearance: none;

        &:focus
        {
          border: none;
          padding-bottom: $padding-bottom;
          border-bottom: 2px solid lighten(#ecbb71, 0%);
        }

        &:hover
        {
          outline: none;
          padding-bottom: $padding-bottom;
          border-bottom: 2px solid lighten(#ecbb71, 0%);
        }

        &:-moz-focusring
        {
          color: transparent;
          text-shadow: 0 0 0 #000;
          padding-bottom: $padding-bottom;
          border-bottom: 2px solid lighten(#ecbb71, 0%);
        }
      }
    }

    button
    {
      background-color: $color-btn-base;
      border: none;
      border-radius: 20px;
      padding: 0.5em 30px;
      outline: none;
      font-weight: bold;
      color: #666;
      position: relative;

      &:hover {
        background-color: $color-btn-hover;
      }

      .armed {
        display: none;
      }

      .busy {
        position: absolute;
        top: 50%;
        transform: translateY(-50%);
        right: 0.5em;

        >div {
          border-radius: 100%;
          border: .15em solid #666;
          border-top-color: rgba(212, 215, 219, 0);
          animation: spin 1s infinite linear;
          padding: 0 0 0.5em 0.5em;
        }
      }

      @keyframes spin {
          from {transform:rotate(0deg);}
          to {transform:rotate(360deg);}
      }

      a
      {
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
    }
  }
</style>
