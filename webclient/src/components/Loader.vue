<template>
  <div class="loader-container">
    <div class="loader">
    </div>
  </div>
</template>

<script>
  export default {
    data: function () {
      return {
      }
    }
  }
</script>

<style lang="scss">
  @import "../styles/mixins.scss";

  .loader-container
  {
    height: 100%;
    display: flex;
    justify-content: center;
    align-items: center;
  }

  .loader
  {
    border: 16px solid #f3f3f3;
    border-top: 16px solid #666;
    border-radius: 50%;
    width: 120px;
    height: 120px;
    @include animation('spin 2s linear infinite');
  }

  @include keyframes(spin)
  {
    0%
    {
      transform: rotate(0deg);
    }
    100%
    {
      transform: rotate(360deg);
    }
  }
</style>
