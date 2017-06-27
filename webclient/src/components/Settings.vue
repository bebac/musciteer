<template>
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
</template>

<script>
  import ReplayGain from '@/components/SettingsReplayGain'
  import AudioOutput from '@/components/SettingsAudioOutput'
  import ContinuousPlayback from '@/components/SettingsContinuousPlayback'
  import LocalSource from '@/components/SettingsLocalSource'
  import SpotifySource from '@/components/SettingsSpotifySource'
  import SpotifyWeb from '@/components/SettingsSpotifyWeb'
  import SpotifyImport from '@/components/SettingsSpotifyImport'

  export default {
    components: {
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

  #m-settings
  {
    color: #666;
    font-family: Arial;
    font-size: 0.88em;
    margin-bottom: 2em;

    .header
    {
      display: flex;
      flex-direction: column;
      padding: 1em;
      //border-bottom: 1px solid #eee;
    }

    .section, .section-header
    {
      padding: 1em;
    }

    .section-header
    {
      border-top: 1px solid #eee;
      padding: 1em;
    }

    .section-header
    {
      margin-top: 1em;
    }

    .section-content
    {
      display: flex;
      justify-content: flex-start;
      align-items: center;

      label
      {
        flex: 1;
        //align-self: flex-start;
        padding: 0.25em 1em;
        font-weight: bold;
        text-align: right;
      }

      div
      {
        flex: 2;
        padding: 0.25em 1em;
      }

      span
      {
        flex: 2;
        //align-self: flex-start;
        padding: 0.25em 1em;
      }
    }

    h1
    {
      font-size: 1.25em;
    }

    h2
    {
      font-size: 1.125em;
    }

    input[type="text"], input[type="password"]
    {
      padding: 0.5em 1em;
      width: 100%;
      border-radius: 20px;
      border: 2px solid lighten(#D4D7DB, 5%);
    }

    input[type="text"]:focus, input[type="password"]:focus
    {
      outline: none;
      border: 2px solid darken(#D4D7DB, 5%);
    }

    button
    {
      background: lighten(#D4D7DB, 5%);
      //background: linear-gradient(to bottom, #D4D7DB, lighten(#D4D7DB, 5%));
      border: 1px solid darken(#D4D7DB, 0%);
      //border: 1px solid #eee;
      //border: none;
      border-radius: 20px;
      margin: 0.3em;
      padding: 0.5em 20px;
      outline: none;
      font-weight: bold;
      color: #666;
      position: relative;

      .armed {
        display: none;
      }

      .busy {
        position: absolute;
        top: 50%;
        transform: translateY(-50%);
        right: 0.3em;
        width: 1.5em;
        //border: 1px solid #666;

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
