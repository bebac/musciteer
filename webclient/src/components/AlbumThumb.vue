<template>
  <router-link :to="{ name: 'Album', params: { albumId: album.id }}" tag="li">
    <div class="cover">
      <img v-bind:src="album.cover" onerror="this.style.display='none'">
    </div>
    <div class="label">
      <h1>{{ album.title }}</h1>
      <h2>{{ album.artist.name }}</h2>
    </div>
  </router-link>
</template>

<script>
  export default {
    props: [
      'album'
    ]
  }
</script>

<style lang="scss" scoped>
  @import "../styles/mixins.scss";
  @import "../styles/variables.scss";

  @media (min-width: $break)
  {
    li
    {
      list-style: none;
      margin: 0 1em 2em 1em;
      width: 12em;
      color: #666;

      .cover
      {
        position: relative;
        height: 12em;
        border: 1px solid #ddd;
        background-color: #eee;

        img
        {
          position: absolute;
          top: 0;
          left: 0;
          min-width: 100%;
          max-width: 100%;
          max-height: 12em;
        }
      }

      .label
      {
        h1, h2
        {
          margin: 0.33em 0.33em 0.33em 0.33em;
          text-align: left;
          text-overflow: ellipsis;
          white-space: nowrap;
          overflow: hidden;
        }
        h1
        {
          font-size: 1em;
          font-weight: bold;
        }
        h2
        {
          font-size: 0.88em;
          font-weight: normal;
        }
      }
    }
  }

  @media (max-width: $break)
  {
    $cover-size: 4em;

    li
    {
      display: flex;
      border-bottom: 1px solid #eee;
      padding: 0.33em;
      color: #666;

      .cover
      {
        position: relative;
        height: $cover-size;

        >img
        {
          //position: absolute;
          //top: 0;
          min-width: $cover-size;
          max-width: $cover-size;
          max-height: $cover-size;
          border: 1px solid #eee;
        }
      }

      .label
      {
        min-width: 0;

        h1, h2
        {
          margin: 0.33em 0.33em 0.33em 0.33em;
          text-align: left;
          text-overflow: ellipsis;
          white-space: nowrap;
          overflow: hidden;
        }
        h1
        {
          font-size: 1em;
          font-weight: bold;
        }
        h2
        {
          font-size: 0.88em;
          font-weight: normal;
        }
      }
    }
  }
</style>
