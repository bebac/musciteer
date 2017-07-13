import Vue from 'vue'
import Router from 'vue-router'
import Album from '@/components/Album'
import Albums from '@/components/Albums'
import Tracks from '@/components/Tracks'
import Player from '@/components/Player'
import Playlists from '@/components/Playlists'
import Settings from '@/components/Settings'

Vue.use(Router)

export default new Router({
  mode: 'history',
  routes: [
    {
      path: '/', redirect: '/albums'
    },
    {
      path: '/albums',
      name: 'Albums',
      component: Albums
    },
    {
      path: '/albums/:albumId',
      name: 'Album',
      component: Album,
      props: (route) => {
        route.params
      }
    },
    {
      path: '/tracks',
      name: 'Tracks',
      component: Tracks
    },
    {
      path: '/player',
      name: 'Player',
      component: Player
    },
    {
      path: '/playlists',
      name: 'Playlists',
      component: Playlists
    },
    {
      path: '/settings',
      name: 'Settings',
      component: Settings
    }
  ]
})
