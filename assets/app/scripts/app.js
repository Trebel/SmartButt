angular.module('app', [])

.controller('controller', function($scope, $http, $interval) {
  $http({
    url: "/api/toilets",
    method: "GET",
    headers: {
        "Content-Type": "application/json; charset=utf-8"
    }
  }).success(function(response){
      $scope.loos = response;
  }).error(function(error){
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
        "id":"234233",
        "floor":"3",
        "bathroom":"1",
        "stall":"1",
        "occupied":false,
        "occupyChance":50.86311145219952,
        "tags":["handicap"]
      },
      {
        "id":"3910361",
        "floor":"2",
        "bathroom":"1",
        "stall":"1",
        "occupied":true,
        "occupyChance":83.86311145219952,
        "tags":["handicap","private"]
      }];
  });

  $interval(
    function () {
      $http({
        url: "/api/toilets",
        method: "GET",
        headers: {
            "Content-Type": "application/json; charset=utf-8"
        }
      }).success(function(response){
          $scope.loos = response;
      }).error(function(error){
          $scope.error = error;
      });
    }, 5000);
})

.filter('capitalize', function() {
    return function(input, scope) {
        if (input!=null)
            return input.substring(0,1).toUpperCase()+input.substring(1);
    }
});