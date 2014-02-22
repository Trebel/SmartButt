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
      $scope.error = error;
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