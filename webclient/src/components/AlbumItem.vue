<template>
  <li
    v-on:click="queue(item)"
    :class="item.is_header ? 'disc-header' : 'track'"
  >
    <div v-if="item.is_track">
      <div class="tn">{{ item.tn }}</div>
      <div class="title">{{ item.title }}</div>
    </div>
    <div v-if="item.is_track">
      <div class="duration">{{ item.duration_mmss }}</div>
    </div>
    <div v-if="item.is_header">
      Disc {{item.dn}}
    </div>
  </li>
</template>

<script>
  export default {
    props: [
      'item'
    ],

    data () {
      return {
      }
    },

    methods: {
      queue: function (item) {
        if (item.is_track) {
          this.$musciteer.send({ event: 'queue', data: item.id })
        }
      }
    }
  }
</script>

<style lang="scss" scoped>
  li
  {
    display: flex;
    padding: 0.66em 1em;
    border-bottom: 1px solid #eee;

    >div
    {
      display: flex;
    }

    >div:nth-child(1)
    {
      justify-content: flex-start;
      flex: 3;
      min-width: 0;
    }

    >div:nth-child(2)
    {
      justify-content: flex-end;
      flex: 1;
    }

    .tn
    {
      width: 1.33em;
      text-align: right;
      flex: 0 0 auto;
    }

    .title
    {
      padding-left: 0.66em;
      text-overflow: ellipsis;
      white-space: nowrap;
      overflow: hidden;
    }

    .duration
    {
      padding-right: 0.66em;
    }
  }

  li:not(.disc-header):hover {
    background-color: lighten(complement(rgba(241, 239, 236, 1)), 3%);
  }
</style>
