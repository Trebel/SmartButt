angular.module('app', [])

.controller('controller', function($scope) {
  $scope.loos = [
    {
      "id":"3910360",
      "floor":"1",
      "bathroom":"2",
      "stall":"2",
      "occupied":false,
      "occupyChance":19.84247250016779,
      "tags":[""]
    },
    {
      "id":"3910361",
      "floor":"2",
      "bathroom":"1",
      "stall":"1",
      "occupied":true,
      "occupyChance":70.86311145219952,
      "tags":["handicap","private"]
    }
  ];
});