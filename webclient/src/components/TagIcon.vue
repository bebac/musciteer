<template>
  <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 200 200">
  <defs>
    <mask id="hole">
      <rect width="100%" height="100%" fill="white"/>
      <circle cx="150" cy="150" r="20" fill="black"/>
    </mask>
  </defs>
    <g stroke-width="0" stroke-linecap="round">
      <path d="M 10,100 100,10 190,100 190,190 100,190 10,100" stroke-width="24" stroke-linecap="round" mask="url(#hole)"/>
      <!--<circle cx="100" cy="100" r="100" fill-opacity="1" stroke-width="0"></circle>-->
      <!--<path d="M45,100h110"/>-->
      <!--<path d="M45,140h110"/>-->
    </g>
  </svg>
</template>
