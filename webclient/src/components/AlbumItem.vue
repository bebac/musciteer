<template>
  <li>
    <div v-if="item.is_track" v-on:mouseleave="mouseleave">
      <div class="track" v-on:click="queue(item)">
        <div>
          <div class="tn">{{ item.tn }}</div>
          <div class="title">
            {{ item.title }}
            <span class="more" v-on:click.stop="showDetails = !showDetails">
              <svg v-once xmlns="http://www.w3.org/2000/svg" viewBox="0 0 200 200">
                <g>
                  <path d="M 40,130 160,130 M 100,70 100,190" fill="none" stroke-width="16"></path>
                  <circle cx="100" cy="100" r="100" fill-opacity="0" stroke-width="0"></circle>
                </g>
              </svg>
            </span>
          </div>
        </div>
        <div>
          <div class="tags">
            <ul>
              <li v-for="tag in tags">
                <span>{{ tag }}</span>
              </li>
            </ul>
          </div>
          <div class="duration">{{ item.duration_mmss }}</div>
        </div>
      </div>
      <div v-if="showDetails" class="track-details">
        <m-tag-input v-model="tags" v-on:commit="commit"></m-tag-input>
      </div>
    </div>
    <div v-if="item.is_header" class="disc-header">
      Disc {{ item.dn }}
    </div>
  </li>
</template>

<script>
  import axios from 'axios'
  import TagInput from '@/components/TagInput'

  export default {
    components: {
      'm-tag-input': TagInput
    },

    props: [
      'item'
    ],

    data () {
      return {
        showDetails: false,
        tags: this.item.tags
      }
    },

    methods: {
      queue: function (track) {
        this.$musciteer.send({ event: 'queue', data: track.id })
      },
      goto: function (track) {
        this.$router.push('/tracks/' + track.id)
      },
      mouseleave: function () {
        this.showDetails = false
        this.commit()
      },
      commit: function () {
        if (this.tags !== this.item.tags) {
          this.postTags(this.tags.filter(Boolean))
        }
      },
      postTags: function (tags) {
        axios.post('/api/tracks/' + this.item.id + '/tags', tags)
        .then(
          response => { this.tags = this.item.tags = response.data }
        )
      }
    }
  }
</script>

<style lang="scss" scoped>
  @import "../styles/mixins.scss";
  @import "../styles/variables.scss";
  @import "../styles/button.scss";

  li
  {
    display: flex;
    border-bottom: 1px solid #eee;

    >div
    {
      width: 100%;
    }

    .disc-header
    {
      padding: 0.66em 1em;
      font-weight: bold;
    }

    .track
    {
      display: flex;
      padding: 0.66em 1em;

      >div:nth-child(1)
      {
        display: flex;
        justify-content: flex-start;
        align-items: center;
        flex: 3 1 auto;
        min-width: 0;
      }

      >div:nth-child(2)
      {
        display: flex;
        justify-content: flex-end;
        align-items: center;
        flex: 1;
      }
    }

    .track-details
    {
      padding: 0em 1em 0.66em 2.33em;

      label
      {
        font-size: 0.75em;
        font-weight: bold;
      }

      input
      {
        color: #666;
        width: 100%;
        padding: 0.33em 0.66em;
        border: 1px solid #eee;
        border-radius: 20px;

        &:focus, &:hover
        {
          outline: none;
        }

        @include placeholder
        {
          color: #ccc;
        }
      }
    }

    @media (min-width: $break)
    {
      .more
      {
        svg
        {
          @include size(1em);
          stroke: #bbb;
        }

        &:hover
        {
          svg
          {
            stroke: #666;
          }
        }
      }
    }

    @media (max-width: $break)
    {
      .more
      {
        display: none;
      }
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

    .tags
    {
      $item-vspace: 0.155em;
      $item-hspace: 0.155em;

      flex: 1;

      margin-top: -$item-vspace;
      margin-bottom: -$item-vspace;

      ul
      {
        display: flex;
        justify-content: flex-end;
        flex-wrap: wrap;
        padding-right: 1em;

        li
        {
          padding: $item-vspace $item-hspace;
          border: none;

          span
          {
            font-size: 0.77em;
            background-color: saturate(adjust-hue(rgba(241, 239, 236, 1), 180), 100%);
            padding: $item-vspace 1em;
            border-radius: 10px;
          }
        }
      }
    }

    .duration
    {
      padding-right: 0.66em;
    }
  }

  li:not(.disc-header):hover {
    background-color: lighten(complement(rgba(241, 239, 236, 1)), 4%);
  }
</style>
